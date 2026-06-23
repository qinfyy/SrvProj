#include "QuestMgr.h"

#include "AchievementMgr.h"
#include "Bitset.h"
#include "InventoryMgr.h"
#include "Player.h"
#include "../GameConstants.h"
#include "../Logger.h"
#include "../Resources/BinClass/MiscRes.h"
#include "../Resources/BinClass/QuestRes.h"
#include "../Resources/GameData.h"
#include "../proto/NetMsgId.pb.h"
#include "../proto/proto_cpp/notify.pb.h"
#include "../proto/proto_cpp/public.pb.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <initializer_list>
#include <string>
#include <vector>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace {
constexpr uint32_t kCondLoginTotal = 51;
constexpr uint32_t kCondQuestWithSpecificType = 55;

struct QuestSeed {
    uint32_t id;
    proto::QuestType type;
};

struct QuestUpdateSeed {
    uint32_t id;
    proto::QuestType type;
    uint32_t progress;
};

struct QuestParams {
    uint32_t condition = 0;
    uint32_t max = 0;
    uint32_t param = 0;
};

std::vector<uint32_t> ParseParams(const std::string& text)
{
    std::vector<uint32_t> out;
    uint32_t value = 0;
    bool inNumber = false;

    for (unsigned char ch : text)
    {
        if (std::isdigit(ch))
        {
            value = (value * 10) + static_cast<uint32_t>(ch - '0');
            inNumber = true;
            continue;
        }

        if (inNumber)
        {
            out.push_back(value);
            value = 0;
            inNumber = false;
        }
    }

    if (inNumber)
    {
        out.push_back(value);
    }

    return out;
}

bool Contains(const google::protobuf::RepeatedField<uint32_t>& values, uint32_t value)
{
    return std::find(values.begin(), values.end(), value) != values.end();
}

