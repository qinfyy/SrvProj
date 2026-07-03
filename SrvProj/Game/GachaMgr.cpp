#include "GachaMgr.h"

#include "ChangeInfoUtil.h"
#include "CharacterMgr.h"
#include "InventoryMgr.h"
#include "Player.h"
#include "../GameConstants.h"
#include "../GameTime.h"
#include "../Resources/BinClass/CharacterRes.h"
#include "../Resources/BinClass/DiscRes.h"
#include "../Resources/BinClass/GachaRes.h"
#include "../Resources/BinClass/ItemsRes.h"
#include "../Resources/GameData.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <random>
#include <unordered_map>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace {
constexpr int64_t kHistoryRetentionSeconds = 60LL * 60LL * 24LL * 30LL * 6LL;
constexpr int kHistoryMaxSize = 2000;
constexpr int kItemTypeResource = 1;
constexpr int kItemTypeNormal = 2;
constexpr int kItemTypeCharacter = 3;
constexpr int kItemTypeDisc = 7;
constexpr int kNewbieTenPullCount = 10;
constexpr double kNewbieFiveRate = 0.75;
constexpr double kNewbieMultiFourRate = 10.0 / 15.0;
constexpr double kNewbieThreeFourRate = 0.35;

int32_t ClampChangeQty(int64_t qty)
{
    if (qty > std::numeric_limits<int32_t>::max())
    {
        return std::numeric_limits<int32_t>::max();
    }

    if (qty < std::numeric_limits<int32_t>::min())
    {
        return std::numeric_limits<int32_t>::min();
    }

    return static_cast<int32_t>(qty);
}

void AddItemTpl(proto::ItemTpl* tpl, uint32_t tid, int64_t qty)
{
    if (!tpl)
    {
        return;
    }

    tpl->set_tid(tid);
    tpl->set_qty(ClampChangeQty(qty));
}

void AddMapItem(std::map<uint32_t, int64_t>& items, uint32_t tid, int64_t qty)
{
    if (tid == 0 || qty == 0)
    {
        return;
    }

    items[tid] += qty;
}

int GetItemType(uint32_t itemId)
{
    const auto it = GameData::ItemDataTable.find(itemId);
    if (it == GameData::ItemDataTable.end())
    {
        return 0;
    }

    return it->second.Type;
}

bool IsResolvableItem(uint32_t itemId)
{
    const auto itemIt = GameData::ItemDataTable.find(itemId);
    if (itemIt == GameData::ItemDataTable.end())
    {
        return false;
    }

    if (itemIt->second.Type == kItemTypeCharacter)
    {
        return GameData::CharacterDataTable.find(itemId) != GameData::CharacterDataTable.end();
    }

    if (itemIt->second.Type == kItemTypeDisc)
    {
        return GameData::DiscDataTable.find(itemId) != GameData::DiscDataTable.end();
    }

    return true;
}

double RandomDouble()
{
    static thread_local std::mt19937 rng{ std::random_device{}() };
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}

int RandomInt(int minValue, int maxValue)
{
    static thread_local std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<int> dist(minValue, maxValue);
    return dist(rng);
}
}

void GachaMgr::OnCreate()
{
    MutableBin()->Clear();
}

void GachaMgr::OnLoad()
{
    TrimHistories(GameTime::ServerNowSeconds());
}

void GachaMgr::BeforeSave()
{
    TrimHistories(GameTime::ServerNowSeconds());
}

ServerProto::GachaCompBin* GachaMgr::MutableBin()
{
    return GetPlayer()->SaveData().mutable_gachacomp();
}

const ServerProto::GachaCompBin& GachaMgr::Bin() const
{
    return GetPlayer()->SaveData().gachacomp();
}

ServerProto::GachaBannerInfoBin* GachaMgr::UpsertBanner(uint32_t bannerId)
{
    auto* banners = MutableBin()->mutable_banners();
    auto& banner = (*banners)[bannerId];
    if (banner.id() == 0)
    {
        banner.set_id(bannerId);
    }

    return &banner;
}

const ServerProto::GachaBannerInfoBin* GachaMgr::FindBanner(uint32_t bannerId) const
{
    const auto& banners = Bin().banners();
    const auto it = banners.find(bannerId);
    return it == banners.end() ? nullptr : &it->second;
}

ServerProto::GachaPityStateBin* GachaMgr::UpsertPity(uint32_t storageId)
{
    auto* pityStates = MutableBin()->mutable_pitystates();
    return &(*pityStates)[storageId];
}

