#include "BattlePassMgr.h"

#include "Bitset.h"
#include "ChangeInfoUtil.h"
#include "InventoryMgr.h"
#include "Player.h"
#include "../Config.h"
#include "../GameConstants.h"
#include "../GameSession.h"
#include "../GameServices.h"
#include "../GameTime.h"
#include "../PaymentController.h"
#include "../Resources/BinClass/BattlePassRes.h"
#include "../Resources/BinClass/ItemsRes.h"
#include "../Resources/GameData.h"
#include "../Util.h"
#include "../proto/NetMsgId.pb.h"
#include "../proto/proto_cpp/notify.pb.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <ctime>
#include <limits>
#include <string>
#include <vector>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace {
constexpr int64_t kNoDeadline = std::numeric_limits<int64_t>::max();

uint32_t ResourceIdFromKey(const std::string& key)
{
    try
    {
        return static_cast<uint32_t>(std::stoul(key));
    }
    catch (...)
    {
        return 0;
    }
}

template<typename TableT>
std::vector<uint32_t> SortedResourceIds(const TableT& table)
{
    std::vector<uint32_t> ids;
    ids.reserve(table.size());

    for (const auto& [key, data] : table)
    {
        const uint32_t id = ResourceIdFromKey(key);
        if (id != 0)
        {
            ids.push_back(id);
        }
    }

    std::sort(ids.begin(), ids.end());
    return ids;
}

uint32_t QuestStatus(uint32_t status, uint32_t cur, uint32_t max)
{
    if (status == 2)
    {
        return 2;
    }

    if (status == 1 || (max > 0 && cur >= max))
    {
        return 1;
    }

    return 0;
}

bool IsQuestComplete(const ServerProto::QuestInfoBin& quest)
{
    const uint32_t cur = quest.progress_size() > 0 ? quest.progress(0).cur() : 0;
    const uint32_t max = quest.progress_size() > 0 ? quest.progress(0).max() : 0;
    return QuestStatus(quest.status(), cur, max) == 1;
}

proto::QuestType GetQuestType(const BattlePassQuestRes& data)
{
    return data.Type == 1 ? proto::BattlePassDaily : proto::BattlePassWeekly;
}

void AddReward(proto::ChangeInfo& change, Player* player, uint32_t tid, int64_t qty)
{
    if (!player || tid == 0 || qty <= 0)
    {
        return;
    }

    player->Inventory().AddItem(tid, qty, &change);
}

proto::Quest BuildQuestProto(const ServerProto::QuestInfoBin& quest)
{
    proto::Quest out;
    out.set_id(quest.id());
    out.set_type(static_cast<proto::QuestType>(quest.type()));

    if (quest.type() == static_cast<uint32_t>(proto::BattlePassDaily))
    {
        out.set_expire(GameTime::NextDailyReset());
    }
    else if (quest.type() == static_cast<uint32_t>(proto::BattlePassWeekly))
    {
        out.set_expire(GameTime::NextWeeklyReset());
    }
    else if (quest.expire() > 0)
    {
        out.set_expire(quest.expire());
    }

    const uint32_t cur = quest.progress_size() > 0 ? quest.progress(0).cur() : 0;
    const uint32_t max = quest.progress_size() > 0 ? quest.progress(0).max() : 0;
    out.set_status(QuestStatus(quest.status(), cur, max));

    auto* progress = out.add_progress();
    progress->set_cur(cur);
    progress->set_max(max);
    return out;
}

bool EndsWith(std::string_view text, std::string_view suffix)
{
    return text.size() >= suffix.size() && text.substr(text.size() - suffix.size()) == suffix;
}

void AppendDisplayProp(const google::protobuf::Any& any, proto::ChangeInfo& displayChange)
{
    const std::string_view typeUrl = any.type_url();
    if (EndsWith(typeUrl, ".Item") || EndsWith(typeUrl, ".Res"))
    {
        displayChange.add_props()->CopyFrom(any);
        return;
    }

    if (EndsWith(typeUrl, ".HeadIcon"))
    {
        proto::HeadIcon headIcon;
        if (!headIcon.ParseFromString(any.value()))
        {
            return;
        }

        proto::Item item;
        item.set_tid(headIcon.tid());
        item.set_qty(1);
        ChangeInfoUtil::AddProp(displayChange, item);
    }
}

