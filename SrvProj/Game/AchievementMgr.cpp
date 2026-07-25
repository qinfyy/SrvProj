#include "AchievementMgr.h"

#include "Player.h"
#include "InventoryMgr.h"
#include "../GameTime.h"
#include "../Logger.h"
#include "../Resources/BinClass/AchievementsRes.h"
#include "../Resources/GameData.h"
#include "../proto/NetMsgId.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <string>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace {
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

bool IsIncrementalCondition(uint32_t condition)
{
    switch (condition)
    {
    case 3:   // BattleTotal
    case 7:   // CharacterAdvanceTotal
    case 8:   // CharacterSkillUpTotal
    case 9:   // CharacterSkillWithSpecificUpTotal
    case 12:  // CharacterUpTotal
    case 23:  // ChatTotal
    case 24:  // DailyInstanceClearSpecificDifficultyAndTotal
    case 25:  // DailyInstanceClearSpecificTypeAndTotal
    case 26:  // DailyInstanceClearTotal
    case 33:  // DiscLimitBreakTotal
    case 34:  // DiscPromoteTotal
    case 35:  // DiscStrengthenTotal
    case 40:  // GachaCharacterNotSSRTotal
    case 41:  // GachaCharacterTenModeSSRTotal
    case 42:  // GachaCharacterTotal
    case 44:  // GachaTotal
    case 45:  // GiftGiveTotal
    case 47:  // InfinityStarTowerClearTotal
    case 48:  // ItemsAdd
    case 49:  // ItemsDeplete
    case 50:  // ItemsProductTotal
    case 51:  // LoginTotal
    case 52:  // QuestTravelerDuelChallengeTotal
    case 57:  // RegionBossClearSpecificLevelWithDifficultyAndTotal
    case 58:  // RegionBossClearSpecificTotal
    case 59:  // RegionBossClearTotal
    case 72:  // RegionBossClearSpecificTypeWithTotal
    case 74:  // CharactersDatingTotal
    case 75:  // VampireSurvivorScoreTotal
    case 81:  // VampireClearTotal
    case 82:  // VampireWithSpecificClearTotal
    case 83:  // AgentFinishTotal
    case 84:  // AgentWithSpecificFinishTotal
    case 89:  // InfinityStarTowerClearSpecificDifficultyAndTotal
    case 90:  // SkillInstanceClearTotal
    case 92:  // WeekBoosClearSpecificDifficultyAndTotal
    case 95:  // JointDrillScoreTotal
    case 104: // CharGemInstanceClearTotal
    case 105: // DailyShopReceiveShopTotal
    case 106: // AgentApplyTotal
    case 121: // TutorialLevelSpecificClearTotal
    case 123: // WeekBossClearTotal
    case 501: // StarTowerBattleTimes
    case 502: // StarTowerBossChallengeSpecificHighRewardWithTotal
    case 504: // StarTowerBuildSpecificScoreWithTotal
    case 505: // StarTowerClearSpecificCharacterTypeWithTotal
    case 507: // StarTowerClearSpecificLevelWithDifficultyAndTotal
    case 508: // StarTowerClearTotal
    case 509: // StarTowerEnterRoom
    case 511: // StarTowerEventTimes
    case 512: // StarTowerFateTimes
    case 513: // StarTowerItemsGet
    case 514: // StarTowerSpecificDifficultyShopBuyTimes
    case 521: // StarTowerBuildSpecificDifficultyAndScoreWithTotal
    case 522: // StarTowerSpecificDifficultyStrengthenMachineTotal
    case 524: // StarTowerSpecificDifficultyKillBossTotal
    case 525: // StarTowerBookSpecificCharWithPotentialTotal
    case 526: // StarTowerBuildSpecificCharSpecificScoreWithTotal
    case 528: // StarTowerSpecificFateCardReRollTotal
    case 529: // StarTowerSpecificPotentialReRollTotal
    case 530: // StarTowerSpecificShopReRollTotal
    case 531: // StarTowerSpecificNoteActivateTotal
    case 532: // StarTowerSpecificNoteLevelTotal
    case 533: // StarTowerSpecificPotentialBonusTotal
    case 534: // StarTowerSpecificPotentialLuckyTotal
    case 535: // StarTowerSpecificShopBuyDiscountTotal
    case 536: // StarTowerSpecificSecondarySkillActivateTotal
    case 537: // StarTowerSpecificGetExtraNoteLvTotal
    case 539: // StarTowerSweepTimes
    case 540: // StarTowerSweepTotal
        return true;
    default:
        return false;
    }
}