const ServerProto::GachaPityStateBin* GachaMgr::FindPity(uint32_t storageId) const
{
    const auto& pityStates = Bin().pitystates();
    const auto it = pityStates.find(storageId);
    return it == pityStates.end() ? nullptr : &it->second;
}

ServerProto::NewbieGachaStateBin* GachaMgr::GetOrCreateNewbieState(uint32_t newbieId, uint32_t spinCount, uint32_t saveCount)
{
    auto* states = MutableBin()->mutable_newbiestates();
    auto& state = (*states)[newbieId];
    if (state.id() == 0)
    {
        state.set_id(newbieId);
        state.set_remainingspincount(spinCount);
        state.set_savecount((std::max)(saveCount, 1u));
        state.set_selectedresult(-1);
    }
    else
    {
        state.set_savecount((std::max)(saveCount, 1u));
    }

    return &state;
}

GachaMgr::BannerDraft GachaMgr::CopyBanner(uint32_t bannerId) const
{
    BannerDraft draft;
    const auto* banner = FindBanner(bannerId);
    if (!banner)
    {
        return draft;
    }

    draft.Total = banner->total();
    draft.UsedFirstTen = banner->usedfirstten();
    draft.UsedGuarantee = banner->usedguarantee();
    return draft;
}

GachaMgr::PityDraft GachaMgr::CopyPity(uint32_t storageId) const
{
    PityDraft draft;
    const auto* pity = FindPity(storageId);
    if (!pity)
    {
        return draft;
    }

    draft.MissTimesA = pity->misstimesa();
    draft.MissTimesUpA = pity->misstimesupa();
    draft.MissTimesB = pity->misstimesb();
    draft.BGuaranteeDebt = pity->bguaranteedebt();
    return draft;
}

void GachaMgr::SaveBanner(uint32_t bannerId, const BannerDraft& draft)
{
    auto* banner = UpsertBanner(bannerId);
    banner->set_total(draft.Total);
    banner->set_usedfirstten(draft.UsedFirstTen);
    banner->set_usedguarantee(draft.UsedGuarantee);
}

void GachaMgr::SavePity(uint32_t storageId, const PityDraft& draft)
{
    auto* pity = UpsertPity(storageId);
    pity->set_misstimesa(draft.MissTimesA);
    pity->set_misstimesupa(draft.MissTimesUpA);
    pity->set_misstimesb(draft.MissTimesB);
    pity->set_bguaranteedebt(draft.BGuaranteeDebt);
}

bool GachaMgr::BuildCostPlan(const GachaRes& data, const GachaStorageRes& storage, uint32_t amount, CostPlan& out) const
{
    out = {};

    if (amount == 0)
    {
        return false;
    }

    const auto& inventory = GetPlayer()->Inventory();
    if (data.SpecificTid > 0 && data.SpecificQty > 0)
    {
        const int64_t specificReq = static_cast<int64_t>(data.SpecificQty) * amount;
        const int64_t specificQty = inventory.GetItemCount(static_cast<uint32_t>(data.SpecificTid));
        out.SpecificItemId = static_cast<uint32_t>(data.SpecificTid);
        out.ConsumeSpecificQty = (std::min)(specificReq, specificQty);
    }

    const int64_t coveredPullCount = data.SpecificQty > 0 ? out.ConsumeSpecificQty / data.SpecificQty : 0;
    const int64_t remainingPullCount = (std::max)(static_cast<int64_t>(amount) - coveredPullCount, 0LL);
    const int64_t remainingDefaultCostReq = static_cast<int64_t>(storage.DefaultQty) * remainingPullCount;
    const int64_t defaultQty = inventory.GetItemCount(static_cast<uint32_t>(storage.DefaultId));

    if (remainingDefaultCostReq > defaultQty)
    {
        out.ConvertReq = static_cast<int64_t>(storage.CostQty) * (remainingDefaultCostReq - defaultQty);
        if (out.ConvertReq > inventory.GetItemCount(static_cast<uint32_t>(storage.CostId)))
        {
            return false;
        }
    }

    out.ConsumeDefaultQty = (std::min)(remainingDefaultCostReq, defaultQty);
    return true;
}