void AddRewardWithDisplay(Player* player, uint32_t tid, int64_t qty, proto::ChangeInfo& stateChange, proto::ChangeInfo& displayChange)
{
    if (!player || tid == 0 || qty <= 0)
    {
        return;
    }

    const int before = stateChange.props_size();
    player->Inventory().AddItem(tid, qty, &stateChange);
    for (int i = before; i < stateChange.props_size(); ++i)
    {
        AppendDisplayProp(stateChange.props(i), displayChange);
    }
}

}

ServerProto::BattlePassCompBin* BattlePassMgr::MutableBin()
{
    return GetPlayer()->SaveData().mutable_battlepasscomp();
}

const ServerProto::BattlePassCompBin& BattlePassMgr::Bin() const
{
    return GetPlayer()->SaveData().battlepasscomp();
}

void BattlePassMgr::OnCreate()
{
    InitializeDefault();
}

void BattlePassMgr::OnLoad()
{
    if (!GetPlayer())
    {
        return;
    }

    const bool hadBattlePassComp = GetPlayer()->SaveData().has_battlepasscomp();
    auto* bin = MutableBin();
    const uint32_t activeBattlePassId = GetActiveBattlePassId();
    if (!hadBattlePassComp || bin->battlepassid() == 0 || bin->battlepassid() != activeBattlePassId)
    {
        InitializeDefault();
        return;
    }

    if (bin->basicreward().empty())
    {
        bin->set_basicreward(Bitset().ToByteArray());
    }
    if (bin->premiumreward().empty())
    {
        bin->set_premiumreward(Bitset().ToByteArray());
    }

    for (auto& quest : *bin->mutable_quests())
    {
        if (quest.progress_size() == 0)
        {
            quest.add_progress();
        }

        const auto params = GetBattlePassQuestParams(static_cast<int>(quest.id()));
        quest.set_condition(static_cast<uint32_t>(params.CompleteCond));
        quest.set_param(params.CompleteCondParams.size() >= 2 ? static_cast<uint32_t>(std::max(params.CompleteCondParams[1], 0)) : 0);
        quest.mutable_progress(0)->set_max(params.CompleteCondParams.empty() ? quest.progress(0).max() : static_cast<uint32_t>(std::max(params.CompleteCondParams[0], 0)));
    }

    for (uint32_t id : SortedResourceIds(GameData::BattlePassQuestDataTable))
    {
        const auto it = GameData::BattlePassQuestDataTable.find(std::to_string(id));
        if (it == GameData::BattlePassQuestDataTable.end())
        {
            continue;
        }

        UpsertQuest(it->second);
    }
}

void BattlePassMgr::OnLogin()
{
}

void BattlePassMgr::EncodePlayerInfo(proto::PlayerInfo& out) const
{
    auto* state = out.mutable_state()->mutable_battlepass();
    state->set_state(GetClientState());
}

void BattlePassMgr::InitializeDefault()
{
    auto* bin = MutableBin();
    bin->Clear();
    bin->set_battlepassid(GetActiveBattlePassId());
    bin->set_mode(0);
    bin->set_level(0);
    bin->set_exp(0);
    bin->set_expweek(0);
    bin->set_basicreward(Bitset().ToByteArray());
    bin->set_premiumreward(Bitset().ToByteArray());

    for (uint32_t id : SortedResourceIds(GameData::BattlePassQuestDataTable))
    {
        const auto it = GameData::BattlePassQuestDataTable.find(std::to_string(id));
        if (it == GameData::BattlePassQuestDataTable.end())
        {
            continue;
        }

        auto* quest = bin->add_quests();
        quest->set_id(id);
        quest->set_type(static_cast<uint32_t>(GetQuestType(it->second)));
        quest->set_status(0);
        quest->set_expire(0);
        const auto params = GetBattlePassQuestParams(static_cast<int>(id));
        quest->set_condition(static_cast<uint32_t>(params.CompleteCond));
        quest->set_param(params.CompleteCondParams.size() >= 2 ? static_cast<uint32_t>(std::max(params.CompleteCondParams[1], 0)) : 0);
        auto* progress = quest->add_progress();
        progress->set_cur(0);
        progress->set_max(params.CompleteCondParams.empty() ? 0 : static_cast<uint32_t>(std::max(params.CompleteCondParams[0], 0)));
    }
}