uint32_t AchievementStatus(const ServerProto::AchievementInfoBin& achievement)
{
    if (achievement.claimed() || achievement.status() == 2)
    {
        return 2;
    }

    if (achievement.completed() > 0 || achievement.status() == 1)
    {
        return 1;
    }

    return 0;
}

uint32_t AchievementMax(uint32_t id)
{
    if (auto it = GameData::AchievementDataTable.find(id); it != GameData::AchievementDataTable.end())
    {
        return static_cast<uint32_t>(std::max(it->second.AimNumShow, 0));
    }

    LOG_WARNING("Achievement resource missing: id={}", id);
    return 0;
}

constexpr uint32_t kCondLoginTotal = 51;
constexpr uint32_t kCondCharacterTotal = 20;
}

ServerProto::AchievementCompBin* AchievementMgr::MutableBin()
{
    return GetPlayer()->SaveData().mutable_achievementcomp();
}

const ServerProto::AchievementCompBin& AchievementMgr::Bin() const
{
    return GetPlayer()->SaveData().achievementcomp();
}

ServerProto::AchievementInfoBin* AchievementMgr::FindAchievement(uint32_t id)
{
    auto* achievements = MutableBin()->mutable_achievements();
    for (auto& achievement : *achievements)
    {
        if (achievement.id() == id)
        {
            return &achievement;
        }
    }

    return nullptr;
}

uint32_t AchievementMgr::GetCompletedAchievementsCount() const
{
    uint32_t count = 0;
    for (const auto& achievement : Bin().achievements())
    {
        if (AchievementStatus(achievement) != 0)
        {
            ++count;
        }
    }

    return count;
}

ServerProto::AchievementInfoBin* AchievementMgr::UpsertAchievement(uint32_t id, uint32_t maxProgress)
{
    auto* achievement = FindAchievement(id);
    if (!achievement)
    {
        achievement = MutableBin()->add_achievements();
        achievement->set_id(id);
        achievement->add_progress();
    }

    if (achievement->progress_size() == 0)
    {
        achievement->add_progress();
    }

    auto* progress = achievement->mutable_progress(0);
    if (progress->max() == 0)
    {
        progress->set_max(maxProgress == 0 ? AchievementMax(id) : maxProgress);
    }

    return achievement;
}

proto::Achievement AchievementMgr::BuildAchievementProto(const ServerProto::AchievementInfoBin& achievement) const
{
    proto::Achievement out;
    out.set_id(achievement.id());
    out.set_status(AchievementStatus(achievement));
    out.set_completed(achievement.completed());

    if (achievement.progress_size() == 0)
    {
        auto* progress = out.add_progress();
        progress->set_max(AchievementMax(achievement.id()));
        return out;
    }

    for (const auto& savedProgress : achievement.progress())
    {
        auto* progress = out.add_progress();
        progress->set_cur(savedProgress.cur());
        progress->set_max(savedProgress.max());
    }

    return out;
}

proto::Achievements AchievementMgr::ToProto() const
{
    proto::Achievements out;
    for (const auto& achievement : Bin().achievements())
    {
        out.add_list()->CopyFrom(BuildAchievementProto(achievement));
    }

    return out;
}

void AchievementMgr::SyncAchievement(const ServerProto::AchievementInfoBin& achievement)
{
    if (!GetPlayer() || !GetPlayer()->GetSessionRef())
    {
        return;
    }

    GetPlayer()->PushNextPackage(achievement_change_notify, BuildAchievementProto(achievement));
}