bool GachaMgr::ApplyCostPlan(const GachaStorageRes& storage, const CostPlan& plan, proto::ChangeInfo& change)
{
    auto& inventory = GetPlayer()->Inventory();
    if (plan.ConsumeSpecificQty > 0 && !inventory.ConsumeItem(plan.SpecificItemId, plan.ConsumeSpecificQty, &change))
    {
        return false;
    }
    if (plan.ConvertReq > 0 && !inventory.ConsumeItem(static_cast<uint32_t>(storage.CostId), plan.ConvertReq, &change))
    {
        return false;
    }
    if (plan.ConsumeDefaultQty > 0 && !inventory.ConsumeItem(static_cast<uint32_t>(storage.DefaultId), plan.ConsumeDefaultQty, &change))
    {
        return false;
    }

    return true;
}

bool GachaMgr::PullOnce(const GachaRes& data, PityDraft& pity, uint32_t& itemId) const
{
    const auto storageIt = GameData::GachaStorageDataTable.find(data.StorageId);
    if (storageIt == GameData::GachaStorageDataTable.end())
    {
        return false;
    }

    const auto& storage = storageIt->second;
    int rollBase = (std::max)(GameConstants::GachaProbabilityBase, GachaATypeProbRes::GetMaxProb());
    int chanceA = GachaATypeProbRes::GetProb(storage.ATypeGroup, static_cast<int>(pity.MissTimesA), GameConstants::GachaDefaultATypeProb);
    int chanceB = storage.BTypeProb;
    int aupGuaranteeTimes = storage.AUpGuaranteeTimes;
    int bGuaranteeTimes = GameConstants::GachaDefaultBGuaranteeTimes;

    if (aupGuaranteeTimes > 0 && pity.MissTimesA >= static_cast<uint32_t>(aupGuaranteeTimes - 1))
    {
        chanceA = rollBase;
    }
    if (chanceA >= rollBase)
    {
        chanceA = rollBase;
        chanceB = 0;
    }

    const bool forceAUp = aupGuaranteeTimes > 0 && pity.MissTimesUpA >= static_cast<uint32_t>(aupGuaranteeTimes - 1);
    bool bGuaranteeTriggered = false;
    if (bGuaranteeTimes > 0 && pity.MissTimesB >= static_cast<uint32_t>(bGuaranteeTimes - 1))
    {
        chanceB = rollBase;
        bGuaranteeTriggered = true;
    }

    const int random = RandomInt(1, rollBase);
    const GachaRes::GachaPackage* package = nullptr;
    GachaRes::GachaPackage directAUp;

    if (forceAUp)
    {
        if (data.ATypeUpPkg > 0)
        {
            directAUp = { GachaRes::GachaPackageType::AUp, data.ATypeUpPkg };
            package = &directAUp;
        }
        else
        {
            package = data.PackageA.Next();
        }
    }
    else if (random <= chanceA)
    {
        package = data.PackageA.Next();
    }
    else if (random <= chanceB)
    {
        package = data.PackageB.Next();
    }
    else
    {
        package = data.PackageC.Next();
    }

    if (!package)
    {
        return false;
    }

    PityDraft next = pity;
    next.MissTimesA++;
    next.MissTimesB++;

    if (forceAUp || random <= chanceA)
    {
        next.MissTimesA = 0;
        if (bGuaranteeTriggered)
        {
            next.BGuaranteeDebt = true;
        }
    }
    else if (random <= chanceB)
    {
        next.MissTimesB = 0;
        next.BGuaranteeDebt = false;
    }

    GachaRes::GachaPackage selectedPackage = *package;
    if (next.BGuaranteeDebt && selectedPackage.Type == GachaRes::GachaPackageType::C)
    {
        const auto* compensated = data.PackageB.Next();
        if (compensated)
        {
            selectedPackage = *compensated;
            next.MissTimesB = 0;
            next.BGuaranteeDebt = false;
        }
    }

    if (selectedPackage.Type == GachaRes::GachaPackageType::AUp)
    {
        next.MissTimesUpA = 0;
    }
    else
    {
        next.MissTimesUpA++;
    }

    const auto* goodsPackage = GachaPkgRes::GetPackageById(selectedPackage.Id);
    if (!goodsPackage)
    {
        return false;
    }

    const int* goodsId = goodsPackage->Next();
    if (!goodsId || *goodsId <= 0)
    {
        return false;
    }

    itemId = static_cast<uint32_t>(*goodsId);
    pity = next;
    return true;
}