uint32_t BattlePassMgr::GetActiveBattlePassId() const
{
    const int64_t now = GameTime::ServerNowSeconds();
    for (const auto& [key, data] : GameData::BattlePassDataTable)
    {
        const int64_t start = data.StartTime > 0 ? data.StartTime : 0;
        const int64_t end = data.EndTime > 0 ? data.EndTime : kNoDeadline;
        if (now >= start && now <= end)
        {
            return static_cast<uint32_t>(std::max(data.Id, 0));
        }
    }

    return GameConstants::BattlePassId;
}

const BattlePassRes* BattlePassMgr::GetCurrentSeason() const
{
    auto it = GameData::BattlePassDataTable.find(std::to_string(Bin().battlepassid()));
    if (it != GameData::BattlePassDataTable.end())
    {
        return &it->second;
    }

    it = GameData::BattlePassDataTable.find(std::to_string(GameConstants::BattlePassId));
    return it != GameData::BattlePassDataTable.end() ? &it->second : nullptr;
}

const BattlePassRewardRes* BattlePassMgr::GetRewardData(uint32_t level) const
{
    const std::string rewardKey = std::to_string(Bin().battlepassid()) + "|" + std::to_string(level);
    auto it = GameData::BattlePassRewardDataTable.find(rewardKey);
    return it != GameData::BattlePassRewardDataTable.end() ? &it->second : nullptr;
}

int64_t BattlePassMgr::GetDeadline() const
{
    return kNoDeadline;
}

bool BattlePassMgr::IsUnlocked() const
{
    return GetPlayer() && GetPlayer()->IsBattlePassUnlocked();
}

ServerProto::QuestInfoBin* BattlePassMgr::FindQuest(proto::QuestType type, uint32_t id)
{
    auto* quests = MutableBin()->mutable_quests();
    for (auto& quest : *quests)
    {
        if (quest.id() == id && quest.type() == static_cast<uint32_t>(type))
        {
            return &quest;
        }
    }

    return nullptr;
}

const ServerProto::QuestInfoBin* BattlePassMgr::FindQuest(proto::QuestType type, uint32_t id) const
{
    for (const auto& quest : Bin().quests())
    {
        if (quest.id() == id && quest.type() == static_cast<uint32_t>(type))
        {
            return &quest;
        }
    }

    return nullptr;
}

ServerProto::QuestInfoBin* BattlePassMgr::UpsertQuest(const BattlePassQuestRes& data)
{
    const auto type = GetQuestType(data);
    auto* quest = FindQuest(type, static_cast<uint32_t>(std::max(data.Id, 0)));
    if (!quest)
    {
        quest = MutableBin()->add_quests();
        quest->set_id(static_cast<uint32_t>(std::max(data.Id, 0)));
        quest->set_type(static_cast<uint32_t>(type));
        quest->set_expire(0);
        quest->add_progress();
    }

    const auto params = GetBattlePassQuestParams(data.Id);
    quest->set_condition(static_cast<uint32_t>(std::max(params.CompleteCond, 0)));
    quest->set_param(params.CompleteCondParams.size() >= 2 ? static_cast<uint32_t>(std::max(params.CompleteCondParams[1], 0)) : 0);

    if (quest->progress_size() == 0)
    {
        quest->add_progress();
    }

    const uint32_t maxProgress = params.CompleteCondParams.empty()
        ? 0
        : static_cast<uint32_t>(std::max(params.CompleteCondParams[0], 0));
    quest->mutable_progress(0)->set_max(maxProgress);
    return quest;
}

void BattlePassMgr::SyncQuest(const ServerProto::QuestInfoBin& quest) const
{
    if (!GetPlayer() || !GetPlayer()->GetSessionRef() || !IsUnlocked())
    {
        return;
    }

    GetPlayer()->PushNextPackage(quest_change_notify, BuildQuestProto(quest));
}