void AchievementMgr::OnCreate()
{
    MutableBin()->clear_achievements();
}

void AchievementMgr::OnLoad()
{
    auto* achievements = MutableBin()->mutable_achievements();
    for (auto& achievement : *achievements)
    {
        if (achievement.progress_size() == 0)
        {
            achievement.add_progress();
        }

        auto* progress = achievement.mutable_progress(0);
        if (progress->max() == 0)
        {
            progress->set_max(AchievementMax(achievement.id()));
        }
    }
}

bool AchievementMgr::UpdateAchievement(
    const AchievementRes& data,
    uint32_t progressValue,
    uint32_t param1,
    uint32_t param2,
    bool incremental,
    bool& completed)
{
    completed = false;

    if (progressValue == 0 || !data.MatchParams(static_cast<int>(param1), static_cast<int>(param2)))
    {
        return false;
    }

    auto* achievement = UpsertAchievement(
        static_cast<uint32_t>(data.Id),
        static_cast<uint32_t>(std::max(data.AimNumShow, 0)));
    if (AchievementStatus(*achievement) != 0)
    {
        return false;
    }

    auto* progress = achievement->mutable_progress(0);
    const uint32_t oldCur = progress->cur();
    const uint32_t max = progress->max();
    const uint32_t nextCur = incremental
        ? (max > 0 ? std::min<uint32_t>(oldCur + progressValue, max) : oldCur + progressValue)
        : (max > 0 ? std::min<uint32_t>(progressValue, max) : progressValue);

    if (nextCur == oldCur)
    {
        return false;
    }

    progress->set_cur(nextCur);
    if (max > 0 && nextCur >= max)
    {
        achievement->set_completed(GameTime::NowSeconds());
        achievement->set_status(1);
        completed = true;
    }

    SyncAchievement(*achievement);
    return true;
}

void AchievementMgr::TriggerAchievementTotalIfNeeded(bool completed)
{
    if (!completed || mUpdatingAchievementTotal)
    {
        return;
    }

    mUpdatingAchievementTotal = true;
    GetPlayer()->Trigger(2, GetCompletedAchievementsCount(), 0, 0);
    mUpdatingAchievementTotal = false;
}

void AchievementMgr::BuildConditionIndex() const
{
    if (mConditionIndexBuilt)
    {
        return;
    }

    mConditionIndex.clear();
    for (const auto& [key, data] : GameData::AchievementDataTable)
    {
        if (data.CompleteCond <= 0)
        {
            continue;
        }

        mConditionIndex[static_cast<uint32_t>(data.CompleteCond)].push_back(&data);
    }

    for (auto& [condition, achievements] : mConditionIndex)
    {
        std::sort(achievements.begin(), achievements.end(), [](const AchievementRes* lhs, const AchievementRes* rhs) {
            return lhs->Id < rhs->Id;
        });
    }

    mConditionIndexBuilt = true;
}

const std::vector<const AchievementRes*>& AchievementMgr::GetAchievementsByCondition(uint32_t condition) const
{
    BuildConditionIndex();
    if (auto it = mConditionIndex.find(condition); it != mConditionIndex.end())
    {
        return it->second;
    }

    mEmptyConditionList.clear();
    return mEmptyConditionList;
}

std::vector<uint32_t> AchievementMgr::CollectInitialAchievements(
    uint32_t condition,
    uint32_t progressValue,
    uint32_t param1,
    uint32_t param2,
    size_t limit) const
{
    std::vector<uint32_t> ids;
    for (const auto* data : GetAchievementsByCondition(condition))
    {
        if (!data || !data->MatchParams(static_cast<int>(param1), static_cast<int>(param2)))
        {
            continue;
        }

        if (data->AimNumShow <= 0 || progressValue < static_cast<uint32_t>(data->AimNumShow))
        {
            ids.push_back(static_cast<uint32_t>(data->Id));
        }

        if (limit > 0 && ids.size() >= limit)
        {
            break;
        }
    }

    return ids;
}