void AddItemChange(proto::ChangeInfo& change, uint32_t tid, int32_t qty)
{
    if (tid == 0 || qty == 0)
    {
        return;
    }

    proto::Item item;
    item.set_tid(tid);
    item.set_qty(qty);
    change.add_props()->PackFrom(item);
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

proto::Quest BuildQuestProto(const ServerProto::QuestInfoBin& bin)
{
    proto::Quest quest;
    quest.set_id(bin.id());
    quest.set_type(static_cast<proto::QuestType>(bin.type()));
    quest.set_expire(bin.expire());

    const uint32_t cur = bin.progress_size() > 0 ? bin.progress(0).cur() : 0;
    const uint32_t max = bin.progress_size() > 0 ? bin.progress(0).max() : 0;
    quest.set_status(QuestStatus(bin.status(), cur, max));

    if (bin.progress_size() == 0)
    {
        auto* progress = quest.add_progress();
        progress->set_cur(0);
        progress->set_max(0);
        return quest;
    }

    for (const auto& savedProgress : bin.progress())
    {
        auto* progress = quest.add_progress();
        progress->set_cur(savedProgress.cur());
        progress->set_max(savedProgress.max());
    }

    return quest;
}

proto::SigninRewardUpdate BuildSigninRewardUpdate()
{
    proto::SigninRewardUpdate update;
    update.set_index(1);
    update.set_switch_(true);

    const SignInRes* firstReward = nullptr;
    for (const auto& [key, reward] : GameData::SignInDataTable)
    {
        if (reward.Day != 1)
        {
            continue;
        }
        if (!firstReward || reward.Group > firstReward->Group)
        {
            firstReward = &reward;
        }
    }

    if (firstReward)
    {
        AddItemChange(*update.mutable_change(), static_cast<uint32_t>(firstReward->ItemId), firstReward->ItemQty);
    }
    else
    {
        LOG_WARNING("SignIn day 1 resource missing");
    }

    return update;
}

std::string BuildHandbookFlag(uint32_t type, std::initializer_list<uint32_t> handbookIds)
{
    Bitset bitset;
    for (uint32_t id : handbookIds)
    {
        const auto it = GameData::HandbookDataTable.find(std::to_string(id));
        if (it == GameData::HandbookDataTable.end())
        {
            LOG_WARNING("Handbook resource missing: id={}", id);
            continue;
        }
        if (static_cast<uint32_t>(std::max(it->second.Type, 0)) != type)
        {
            continue;
        }
        bitset.SetBit(static_cast<uint32_t>(std::max(it->second.Index, 0)));
    }

    return bitset.ToByteArray();
}

proto::HandbookInfo BuildHandbookInfo(uint32_t type, const std::string& data)
{
    proto::HandbookInfo info;
    info.set_type(type);
    info.set_data(data);
    return info;
}

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

QuestParams GetQuestParams(proto::QuestType type, uint32_t id, uint32_t fallbackMax)
{
    const auto key = std::to_string(id);

    if (type == proto::Daily)
    {
        if (auto it = GameData::DailyQuestDataTable.find(key); it != GameData::DailyQuestDataTable.end())
        {
            auto params = ParseParams(it->second.CompleteCondParams);
            return {
                static_cast<uint32_t>(std::max(it->second.CompleteCond, 0)),
                params.empty() ? fallbackMax : params[0],
                params.size() >= 2 ? params[1] : 0
            };
        }
    }
    else if (type == proto::Weekly)
    {
        if (auto it = GameData::WeeklyQuestDataTable.find(key); it != GameData::WeeklyQuestDataTable.end())
        {
            auto params = ParseParams(it->second.CompleteCondParams);
            return {
                static_cast<uint32_t>(std::max(it->second.CompleteCond, 0)),
                params.empty() ? fallbackMax : params[0],
                params.size() >= 2 ? params[1] : 0
            };
        }
    }
    return {0, fallbackMax, 0};
}

bool HasQuestResource(uint32_t id, proto::QuestType type)
{
    const auto key = std::to_string(id);

    switch (type)
    {
    case proto::Daily:
        return GameData::DailyQuestDataTable.find(key) != GameData::DailyQuestDataTable.end();
    case proto::Weekly:
        return GameData::WeeklyQuestDataTable.find(key) != GameData::WeeklyQuestDataTable.end();
    default:
        return true;
    }
}

void ValidateQuestSeed(const QuestSeed& seed)
{
    if (!HasQuestResource(seed.id, seed.type))
    {
        LOG_WARNING("Quest resource missing: id={}, type={}", seed.id, static_cast<int>(seed.type));
    }
}

std::vector<uint32_t> OrderedIds(const std::vector<uint32_t>& preferredOrder, std::vector<uint32_t> resourceIds)
{
    std::vector<uint32_t> out;
    out.reserve(resourceIds.size());

    for (uint32_t id : preferredOrder)
    {
        auto it = std::find(resourceIds.begin(), resourceIds.end(), id);
        if (it == resourceIds.end())
        {
            continue;
        }

        out.push_back(id);
        resourceIds.erase(it);
    }

    std::sort(resourceIds.begin(), resourceIds.end());
    out.insert(out.end(), resourceIds.begin(), resourceIds.end());
    return out;
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

std::vector<QuestSeed> BuildFirstLoginQuestSnapshotSeeds()
{
    std::vector<QuestSeed> seeds;

    const auto dailyOrder = OrderedIds(
        {1008, 1006, 1004, 1010, 2002, 1002, 2001, 2003, 2005, 2004, 1009, 1005, 1001, 1003, 1007},
        SortedResourceIds(GameData::DailyQuestDataTable));
    for (uint32_t id : dailyOrder)
    {
        seeds.push_back({id, proto::Daily});
    }

    const auto weeklyOrder = OrderedIds(
        {1008, 1006, 1004, 1010, 1012, 1002, 1009, 1011, 1005, 1001, 1003, 1013, 1007},
        SortedResourceIds(GameData::WeeklyQuestDataTable));
    for (uint32_t id : weeklyOrder)
    {
        seeds.push_back({id, proto::Weekly});
    }

    return seeds;
}

std::vector<QuestUpdateSeed> BuildFirstLoginQuestProgressSeeds()
{
    std::vector<QuestUpdateSeed> seeds;
    for (const auto& questSeed : BuildFirstLoginQuestSnapshotSeeds())
    {
        auto params = GetQuestParams(questSeed.type, questSeed.id, 0);
        if (params.condition == kCondLoginTotal && params.max > 0)
        {
            seeds.push_back({questSeed.id, questSeed.type, 1});
        }
    }

    return seeds;
}

}

ServerProto::QuestCompBin* QuestMgr::MutableBin()
{
    return GetPlayer()->SaveData().mutable_questcomp();
}

const ServerProto::QuestCompBin& QuestMgr::Bin() const
{
    return GetPlayer()->SaveData().questcomp();
}

ServerProto::QuestInfoBin* QuestMgr::FindQuest(proto::QuestType type, uint32_t id)
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

const ServerProto::QuestInfoBin* QuestMgr::FindQuest(proto::QuestType type, uint32_t id) const
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

ServerProto::QuestInfoBin* QuestMgr::UpsertQuest(proto::QuestType type, uint32_t id, uint32_t maxProgress)
{
    auto* quest = FindQuest(type, id);
    if (!quest)
    {
        quest = MutableBin()->add_quests();
        quest->set_id(id);
        quest->set_type(static_cast<uint32_t>(type));
        quest->set_expire(0);
        quest->add_progress();
    }

    auto params = GetQuestParams(type, id, maxProgress);
    quest->set_condition(params.condition);
    quest->set_param(params.param);

    if (quest->progress_size() == 0)
    {
        quest->add_progress();
    }
    quest->mutable_progress(0)->set_max(params.max == 0 ? maxProgress : params.max);

    return quest;
}

void QuestMgr::InitializeDefaultQuests(bool markFirstLoginDone)
{
    auto* bin = MutableBin();
    bin->clear_quests();
    bin->clear_dailyactiveids();
    bin->clear_weeklyactiveids();
    bin->set_dailyshoprewardclaimed(false);
    bin->set_dailymallrewardclaimed(false);

    for (uint32_t id : SortedResourceIds(GameData::DailyQuestDataTable))
    {
        auto* quest = UpsertQuest(proto::Daily, id, 0);
        quest->set_status(0);
        quest->set_expire(0);
        quest->mutable_progress(0)->set_cur(0);
    }

    for (uint32_t id : SortedResourceIds(GameData::WeeklyQuestDataTable))
    {
        auto* quest = UpsertQuest(proto::Weekly, id, 0);
        quest->set_status(0);
        quest->set_expire(0);
        quest->mutable_progress(0)->set_cur(0);
    }

    bin->set_firstloginnotifydone(markFirstLoginDone);
}

void QuestMgr::OnCreate()
{
    InitializeDefaultQuests(false);
}

void QuestMgr::OnLoad()
{
    if (!GetPlayer())
    {
        return;
    }

    const bool hadQuestComp = GetPlayer()->SaveData().has_questcomp();
    auto* bin = MutableBin();

    if (!hadQuestComp || bin->quests_size() == 0)
    {
        InitializeDefaultQuests(true);
        return;
    }

    for (auto& quest : *bin->mutable_quests())
    {
        if (quest.progress_size() == 0)
        {
            quest.add_progress();
        }

        auto params = GetQuestParams(static_cast<proto::QuestType>(quest.type()), quest.id(), quest.progress(0).max());
        quest.set_condition(params.condition);
        quest.set_param(params.param);
        quest.mutable_progress(0)->set_max(params.max);
    }

    bin->set_firstloginnotifydone(true);
}

void QuestMgr::SyncQuest(const ServerProto::QuestInfoBin& quest)
{
    if (!GetPlayer() || !GetPlayer()->GetSessionRef())
    {
        return;
    }

    GetPlayer()->PushNextPackage(quest_change_notify, BuildQuestProto(quest));
}

void QuestMgr::PushFirstLoginNotifications()
{
    auto* player = GetPlayer();
    if (!player || !player->GetSessionRef())
    {
        return;
    }

    for (const auto& seed : BuildFirstLoginQuestSnapshotSeeds())
    {
        ValidateQuestSeed(seed);
        auto* quest = UpsertQuest(seed.type, seed.id, 0);
        quest->set_status(0);
        quest->mutable_progress(0)->set_cur(0);
        player->PushNextPackage(quest_change_notify, BuildQuestProto(*quest));
    }

    for (const auto& seed : BuildFirstLoginQuestProgressSeeds())
    {
        auto* quest = UpsertQuest(seed.type, seed.id, 0);
        quest->set_status(seed.progress >= quest->mutable_progress(0)->max() ? 1 : 0);
        quest->mutable_progress(0)->set_cur(seed.progress);
        player->PushNextPackage(quest_change_notify, BuildQuestProto(*quest));
    }

    player->Achievements().PushFirstLoginNotificationsBeforeSignin();
    player->PushNextPackage(signin_reward_change_notify, BuildSigninRewardUpdate());
    player->Achievements().PushFirstLoginNotificationsAfterSignin();

    player->PushNextPackage(handbook_change_notify, BuildHandbookInfo(1, BuildHandbookFlag(1, {410301, 410302, 410601})));
    player->PushNextPackage(handbook_change_notify, BuildHandbookInfo(2, BuildHandbookFlag(2, {})));
}

void QuestMgr::OnLogin()
{
    auto* bin = MutableBin();
    if (!bin->firstloginnotifydone())
    {
        bin->set_firstloginnotifydone(true);
        PushFirstLoginNotifications();
    }
}

void QuestMgr::ResetDailyQuests(bool resetWeekly)
{
    auto* bin = MutableBin();

    for (uint32_t id : SortedResourceIds(GameData::DailyQuestDataTable))
    {
        auto* quest = UpsertQuest(proto::Daily, id, 0);
        quest->set_status(0);
        quest->set_expire(0);
        quest->mutable_progress(0)->set_cur(0);
        SyncQuest(*quest);
    }

    bin->clear_dailyactiveids();
    bin->set_dailyshoprewardclaimed(false);
    bin->set_dailymallrewardclaimed(false);

    if (!resetWeekly)
    {
        return;
    }

    for (uint32_t id : SortedResourceIds(GameData::WeeklyQuestDataTable))
    {
        auto* quest = UpsertQuest(proto::Weekly, id, 0);
        quest->set_status(0);
        quest->set_expire(0);
        quest->mutable_progress(0)->set_cur(0);
        SyncQuest(*quest);
    }

    bin->clear_weeklyactiveids();
}

void QuestMgr::Trigger(uint32_t condition, uint32_t progress, uint32_t param1, uint32_t param2)
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
        const uint32_t oldCur = savedProgress->cur();
        const uint32_t max = savedProgress->max();
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

bool QuestMgr::ClaimDailyQuestReward(uint32_t questId, proto::ChangeInfo& out)
{
    std::vector<ServerProto::QuestInfoBin*> claimList;
    for (auto& quest : *MutableBin()->mutable_quests())
    {
        if (quest.type() != static_cast<uint32_t>(proto::Daily))
        {
            continue;
        }

        if (questId > 0)
        {
            if (quest.id() == questId && quest.status() != 2)
            {
                claimList.push_back(&quest);
            }
            continue;
        }

        if (QuestStatus(quest.status(), quest.progress_size() > 0 ? quest.progress(0).cur() : 0, quest.progress_size() > 0 ? quest.progress(0).max() : 0) != 1)
        {
            continue;
        }

        claimList.push_back(&quest);
    }

    uint32_t claimedCount = 0;
    for (auto* quest : claimList)
    {
        if (!quest)
        {
            continue;
        }

        if (auto it = GameData::DailyQuestDataTable.find(std::to_string(quest->id())); it != GameData::DailyQuestDataTable.end())
        {
            GetPlayer()->Inventory().AddItem(static_cast<uint32_t>(it->second.ItemTid), it->second.ItemQty, &out);
        }

        quest->set_status(2);
        SyncQuest(*quest);
        ++claimedCount;
    }

    if (claimedCount > 0)
    {
        Trigger(kCondQuestWithSpecificType, claimedCount, proto::Daily, 0);
    }

    return claimedCount > 0;
}

bool QuestMgr::ClaimWeeklyQuestReward(uint32_t questId, proto::ChangeInfo& out)
{
    std::vector<ServerProto::QuestInfoBin*> claimList;
    for (auto& quest : *MutableBin()->mutable_quests())
    {
        if (quest.type() != static_cast<uint32_t>(proto::Weekly))
        {
            continue;
        }

        if (questId > 0)
        {
            if (quest.id() == questId && quest.status() != 2)
            {
                claimList.push_back(&quest);
            }
            continue;
        }

        if (QuestStatus(quest.status(), quest.progress_size() > 0 ? quest.progress(0).cur() : 0, quest.progress_size() > 0 ? quest.progress(0).max() : 0) != 1)
        {
            continue;
        }

        claimList.push_back(&quest);
    }

    uint32_t claimedCount = 0;
    for (auto* quest : claimList)
    {
        if (!quest)
        {
            continue;
        }

        if (auto it = GameData::WeeklyQuestDataTable.find(std::to_string(quest->id())); it != GameData::WeeklyQuestDataTable.end())
        {
            GetPlayer()->Inventory().AddItem(static_cast<uint32_t>(it->second.ItemTid), it->second.ItemQty, &out);
        }

        quest->set_status(2);
        SyncQuest(*quest);
        ++claimedCount;
    }

    if (claimedCount > 0)
    {
        Trigger(kCondQuestWithSpecificType, claimedCount, proto::Weekly, 0);
    }

    return claimedCount > 0;
}

bool QuestMgr::ClaimDailyActiveRewards(std::vector<uint32_t>& activeIds, proto::ChangeInfo& out)
{
    uint32_t activity = 0;
    for (const auto& quest : Bin().quests())
    {
        if (quest.type() != static_cast<uint32_t>(proto::Daily) || quest.status() != 2)
        {
            continue;
        }
        if (auto it = GameData::DailyQuestDataTable.find(std::to_string(quest.id())); it != GameData::DailyQuestDataTable.end())
        {
            activity += static_cast<uint32_t>(std::max(it->second.Active, 0));
        }
    }

    auto* bin = MutableBin();
    for (const auto& [key, reward] : GameData::DailyQuestActiveDataTable)
    {
        const uint32_t id = static_cast<uint32_t>(reward.Id);
        if (Contains(bin->dailyactiveids(), id) || activity < static_cast<uint32_t>(std::max(reward.Active, 0)))
        {
            continue;
        }

        bin->add_dailyactiveids(id);
        activeIds.push_back(id);
        GetPlayer()->Inventory().AddItem(static_cast<uint32_t>(reward.ItemTid1), reward.Number1, &out);
        GetPlayer()->Inventory().AddItem(static_cast<uint32_t>(reward.ItemTid2), reward.Number2, &out);
    }

    return !activeIds.empty();
}

bool QuestMgr::ClaimWeeklyActiveRewards(std::vector<uint32_t>& activeIds, proto::ChangeInfo& out)
{
    uint32_t activity = 0;
    for (const auto& quest : Bin().quests())
    {
        if (quest.type() != static_cast<uint32_t>(proto::Weekly) || quest.status() != 2)
        {
            continue;
        }
        if (auto it = GameData::WeeklyQuestDataTable.find(std::to_string(quest.id())); it != GameData::WeeklyQuestDataTable.end())
        {
            activity += static_cast<uint32_t>(std::max(it->second.Active, 0));
        }
    }

    auto* bin = MutableBin();
    for (const auto& [key, reward] : GameData::WeeklyQuestActiveDataTable)
    {
        const uint32_t id = static_cast<uint32_t>(reward.Id);
        if (Contains(bin->weeklyactiveids(), id) || activity < static_cast<uint32_t>(std::max(reward.Active, 0)))
        {
            continue;
        }

        bin->add_weeklyactiveids(id);
        activeIds.push_back(id);
        GetPlayer()->Inventory().AddItem(static_cast<uint32_t>(reward.ItemTid1), reward.Number1, &out);
        GetPlayer()->Inventory().AddItem(static_cast<uint32_t>(reward.ItemTid2), reward.Number2, &out);
    }

    return !activeIds.empty();
}

bool QuestMgr::ClaimDailyShopGift(proto::ChangeInfo& out)
{
    auto* bin = MutableBin();
    if (bin->dailyshoprewardclaimed())
    {
        return false;
    }

    bin->set_dailyshoprewardclaimed(true);
    GetPlayer()->Inventory().AddItem(GameConstants::GoldItemId, 10000, &out);
    Trigger(105, 1, 0, 0);
    return true;
}

bool QuestMgr::ClaimDailyMallGift(proto::ChangeInfo& out)
{
    auto* bin = MutableBin();
    if (bin->dailymallrewardclaimed())
    {
        return false;
    }

    bin->set_dailymallrewardclaimed(true);
    GetPlayer()->Inventory().AddItem(GameConstants::JointDrillTicketId, 1, &out);
    Trigger(105, 1, 0, 0);
    return true;
}

bool QuestMgr::HasDailyShopReward() const
{
    return !Bin().dailyshoprewardclaimed();
}

bool QuestMgr::HasDailyMallReward() const
{
    return !Bin().dailymallrewardclaimed();
}

void QuestMgr::EncodePlayerInfo(proto::PlayerInfo& out) const
{
    auto* quests = out.mutable_quests();

    for (const auto& quest : Bin().quests())
    {
        quests->add_list()->CopyFrom(BuildQuestProto(quest));
    }

    for (uint32_t id : Bin().dailyactiveids())
    {
        out.add_dailyactiveids(id);
    }

    for (uint32_t id : Bin().weeklyactiveids())
    {
        out.add_weeklyactiveids(id);
    }

    out.set_dailyshoprewardstatus(HasDailyShopReward());
    out.set_dailymallrewardstatus(HasDailyMallReward());
    out.set_tourguidequestgroup(9);
}