void BattlePassMgr::ResetDailyQuests(bool resetWeekly)
{
    auto* bin = MutableBin();

    for (uint32_t id : SortedResourceIds(GameData::BattlePassQuestDataTable))
    {
        const auto it = GameData::BattlePassQuestDataTable.find(std::to_string(id));
        if (it == GameData::BattlePassQuestDataTable.end())
        {
            continue;
        }

        const auto type = GetQuestType(it->second);
        if (type == proto::BattlePassWeekly && !resetWeekly)
        {
            continue;
        }

        auto* quest = UpsertQuest(it->second);
        quest->set_status(0);
        quest->set_expire(0);
        quest->mutable_progress(0)->set_cur(0);
        SyncQuest(*quest);
    }

    if (resetWeekly)
    {
        bin->set_expweek(0);
    }
}

void BattlePassMgr::Trigger(uint32_t condition, uint32_t progress, uint32_t param1, uint32_t param2)
{
    if (progress == 0)
    {
        return;
    }

    for (auto& quest : *MutableBin()->mutable_quests())
    {
        if (quest.condition() != condition)
        {
            continue;
        }

        if (quest.param() != 0 && quest.param() != param1)
        {
            continue;
        }

        if (quest.status() == 2)
        {
            continue;
        }

        if (quest.progress_size() == 0)
        {
            quest.add_progress();
        }

        auto* savedProgress = quest.mutable_progress(0);
        const uint32_t max = savedProgress->max();
        const uint32_t oldCur = savedProgress->cur();
        const uint32_t nextCur = max > 0
            ? std::min<uint32_t>(oldCur + progress, max)
            : oldCur + progress;

        if (nextCur == oldCur)
        {
            continue;
        }

        savedProgress->set_cur(nextCur);
        if (max > 0 && nextCur >= max)
        {
            quest.set_status(1);
        }

        SyncQuest(quest);
    }
}

uint32_t BattlePassMgr::GetMaxExpForNextLevel() const
{
    const uint32_t nextLevel = Bin().level() + 1;
    const auto it = GameData::BattlePassLevelDataTable.find(std::to_string(nextLevel));
    return it == GameData::BattlePassLevelDataTable.end()
        ? 0
        : static_cast<uint32_t>(std::max(it->second.Exp, 0));
}

void BattlePassMgr::AddExp(uint32_t amount)
{
    if (amount == 0)
    {
        return;
    }

    auto* bin = MutableBin();
    bin->set_exp(bin->exp() + amount);

    uint32_t expRequired = GetMaxExpForNextLevel();
    while (expRequired > 0 && bin->exp() >= expRequired)
    {
        bin->set_level(bin->level() + 1);
        bin->set_exp(bin->exp() - expRequired);
        expRequired = GetMaxExpForNextLevel();
    }
}

bool BattlePassMgr::ClaimQuestReward(uint32_t questId, uint32_t& level, uint32_t& exp, uint32_t& expThisWeek)
{
    if (!IsUnlocked())
    {
        return false;
    }

    std::vector<ServerProto::QuestInfoBin*> claimList;
    for (auto& quest : *MutableBin()->mutable_quests())
    {
        if (questId != 0 && quest.id() != questId)
        {
            continue;
        }

        if (questId == 0 && !IsQuestComplete(quest))
        {
            continue;
        }

        if (quest.status() == 2)
        {
            continue;
        }

        claimList.push_back(&quest);
    }

    if (claimList.empty())
    {
        return false;
    }

    uint32_t addExp = 0;
    uint32_t addExpWeek = 0;
    for (auto* quest : claimList)
    {
        if (!quest)
        {
            continue;
        }

        const auto it = GameData::BattlePassQuestDataTable.find(std::to_string(quest->id()));
        if (it == GameData::BattlePassQuestDataTable.end())
        {
            continue;
        }

        quest->set_status(2);
        addExp += static_cast<uint32_t>(std::max(it->second.Exp, 0));
        if (quest->type() == static_cast<uint32_t>(proto::BattlePassWeekly))
        {
            addExpWeek += static_cast<uint32_t>(std::max(it->second.Exp, 0));
        }
    }

    AddExp(addExp);
    auto* bin = MutableBin();
    bin->set_expweek(bin->expweek() + addExpWeek);

    level = bin->level();
    exp = bin->exp();
    expThisWeek = bin->expweek();
    PushStateNotify();
    return true;
}