bool GachaMgr::BuildRewardPlan(const std::vector<uint32_t>& cards, const std::map<uint32_t, int64_t>& bonusItems, RewardPlan& out) const
{
    out = {};
    for (const auto& [itemId, qty] : bonusItems)
    {
        AddMapItem(out.GrantItems, itemId, qty);
    }

    for (uint32_t itemId : cards)
    {
        if (!IsResolvableItem(itemId))
        {
            return false;
        }

        auto& entry = out.AcquireItems[itemId];
        if (entry.Count == 0)
        {
            entry.Type = GetItemType(itemId);
            if (entry.Type == kItemTypeCharacter)
            {
                entry.Begin = GetPlayer()->Characters().HasCharacter(static_cast<int>(itemId)) ? 1 : 0;
            }
            else if (entry.Type == kItemTypeDisc)
            {
                const auto* disc = GetPlayer()->Characters().GetDiscById(static_cast<int>(itemId));
                if (disc)
                {
                    entry.Begin = 1 + static_cast<uint32_t>((std::max)(disc->star(), 0));
                    const auto discIt = GameData::DiscDataTable.find(itemId);
                    if (discIt != GameData::DiscDataTable.end())
                    {
                        entry.Begin += static_cast<uint32_t>(std::max<int64_t>(GetPlayer()->Inventory().GetItemCount(static_cast<uint32_t>(discIt->second.TransformItemId)), 0));
                    }
                }
            }
        }
        entry.Count++;
    }

    for (const auto& [itemId, entry] : out.AcquireItems)
    {
        if (entry.Type == kItemTypeCharacter)
        {
            const uint32_t newCharacterCount = entry.Begin == 0 ? 1 : 0;
            const uint32_t duplicateCount = entry.Count > newCharacterCount ? entry.Count - newCharacterCount : 0;

            if (duplicateCount > 0)
            {
                const auto charIt = GameData::CharacterDataTable.find(itemId);
                if (charIt == GameData::CharacterDataTable.end())
                {
                    return false;
                }

                AddMapItem(out.TransformSrc, itemId, duplicateCount);
                AddMapItem(out.TransformDst, static_cast<uint32_t>(charIt->second.FragmentsId), static_cast<int64_t>(charIt->second.TransformQty) * duplicateCount);
                AddMapItem(out.TransformDst, GameConstants::GachaExpertPermitItemId, static_cast<int64_t>(GameConstants::GachaExpertPermitPerDuplicateCharacter) * duplicateCount);
            }

            if (newCharacterCount > 0)
            {
                AddMapItem(out.GrantItems, itemId, newCharacterCount);
            }
            out.CharacterCount += entry.Count;
        }
        else if (entry.Type == kItemTypeDisc)
        {
            const uint32_t newDiscCount = entry.Begin == 0 ? 1 : 0;
            const uint32_t duplicateCount = entry.Count > newDiscCount ? entry.Count - newDiscCount : 0;
            const uint32_t effectiveBegin = entry.Begin + newDiscCount;
            const uint32_t maxTransformCount = effectiveBegin < 6 ? 6 - effectiveBegin : 0;
            const uint32_t transformCount = (std::min)(duplicateCount, maxTransformCount);
            const uint32_t extraCount = duplicateCount > transformCount ? duplicateCount - transformCount : 0;

            const auto discIt = GameData::DiscDataTable.find(itemId);
            if (discIt == GameData::DiscDataTable.end())
            {
                return false;
            }

            if (transformCount > 0)
            {
                AddMapItem(out.TransformSrc, itemId, transformCount);
                AddMapItem(out.TransformDst, static_cast<uint32_t>(discIt->second.TransformItemId), transformCount);
            }
            if (extraCount > 0)
            {
                AddMapItem(out.TransformSrc, itemId, extraCount);
                AddMapItem(out.TransformDst, GameConstants::GachaTravelPermitItemId, static_cast<int64_t>(GameConstants::GachaTravelPermitPerDisc) * extraCount);
            }
            if (newDiscCount > 0)
            {
                AddMapItem(out.GrantItems, itemId, newDiscCount);
            }
            AddMapItem(out.GrantItems, GameConstants::GachaTravelPermitItemId, static_cast<int64_t>(GameConstants::GachaTravelPermitPerDisc) * entry.Count);
        }
        else
        {
            AddMapItem(out.GrantItems, itemId, entry.Count);
        }
    }

    for (const auto& [itemId, qty] : out.TransformDst)
    {
        AddMapItem(out.GrantItems, itemId, qty);
    }

    return true;
}