void AchievementMgr::PushFirstLoginAchievementGroup(
    uint32_t condition,
    uint32_t progressValue,
    uint32_t param1,
    uint32_t param2,
    size_t limit)
{
    for (uint32_t id : CollectInitialAchievements(condition, progressValue, param1, param2, limit))
    {
        auto* achievement = UpsertAchievement(id, AchievementMax(id));
        achievement->set_status(0);
        achievement->set_completed(0);
        achievement->set_claimed(false);
        achievement->mutable_progress(0)->set_cur(progressValue);
        achievement->mutable_progress(0)->set_max(AchievementMax(id));
        SyncAchievement(*achievement);
    }
}

void AchievementMgr::Trigger(uint32_t condition, uint32_t progressValue, uint32_t param1, uint32_t param2)
{
    if (progressValue == 0 || condition == 200)
    {
        return;
    }

    const bool incremental = IsIncrementalCondition(condition);
    bool anyCompleted = false;

    for (const auto* data : GetAchievementsByCondition(condition))
    {
        if (!data)
        {
            continue;
        }

        bool completed = false;
        UpdateAchievement(*data, progressValue, param1, param2, incremental, completed);
        anyCompleted = anyCompleted || completed;
    }

    TriggerAchievementTotalIfNeeded(anyCompleted);
}

void AchievementMgr::TriggerOne(uint32_t id, uint32_t progressValue, uint32_t param1, uint32_t param2)
{
    if (progressValue == 0)
    {
        return;
    }

    auto it = GameData::AchievementDataTable.find(id);
    if (it == GameData::AchievementDataTable.end())
    {
        LOG_WARNING("Achievement resource missing: id={}", id);
        return;
    }

    bool completed = false;
    UpdateAchievement(
        it->second,
        progressValue,
        param1,
        param2,
        IsIncrementalCondition(static_cast<uint32_t>(std::max(it->second.CompleteCond, 0))),
        completed);
    TriggerAchievementTotalIfNeeded(completed);
}

void AchievementMgr::HandleClientEvents(const proto::Events& events)
{
    bool anyCompleted = false;

    for (const auto& event : events.list())
    {
        if (event.id() != 200 || event.data_size() < 2)
        {
            continue;
        }

        const uint32_t progressValue = event.data(0);
        const uint32_t achievementId = event.data(1);
        if (progressValue == 0)
        {
            continue;
        }

        auto it = GameData::AchievementDataTable.find(achievementId);
        if (it == GameData::AchievementDataTable.end() || it->second.CompleteCond != 200)
        {
            continue;
        }

        bool completed = false;
        UpdateAchievement(it->second, progressValue, 0, 0, true, completed);
        anyCompleted = anyCompleted || completed;
    }

    TriggerAchievementTotalIfNeeded(anyCompleted);
}

bool AchievementMgr::ClaimRewards(const google::protobuf::RepeatedField<uint32_t>& ids, proto::ChangeInfo& out)
{
    for (uint32_t id : ids)
    {
        auto* achievement = FindAchievement(id);
        if (!achievement || AchievementStatus(*achievement) != 1)
        {
            continue;
        }

        if (auto it = GameData::AchievementDataTable.find(id); it != GameData::AchievementDataTable.end())
        {
            GetPlayer()->Inventory().AddItem(static_cast<uint32_t>(it->second.Tid1), it->second.Qty1, &out);
        }

        achievement->set_claimed(true);
        achievement->set_status(2);
    }

    return ids.size() > 0;
}

void AchievementMgr::PushFirstLoginNotificationsBeforeSignin()
{
    PushFirstLoginAchievementGroup(kCondLoginTotal, 1, 0, 0, 5);
}

void AchievementMgr::PushFirstLoginNotificationsAfterSignin()
{
    PushFirstLoginAchievementGroup(kCondCharacterTotal, 3, 0, 0, 5);
}

bool AchievementMgr::HasNewAchievements() const
{
    for (const auto& achievement : Bin().achievements())
    {
        if (AchievementStatus(achievement) == 1)
        {
            return true;
        }
    }

    return false;
}