bool BattlePassMgr::AddRewardItems(uint32_t level, bool premium, proto::ChangeInfo& change)
{
    const auto* data = GetRewardData(level);
    if (!data)
    {
        return true;
    }

    if (premium)
    {
        AddReward(change, GetPlayer(), static_cast<uint32_t>(std::max(data->Tid2, 0)), data->Qty2);
        if (Bin().mode() >= 2 && data->Tid3 > 0 && data->Qty3 > 0)
        {
            AddReward(change, GetPlayer(), static_cast<uint32_t>(std::max(data->Tid3, 0)), data->Qty3);
        }
    }
    else
    {
        AddReward(change, GetPlayer(), static_cast<uint32_t>(std::max(data->Tid1, 0)), data->Qty1);
    }

    return true;
}

bool BattlePassMgr::ReceiveSingleReward(bool premium, uint32_t level, proto::ChangeInfo& change)
{
    if (level == 0)
    {
        return false;
    }

    auto rewards = premium ? Bitset(Bin().premiumreward()) : Bitset(Bin().basicreward());
    if (rewards.IsSet(level))
    {
        return false;
    }

    rewards.SetBit(level);
    if (premium)
    {
        MutableBin()->set_premiumreward(rewards.ToByteArray());
    }
    else
    {
        MutableBin()->set_basicreward(rewards.ToByteArray());
    }

    return AddRewardItems(level, premium, change);
}

bool BattlePassMgr::ReceiveAllRewards(proto::ChangeInfo& change)
{
    bool hasRewards = false;
    auto basicRewards = Bitset(Bin().basicreward());
    auto premiumRewards = Bitset(Bin().premiumreward());
    const bool luxury = Bin().mode() >= 2;

    for (uint32_t level = 1; level <= Bin().level(); ++level)
    {
        const bool claimBasic = !basicRewards.IsSet(level);
        const bool claimPremium = Bin().mode() > 0 && !premiumRewards.IsSet(level);

        if (!claimBasic && !claimPremium)
        {
            continue;
        }

        const auto* data = GetRewardData(level);
        if (!data)
        {
            continue;
        }

        if (claimBasic)
        {
            basicRewards.SetBit(level);
            AddReward(change, GetPlayer(), static_cast<uint32_t>(std::max(data->Tid1, 0)), data->Qty1);
            hasRewards = true;
        }

        if (claimPremium)
        {
            AddReward(change, GetPlayer(), static_cast<uint32_t>(std::max(data->Tid2, 0)), data->Qty2);
            if (luxury)
            {
                AddReward(change, GetPlayer(), static_cast<uint32_t>(std::max(data->Tid3, 0)), data->Qty3);
            }
            premiumRewards.SetBit(level);
            hasRewards = true;
        }
    }

    if (!hasRewards)
    {
        return false;
    }

    auto* bin = MutableBin();
    bin->set_basicreward(basicRewards.ToByteArray());
    bin->set_premiumreward(premiumRewards.ToByteArray());
    return true;
}

bool BattlePassMgr::ClaimReward(const proto::BattlePassRewardReceiveReq& req, proto::BattlePassRewardReceiveResp& rsp)
{
    if (!IsUnlocked())
    {
        return false;
    }

    proto::ChangeInfo change;
    bool ok = false;

    if (req.premium() > 0)
    {
        ok = ReceiveSingleReward(true, req.premium(), change);
    }
    else if (req.basic() > 0)
    {
        ok = ReceiveSingleReward(false, req.basic(), change);
    }
    else if (req.has_all() || req.Mode_case() == proto::BattlePassRewardReceiveReq::kAll)
    {
        ok = ReceiveAllRewards(change);
    }

    if (!ok)
    {
        return false;
    }

    rsp.set_basicreward(Bin().basicreward());
    rsp.set_premiumreward(Bin().premiumreward());
    rsp.mutable_change()->CopyFrom(change);
    PushStateNotify();
    return true;
}