bool GachaMgr::ApplyGrantItem(uint32_t itemId, int64_t qty, proto::ChangeInfo& change)
{
    if (itemId == 0 || qty <= 0)
    {
        return true;
    }

    const int type = GetItemType(itemId);
    if (type == kItemTypeCharacter)
    {
        for (int64_t i = 0; i < qty; ++i)
        {
            auto* character = GetPlayer()->Characters().AddCharacterFromId(static_cast<int>(itemId));
            if (!character)
            {
                return false;
            }
            GetPlayer()->Characters().AddCharacterChange(change, *character);
        }
        return true;
    }

    if (type == kItemTypeDisc)
    {
        for (int64_t i = 0; i < qty; ++i)
        {
            auto* disc = GetPlayer()->Characters().AddDiscFromId(static_cast<int>(itemId));
            if (!disc)
            {
                return false;
            }
            GetPlayer()->Characters().AddDiscChange(change, *disc);
        }
        return true;
    }

    return GetPlayer()->Inventory().AddItem(itemId, qty, &change);
}

bool GachaMgr::ApplyRewardPlan(const RewardPlan& plan, proto::ChangeInfo& change)
{
    for (const auto& [itemId, qty] : plan.GrantItems)
    {
        if (!ApplyGrantItem(itemId, qty, change))
        {
            return false;
        }
    }

    proto::Acquire acquire;
    for (const auto& [itemId, entry] : plan.AcquireItems)
    {
        auto* info = acquire.add_list();
        info->set_tid(itemId);
        info->set_begin(entry.Begin);
        info->set_count(entry.Count);
    }
    ChangeInfoUtil::AddProp(change, acquire);

    proto::Transform transform;
    for (const auto& [itemId, qty] : plan.TransformSrc)
    {
        AddItemTpl(transform.add_src(), itemId, qty);
    }
    for (const auto& [itemId, qty] : plan.TransformDst)
    {
        AddItemTpl(transform.add_dst(), itemId, qty);
    }
    ChangeInfoUtil::AddProp(change, transform);

    return true;
}

bool GachaMgr::Spin(uint32_t bannerId, uint32_t amount, proto::GachaSpinResp& out)
{
    const auto dataIt = GameData::GachaDataTable.find(bannerId);
    if (dataIt == GameData::GachaDataTable.end())
    {
        return false;
    }

    const auto& data = dataIt->second;
    const int64_t now = GameTime::ServerNowSeconds();
    if (!data.IsActiveAt(now))
    {
        return false;
    }

    const auto storageIt = GameData::GachaStorageDataTable.find(data.StorageId);
    if (storageIt == GameData::GachaStorageDataTable.end())
    {
        return false;
    }
    const auto& storage = storageIt->second;

    CostPlan costPlan;
    if (!BuildCostPlan(data, storage, amount, costPlan))
    {
        return false;
    }

    BannerDraft banner = CopyBanner(bannerId);
    PityDraft pity = CopyPity(static_cast<uint32_t>(data.StorageId));

    std::map<uint32_t, int64_t> bonusItems;
    ItemParamMap giveItems = ItemParamMap::FromJsonString(storage.GiveItems);
    int firstTenMultiplier = 1;
    if (amount == 10 && !banner.UsedFirstTen)
    {
        firstTenMultiplier = (std::max)(data.FirstTenShow, 1);
        banner.UsedFirstTen = true;
    }
    for (const auto& [itemId, qty] : giveItems.Items)
    {
        AddMapItem(bonusItems, static_cast<uint32_t>(itemId), static_cast<int64_t>(qty) * firstTenMultiplier * amount);
    }

    std::vector<uint32_t> cards;
    cards.reserve(amount);
    for (uint32_t i = 0; i < amount; ++i)
    {
        uint32_t itemId = 0;
        if (!PullOnce(data, pity, itemId))
        {
            return false;
        }
        cards.push_back(itemId);
    }

    RewardPlan rewardPlan;
    if (!BuildRewardPlan(cards, bonusItems, rewardPlan))
    {
        return false;
    }

    proto::ChangeInfo change;
    if (!ApplyCostPlan(storage, costPlan, change))
    {
        return false;
    }
    if (!ApplyRewardPlan(rewardPlan, change))
    {
        return false;
    }

    banner.Total += amount;
    SaveBanner(bannerId, banner);
    SavePity(static_cast<uint32_t>(data.StorageId), pity);
    AppendHistory(static_cast<uint32_t>(data.StorageId), bannerId, cards, now);

    out.set_time(now);
    out.set_amisstimes(pity.MissTimesA);
    out.set_aupmisstimes(pity.MissTimesUpA);
    out.set_gachatotaltimes(banner.Total);
    out.set_totaltimes(banner.Total);
    out.set_aupguaranteetimes(static_cast<uint32_t>((std::max)(data.GetDisplayAUpGuaranteeTimes(), 0)));
    out.mutable_change()->CopyFrom(change);
    for (uint32_t cardId : cards)
    {
        auto* card = out.add_cards();
        AddItemTpl(card->mutable_card(), cardId, 1);
    }

    GetPlayer()->Trigger(44, amount, 0, 0);
    if (rewardPlan.CharacterCount > 0)
    {
        GetPlayer()->Trigger(42, rewardPlan.CharacterCount, 0, 0);
    }

    return true;
}