bool BattlePassMgr::BuyLevels(uint32_t levels, proto::BattlePassLevelBuyResp& rsp)
{
    if (!IsUnlocked())
    {
        return false;
    }

    if (levels == 0)
    {
        levels = 1;
    }

    uint32_t totalCost = 0;
    uint32_t totalExp = 0;
    uint32_t affordableLevels = 0;
    const uint32_t currentLevel = Bin().level();

    for (uint32_t i = 1; i <= levels; ++i)
    {
        const uint32_t targetLevel = currentLevel + i;
        const auto it = GameData::BattlePassLevelDataTable.find(std::to_string(targetLevel));
        if (it == GameData::BattlePassLevelDataTable.end())
        {
            break;
        }

        const uint32_t cost = static_cast<uint32_t>(std::max(it->second.Qty, 0));
        if (GetPlayer()->Inventory().GetItemCount(GameConstants::GemItemId) < static_cast<int64_t>(totalCost + cost))
        {
            break;
        }

        totalCost += cost;
        totalExp += static_cast<uint32_t>(std::max(it->second.Exp, 0));
        ++affordableLevels;
    }

    if (affordableLevels == 0)
    {
        return false;
    }

    proto::ChangeInfo change;
    if (totalCost > 0 && !GetPlayer()->Inventory().ConsumeItem(GameConstants::GemItemId, totalCost, &change))
    {
        return false;
    }

    AddExp(totalExp);
    rsp.mutable_change()->CopyFrom(change);
    rsp.set_level(Bin().level());
    PushStateNotify();
    return true;
}

bool BattlePassMgr::CreateOrder(uint32_t mode, proto::OrderInfo& rsp)
{
    if (!IsUnlocked() || mode < 1 || mode > 2 || mode <= Bin().mode())
    {
        return false;
    }

    const auto* season = GetCurrentSeason();
    if (!season)
    {
        return false;
    }

    const std::string orderId = "battlepass." + std::to_string(GetPlayer()->GetUid()) + "." + std::to_string(GameTime::NowMilliseconds());
    std::string webToken;
    if (!GenerateToken(webToken, false))
    {
        return false;
    }

    PaymentContextService::WebOrderContext context;
    context.Token = webToken;
    context.Source = "battlepass";
    context.ProductKey = std::to_string(mode);
    context.GameOrderId = orderId;
    context.PlayerUid = GetPlayer()->GetUid();
    context.CreatedAt = GameTime::ServerNowSeconds();
    PaymentContextService::Instance().RegisterWebOrderContext(context);
    PaymentContextService::BattlePassCollectContext collectContext;
    collectContext.PlayerUid = GetPlayer()->GetUid();
    collectContext.RequestedMode = mode;
    collectContext.BattlePassId = Bin().battlepassid();
    PaymentContextService::Instance().RegisterBattlePassCollectContext(collectContext);

    rsp.set_id(orderId);
    rsp.set_extradata(webToken);
    const auto& cfg = Config::Get().httpServerConfig;
    const std::string host = cfg.publicIp.empty() ? "127.0.0.1" : cfg.publicIp;
    rsp.set_notifyurl("http://" + host + ":" + std::to_string(cfg.port) + "/order/notify");

    proto::OrderStateChange paidNotify;
    paidNotify.set_orderid(orderId);
    paidNotify.set_store(3);
    rsp.set_nextpackage(GameSession::EncodeMessage(order_paid_notify, paidNotify.SerializeAsString()));
    return true;
}