proto::GachaInfo GachaMgr::BuildInfoProto(const GachaRes& data, const ServerProto::GachaBannerInfoBin& banner) const
{
    proto::GachaInfo info;
    info.set_id(static_cast<uint32_t>(data.Id));
    info.set_gachatotaltimes(banner.total());
    info.set_totaltimes(banner.total());
    info.set_recvguaranteereward(banner.usedguarantee());
    info.set_aupguaranteetimes(static_cast<uint32_t>((std::max)(data.GetDisplayAUpGuaranteeTimes(), 0)));

    const auto* pity = FindPity(static_cast<uint32_t>(data.StorageId));
    if (pity)
    {
        info.set_amisstimes(pity->misstimesa());
        info.set_aupmisstimes(pity->misstimesupa());
    }

    bool showFirstTen = false;
    const auto storageIt = GameData::GachaStorageDataTable.find(data.StorageId);
    if (storageIt != GameData::GachaStorageDataTable.end())
    {
        showFirstTen = !ItemParamMap::FromJsonString(storageIt->second.GiveItems).Empty() && !banner.usedfirstten();
    }
    info.set_revefirsttenreward(!showFirstTen);
    return info;
}

proto::GachaInformationResp GachaMgr::BuildInformation()
{
    proto::GachaInformationResp out;
    const int64_t now = GameTime::ServerNowSeconds();
    for (const auto& [_, data] : GameData::GachaDataTable)
    {
        if (!data.IsActiveAt(now))
        {
            continue;
        }

        const auto* banner = UpsertBanner(static_cast<uint32_t>(data.Id));
        out.add_information()->CopyFrom(BuildInfoProto(data, *banner));
    }

    return out;
}

bool GachaMgr::BuildHistories(uint32_t storageId, proto::GachaHistories& out) const
{
    if (GameData::GachaStorageDataTable.find(storageId) == GameData::GachaStorageDataTable.end())
    {
        return false;
    }

    const int64_t retentionStart = GameTime::ServerNowSeconds() - kHistoryRetentionSeconds;
    std::vector<const ServerProto::GachaHistoryBin*> histories;
    for (const auto& history : Bin().histories())
    {
        if (history.storageid() == storageId && history.time() >= retentionStart)
        {
            histories.push_back(&history);
        }
    }

    std::sort(histories.begin(), histories.end(), [](const auto* left, const auto* right) {
        return left->time() > right->time();
    });

    if (histories.size() > kHistoryMaxSize)
    {
        histories.resize(kHistoryMaxSize);
    }

    for (const auto* history : histories)
    {
        auto* item = out.add_list();
        item->set_gid(history->gachaid());
        item->set_time(history->time());
        for (uint32_t id : history->ids())
        {
            item->add_ids(id);
        }
    }

    return true;
}

bool GachaMgr::ReceiveGuarantee(uint32_t bannerId, proto::ChangeInfo& out)
{
    const auto* banner = FindBanner(bannerId);
    if (!banner)
    {
        return false;
    }

    const auto dataIt = GameData::GachaDataTable.find(bannerId);
    if (dataIt == GameData::GachaDataTable.end())
    {
        return false;
    }

    const auto& data = dataIt->second;
    if (!data.IsActiveAt(GameTime::ServerNowSeconds()) || !data.CanGuarantee() || banner->total() < static_cast<uint32_t>(data.GuaranteeTimes) || banner->usedguarantee())
    {
        return false;
    }

    if (!GetPlayer()->Inventory().AddItem(static_cast<uint32_t>(data.GuaranteeTid), data.GuaranteeQty, &out))
    {
        return false;
    }

    BannerDraft draft = CopyBanner(bannerId);
    draft.UsedGuarantee = true;
    SaveBanner(bannerId, draft);
    return true;
}

void GachaMgr::AppendHistory(uint32_t storageId, uint32_t gachaId, const std::vector<uint32_t>& cards, int64_t now)
{
    auto* history = MutableBin()->add_histories();
    history->set_storageid(storageId);
    history->set_gachaid(gachaId);
    history->set_time(now);
    for (uint32_t card : cards)
    {
        history->add_ids(card);
    }

    TrimHistories(now);
}

void GachaMgr::TrimHistories(int64_t now)
{
    const int64_t retentionStart = now - kHistoryRetentionSeconds;
    std::unordered_map<uint32_t, int> counts;
    std::vector<int> keepIndices;
    const auto& histories = Bin().histories();
    keepIndices.reserve(static_cast<size_t>(histories.size()));

    for (int i = histories.size() - 1; i >= 0; --i)
    {
        const auto& history = histories.Get(i);
        if (history.time() < retentionStart)
        {
            continue;
        }

        int& count = counts[history.storageid()];
        if (count >= kHistoryMaxSize)
        {
            continue;
        }

        count++;
        keepIndices.push_back(i);
    }

    std::reverse(keepIndices.begin(), keepIndices.end());
    std::vector<ServerProto::GachaHistoryBin> kept;
    kept.reserve(keepIndices.size());
    for (int index : keepIndices)
    {
        kept.push_back(histories.Get(index));
    }

    auto* mutableHistories = MutableBin()->mutable_histories();
    mutableHistories->Clear();
    for (const auto& history : kept)
    {
        mutableHistories->Add()->CopyFrom(history);
    }
}

bool GachaMgr::RollNewbieTen(const GachaRes& data, std::vector<uint32_t>& cards) const
{
    cards.clear();
    cards.reserve(kNewbieTenPullCount);

    const bool hasFive = RandomDouble() < kNewbieFiveRate;
    int fiveCount = hasFive ? 1 : 0;
    int fourCount = 1;
    if (hasFive)
    {
        fourCount = RandomDouble() < kNewbieMultiFourRate ? 2 : 1;
    }
    else if (RandomDouble() < kNewbieMultiFourRate)
    {
        fourCount = RandomDouble() < kNewbieThreeFourRate ? 3 : 2;
    }
    const int threeCount = kNewbieTenPullCount - fiveCount - fourCount;

    auto appendFromTier = [&cards](const WeightedList<GachaRes::GachaPackage>& tier, int count) {
        for (int i = 0; i < count; ++i)
        {
            const auto* package = tier.Next();
            if (!package)
            {
                return false;
            }

            const auto* goodsPackage = GachaPkgRes::GetPackageById(package->Id);
            if (!goodsPackage)
            {
                return false;
            }

            const int* itemId = goodsPackage->Next();
            if (!itemId || *itemId <= 0)
            {
                return false;
            }

            cards.push_back(static_cast<uint32_t>(*itemId));
        }
        return true;
    };

    if (!appendFromTier(data.PackageA, fiveCount) ||
        !appendFromTier(data.PackageB, fourCount) ||
        !appendFromTier(data.PackageC, threeCount) ||
        cards.size() != kNewbieTenPullCount)
    {
        return false;
    }

    static thread_local std::mt19937 rng{ std::random_device{}() };
    std::shuffle(cards.begin(), cards.end(), rng);
    for (uint32_t card : cards)
    {
        if (!IsResolvableItem(card))
        {
            return false;
        }
    }

    return true;
}

proto::GachaNewbieInfoResp GachaMgr::BuildNewbieInfo()
{
    proto::GachaNewbieInfoResp out;
    for (const auto& [_, data] : GameData::GachaNewbieDataTable)
    {
        auto* state = GetOrCreateNewbieState(static_cast<uint32_t>(data.Id), static_cast<uint32_t>((std::max)(data.SpinCount, 0)), static_cast<uint32_t>((std::max)(data.SaveCount, 1)));
        auto* info = out.add_list();
        info->set_id(static_cast<uint32_t>(data.Id));
        info->set_receive(state->received());
        const uint32_t spinCount = static_cast<uint32_t>((std::max)(data.SpinCount, 0));
        info->set_times(spinCount > state->remainingspincount() ? spinCount - state->remainingspincount() : 0);

        if (state->received())
        {
            continue;
        }

        for (uint32_t card : state->pendingresult())
        {
            info->mutable_temp()->add_values(card);
        }
        for (const auto& saved : state->savedresults())
        {
            auto* group = info->add_cards();
            for (uint32_t card : saved.cards())
            {
                group->add_values(card);
            }
        }
    }

    return out;
}