bool BattlePassMgr::CollectOrder(proto::BattlePassOrderCollectResp& rsp)
{
    if (!IsUnlocked())
    {
        return false;
    }

    auto* bin = MutableBin();
    PaymentContextService::BattlePassCollectContext context;
    if (!PaymentContextService::Instance().ConsumeBattlePassCollectContext(GetPlayer()->GetUid(), context))
    {
        if (bin->mode() == 0)
        {
            return false;
        }

        auto* collect = rsp.mutable_collectresp();
        collect->set_status(proto::CollectResp_StatusEnum_Done);
        rsp.set_mode(bin->mode());
        rsp.set_level(bin->level());
        rsp.set_version(bin->battlepassid());
        return true;
    }

    const uint32_t requestedMode = context.RequestedMode;
    const uint32_t oldMode = bin->mode();
    const auto* season = GetCurrentSeason();
    proto::ChangeInfo stateChange;
    proto::ChangeInfo displayChange;

    if (requestedMode > oldMode)
    {
        bin->set_mode(requestedMode);
        if (requestedMode == 2 && season && season->LuxuryBonusLevel > 0)
        {
            bin->set_level(bin->level() + static_cast<uint32_t>(std::max(season->LuxuryBonusLevel, 0)));
            bin->set_exp(0);
        }

        if (requestedMode == 2 && season)
        {
            if (oldMode == 0)
            {
                AddRewardWithDisplay(GetPlayer(), static_cast<uint32_t>(std::max(season->LuxuryTid, 0)), season->LuxuryQty, stateChange, displayChange);
            }
            else if (oldMode == 1)
            {
                AddRewardWithDisplay(GetPlayer(), static_cast<uint32_t>(std::max(season->ComplementaryTid, 0)), season->ComplementaryQty, stateChange, displayChange);
            }
        }
    }

    auto* collect = rsp.mutable_collectresp();
    collect->set_status(proto::CollectResp_StatusEnum_Done);
    if (displayChange.props_size() > 0)
    {
        collect->mutable_items()->CopyFrom(displayChange);
    }
    rsp.set_mode(bin->mode());
    rsp.set_level(bin->level());
    rsp.set_version(bin->battlepassid());

    if (stateChange.props_size() > 0)
    {
        GetPlayer()->Inventory().PushItemsChange(stateChange);
    }
    return true;
}

bool BattlePassMgr::HasClaimableQuest() const
{
    if (!IsUnlocked() || GetMaxExpForNextLevel() == 0)
    {
        return false;
    }

    for (const auto& quest : Bin().quests())
    {
        if (IsQuestComplete(quest))
        {
            return true;
        }
    }

    return false;
}

bool BattlePassMgr::HasClaimableReward() const
{
    if (!IsUnlocked())
    {
        return false;
    }

    const auto basicRewards = Bitset(Bin().basicreward());
    const auto premiumRewards = Bitset(Bin().premiumreward());
    for (uint32_t level = 1; level <= Bin().level(); ++level)
    {
        if (!basicRewards.IsSet(level))
        {
            return true;
        }

        if (Bin().mode() > 0 && !premiumRewards.IsSet(level))
        {
            return true;
        }
    }

    return false;
}

int32_t BattlePassMgr::GetClientState() const
{
    int32_t state = 0;
    if (HasClaimableQuest())
    {
        state |= 1;
    }
    if (HasClaimableReward())
    {
        state |= 2;
    }
    return state;
}

proto::BattlePassInfo BattlePassMgr::ToProto() const
{
    proto::BattlePassInfo info;
    if (!IsUnlocked())
    {
        info.set_id(0);
        info.set_mode(0);
        info.set_deadline(0);
        info.set_level(0);
        info.set_exp(0);
        info.set_expthisweek(0);
        info.set_basicreward(Bitset().ToByteArray());
        info.set_premiumreward(Bitset().ToByteArray());
        return info;
    }

    info.set_id(Bin().battlepassid());
    info.set_mode(Bin().mode());
    info.set_deadline(kNoDeadline);
    info.set_level(Bin().level());
    info.set_exp(Bin().exp());
    info.set_expthisweek(Bin().expweek());
    info.set_basicreward(Bin().basicreward());
    info.set_premiumreward(Bin().premiumreward());

    auto* daily = info.mutable_dailyquests();
    auto* weekly = info.mutable_weeklyquests();
    for (const auto& quest : Bin().quests())
    {
        auto out = BuildQuestProto(quest);
        if (quest.type() == static_cast<uint32_t>(proto::BattlePassDaily))
        {
            daily->add_list()->CopyFrom(out);
        }
        else if (quest.type() == static_cast<uint32_t>(proto::BattlePassWeekly))
        {
            weekly->add_list()->CopyFrom(out);
        }
    }

    return info;
}

void BattlePassMgr::PushStateNotify() const
{
    if (GetPlayer())
    {
        GetPlayer()->QueueBattlePassStateNotify();
    }
}