bool GachaMgr::SpinNewbie(uint32_t newbieId, proto::GachaNewbieSpinResp& out)
{
    const auto newbieIt = GameData::GachaNewbieDataTable.find(newbieId);
    if (newbieIt == GameData::GachaNewbieDataTable.end())
    {
        return false;
    }

    const auto bannerIt = GameData::GachaDataTable.find(newbieId);
    if (bannerIt == GameData::GachaDataTable.end() || !bannerIt->second.Valid)
    {
        return false;
    }

    auto* state = GetOrCreateNewbieState(newbieId, static_cast<uint32_t>((std::max)(newbieIt->second.SpinCount, 0)), static_cast<uint32_t>((std::max)(newbieIt->second.SaveCount, 1)));
    if (state->received() || state->remainingspincount() == 0)
    {
        return false;
    }

    std::vector<uint32_t> cards;
    if (!RollNewbieTen(bannerIt->second, cards))
    {
        return false;
    }

    state->clear_pendingresult();
    for (uint32_t card : cards)
    {
        state->add_pendingresult(card);
        out.add_cards(card);
    }
    state->set_remainingspincount(state->remainingspincount() - 1);
    return true;
}

bool GachaMgr::SaveNewbie(uint32_t newbieId, std::optional<uint32_t> index)
{
    const auto newbieIt = GameData::GachaNewbieDataTable.find(newbieId);
    if (newbieIt == GameData::GachaNewbieDataTable.end())
    {
        return false;
    }

    auto* state = GetOrCreateNewbieState(newbieId, static_cast<uint32_t>((std::max)(newbieIt->second.SpinCount, 0)), static_cast<uint32_t>((std::max)(newbieIt->second.SaveCount, 1)));
    if (state->received() || state->pendingresult_size() <= 0)
    {
        return false;
    }

    ServerProto::GachaCardGroupBin group;
    for (uint32_t card : state->pendingresult())
    {
        group.add_cards(card);
    }

    if (index.has_value())
    {
        if (*index >= static_cast<uint32_t>(state->savedresults_size()))
        {
            return false;
        }
        state->mutable_savedresults(static_cast<int>(*index))->CopyFrom(group);
    }
    else
    {
        if (state->savedresults_size() >= static_cast<int>(state->savecount()))
        {
            return false;
        }
        state->add_savedresults()->CopyFrom(group);
    }

    state->clear_pendingresult();
    return true;
}

bool GachaMgr::CopyNewbieCards(const ServerProto::NewbieGachaStateBin& state, uint32_t index, std::vector<uint32_t>& cards) const
{
    cards.clear();
    if (index == 0)
    {
        if (state.pendingresult_size() <= 0)
        {
            return false;
        }
        for (uint32_t card : state.pendingresult())
        {
            cards.push_back(card);
        }
        return true;
    }

    const uint32_t savedIndex = index - 1;
    if (savedIndex >= static_cast<uint32_t>(state.savedresults_size()))
    {
        return false;
    }

    const auto& saved = state.savedresults(static_cast<int>(savedIndex));
    for (uint32_t card : saved.cards())
    {
        cards.push_back(card);
    }

    return !cards.empty();
}

bool GachaMgr::ObtainNewbie(uint32_t newbieId, uint32_t index, proto::ChangeInfo& out)
{
    const auto newbieIt = GameData::GachaNewbieDataTable.find(newbieId);
    if (newbieIt == GameData::GachaNewbieDataTable.end())
    {
        return false;
    }

    auto* state = GetOrCreateNewbieState(newbieId, static_cast<uint32_t>((std::max)(newbieIt->second.SpinCount, 0)), static_cast<uint32_t>((std::max)(newbieIt->second.SaveCount, 1)));
    if (state->received())
    {
        return false;
    }

    std::vector<uint32_t> cards;
    if (!CopyNewbieCards(*state, index, cards))
    {
        return false;
    }

    RewardPlan rewardPlan;
    std::map<uint32_t, int64_t> noBonus;
    if (!BuildRewardPlan(cards, noBonus, rewardPlan))
    {
        return false;
    }

    if (!ApplyRewardPlan(rewardPlan, out))
    {
        return false;
    }

    state->set_selectedresult(static_cast<int32_t>(index));
    state->set_received(true);
    state->set_remainingspincount(0);
    state->clear_pendingresult();
    return true;
}
