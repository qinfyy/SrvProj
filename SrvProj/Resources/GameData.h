#pragma once

#include <unordered_map>
#include <memory>

// 这里使用前置声明，在GameData.cpp和其他使用的地方中包含具体的头文件，避免在这里引入过多的依赖，导致后面触发循环依赖。
class CharacterRes;
class CharacterAdvanceRes;
class CharacterSkillUpgradeRes;
class CharacterUpgradeRes;
class CharItemExpRes;
class CharacterSkinRes;
class TalentGroupRes;
class TalentRes;
class CharGemRes;
class CharGemSlotControlRes;
class CharGemAttrValueRes;
class AffinityLevelRes;
class AffinityGiftRes;
class PlotRes;
class DiscRes;
class ChatRes;
class DatingLandmarkRes;
class DatingLandmarkEventRes;
class DatingCharacterEventRes;
class DiscStrengthenRes;
class DiscItemExpRes;
class DiscPromoteRes;
class DiscPromoteLimitRes;
class SecondarySkillRes;
class ItemRes;
class ProductionRes;
class PlayerHeadRes;
class TitleRes;
class HonorRes;
class MallMonthlyCardRes;
class MallPackageRes;
class MallShopRes;
class MallGemRes;
class ResidentShopRes;
class ResidentGoodsRes;
class BattlePassRes;
class BattlePassLevelRes;
class BattlePassQuestRes;
class BattlePassRewardRes;
class AgentRes;
class DictionaryTabRes;
class DictionaryEntryRes;
class GachaATypeProbRes;
class GachaRes;
class GachaNewbieRes;
class GachaStorageRes;
class GachaTypeRes;
class StoryRes;
class StorySetSectionRes;
class StoryEvidenceRes;
class MainScreenCGRes;
class DailyQuestRes;
class DailyQuestActiveRes;
class WeeklyQuestRes;
class WeeklyQuestActiveRes;
class AchievementRes;
class TutorialLevelRes;
class DailyInstanceRes;
class DailyInstanceRewardGroupRes;
class RegionBossLevelRes;
class SkillInstanceRes;
class CharGemInstanceRes;
class WeekBossLevelRes;
class StarTowerRes;
class StarTowerStageRes;
class StarTowerGrowthNodeRes;
class StarTowerFloorExpRes;
class StarTowerTeamExpRes;
class StarTowerEventRes;
class StarTowerBuildRankRes;
class SubNoteSkillPromoteGroupRes;
class PotentialRes;
class CharPotentialRes;
class StarTowerBookFateCardBundleRes;
class StarTowerBookFateCardQuestRes;
class StarTowerBookFateCardRes;
class FateCardRes;
class InfinityTowerLevelRes;
class InfinityTowerDifficultyRes;
class VampireSurvivorRes;
class VampireTalentRes;
class ScoreBossControlRes;
class ScoreBossRewardRes;
class WorldClassRes;
class GuideGroupRes;
class HandbookRes;
class SignInRes;
class ActivityRes;
class LoginRewardGroupControlRes;
class TowerResenseLevelRes;
class TrialControlRes;
class TrialGroupRes;
class JointDrill2LevelRes;
class ActivityLevelsLevelRes;
class ActivityTaskRes;
class ActivityTaskGroupRes;
class ActivityShopRes;
class ActivityShopControlRes;
class ActivityGoodsRes;

class GameData {
public:
    // 禁止实例化
    GameData() = delete;
    ~GameData() = delete;
    GameData(const GameData&) = delete;
    GameData& operator=(const GameData&) = delete;

    // Characters
    static std::unordered_map<int, CharacterRes> CharacterDataTable;
    static std::unordered_map<int, CharacterAdvanceRes> CharacterAdvanceDataTable;
    static std::unordered_map<int, CharacterSkillUpgradeRes> CharacterSkillUpgradeDataTable;
    static std::unordered_map<int, CharacterUpgradeRes> CharacterUpgradeDataTable;
    static std::unordered_map<int, CharItemExpRes> CharItemExpDataTable;
    static std::unordered_map<int, CharacterSkinRes> CharacterSkinDataTable;
    static std::unordered_map<int, TalentGroupRes> TalentGroupDataTable;
    static std::unordered_map<int, TalentRes> TalentDataTable;

    // Characters: Emblems
    static std::unordered_map<int, CharGemRes> CharGemDataTable;
    static std::unordered_map<int, CharGemSlotControlRes> CharGemSlotControlDataTable;

    static std::unordered_map<int, CharGemAttrValueRes> CharGemAttrValueDataTable;

    // Characters: Affinity
    static std::unordered_map<int, AffinityLevelRes> AffinityLevelDataTable;
    static std::unordered_map<int, AffinityGiftRes> AffinityGiftDataTable;
    static std::unordered_map<int, PlotRes> PlotDataTable;

    // Characters: Phone
    static std::unordered_map<int, ChatRes> ChatDataTable;

    // Characters: Dating
    static std::unordered_map<int, DatingLandmarkRes> DatingLandmarkDataTable;
    static std::unordered_map<int, DatingLandmarkEventRes> DatingLandmarkEventDataTable;
    static std::unordered_map<int, DatingCharacterEventRes> DatingCharacterEventDataTable;

    //// ===== Discs =====
    static std::unordered_map<int, DiscRes> DiscDataTable;
    static std::unordered_map<int, DiscStrengthenRes> DiscStrengthenDataTable;
    static std::unordered_map<int, DiscItemExpRes> DiscItemExpDataTable;
    static std::unordered_map<int, DiscPromoteRes> DiscPromoteDataTable;
    static std::unordered_map<int, DiscPromoteLimitRes> DiscPromoteLimitDataTable;

    // Discs: Melody items
    static std::unordered_map<int, SecondarySkillRes> SecondarySkillDataTable;

    // ===== Items =====
    static std::unordered_map<int, ItemRes> ItemDataTable;
    static std::unordered_map<int, ProductionRes> ProductionDataTable;
    static std::unordered_map<int, PlayerHeadRes> PlayerHeadDataTable;
    static std::unordered_map<int, TitleRes> TitleDataTable;
    static std::unordered_map<int, HonorRes> HonorDataTable;

    // ===== Shops =====
    //static inline, std::unordered_map<int, MallMonthlyCardRes> mMallMonthlyCardDataTable;
    //static inline, std::unordered_map<int, MallPackageRes> mMallPackageDataTable;
    //static inline, std::unordered_map<int, MallShopRes> mMallShopDataTable;
    //static inline, std::unordered_map<int, MallGemRes> mMallGemDataTable;

    //static inline, std::unordered_map<int, ResidentShopRes> mResidentShopDataTable;
    //static inline, std::unordered_map<int, ResidentGoodsRes> mResidentGoodsDataTable;

    //// ===== Battle Pass =====
    //static inline, std::unordered_map<int, BattlePassRes> mBattlePassDataTable;
    //static inline, std::unordered_map<int, BattlePassLevelRes> mBattlePassLevelDataTable;
    //static inline, std::unordered_map<int, BattlePassQuestRes> mBattlePassQuestDataTable;
    //static inline, std::unordered_map<int, BattlePassRewardRes> mBattlePassRewardDataTable;

    //// ===== Commissions =====
    //static inline, std::unordered_map<int, AgentRes> mAgentDataTable;

    //// ===== Dictionary =====
    //static inline, std::unordered_map<int, DictionaryTabRes> mDictionaryTabDataTable;
    //static inline, std::unordered_map<int, DictionaryEntryRes> mDictionaryEntryDataTable;

    //// ===== Gacha =====
    //static inline, std::unordered_map<int, GachaATypeProbRes> mGachaATypeProbDataTable;
    //static inline, std::unordered_map<int, GachaRes> mGachaDataTable;
    //static inline, std::unordered_map<int, GachaNewbieRes> mGachaNewbieDataTable;
    //static inline, std::unordered_map<int, GachaStorageRes> mGachaStorageDataTable;
    //static inline, std::unordered_map<int, GachaTypeRes> mGachaTypeDataTable;

    // ===== Story =====
    static std::unordered_map<int, StoryRes> StoryDataTable;
    static std::unordered_map<int, StorySetSectionRes> StorySetSectionDataTable;
    static std::unordered_map<int, StoryEvidenceRes> StoryEvidenceDataTable;

    static std::unordered_map<int, MainScreenCGRes> MainScreenCGDataTable;

    // ===== Daily/Weekly Quests =====
    static std::unordered_map<int, DailyQuestRes> DailyQuestDataTable;
    static std::unordered_map<int, DailyQuestActiveRes> DailyQuestActiveDataTable;
    static std::unordered_map<int, WeeklyQuestRes> WeeklyQuestDataTable;
    static std::unordered_map<int, WeeklyQuestActiveRes> WeeklyQuestActiveDataTable;

    //// ===== Achievements =====
    //static inline, std::unordered_map<int, AchievementRes> mAchievementDataTable;

    //// ===== Tutorials =====
    //static inline, std::unordered_map<int, TutorialLevelRes> mTutorialLevelDataTable;

    //// ===== Instances =====
    //static inline, std::unordered_map<int, DailyInstanceRes> mDailyInstanceDataTable;
    //static inline, std::unordered_map<int, DailyInstanceRewardGroupRes> mDailyInstanceRewardGroupDataTable;
    //static inline, std::unordered_map<int, RegionBossLevelRes> mRegionBossLevelDataTable;
    //static inline, std::unordered_map<int, SkillInstanceRes> mSkillInstanceDataTable;
    //static inline, std::unordered_map<int, CharGemInstanceRes> mCharGemInstanceDataTable;
    //static inline, std::unordered_map<int, WeekBossLevelRes> mWeekBossLevelDataTable;

    //// ===== Star Tower =====
    //static inline, std::unordered_map<int, StarTowerRes> mStarTowerDataTable;
    //static inline, std::unordered_map<int, StarTowerStageRes> mStarTowerStageDataTable;
    //static inline, std::unordered_map<int, StarTowerGrowthNodeRes> mStarTowerGrowthNodeDataTable;
    //static inline, std::unordered_map<int, StarTowerFloorExpRes> mStarTowerFloorExpDataTable;
    //static inline, std::unordered_map<int, StarTowerTeamExpRes> mStarTowerTeamExpDataTable;
    //static inline, std::unordered_map<int, StarTowerEventRes> mStarTowerEventDataTable;
    //static inline, std::unordered_map<int, StarTowerBuildRankRes> mStarTowerBuildRankDataTable;
    //static inline, std::unordered_map<int, SubNoteSkillPromoteGroupRes> mSubNoteSkillPromoteGroupDataTable;

    //static inline, std::unordered_map<int, PotentialRes> mPotentialDataTable;
    //static inline, std::unordered_map<int, CharPotentialRes> mCharPotentialDataTable;

    //static inline, std::unordered_map<int, StarTowerBookFateCardBundleRes> mStarTowerBookFateCardBundleDataTable;
    //static inline, std::unordered_map<int, StarTowerBookFateCardQuestRes> mStarTowerBookFateCardQuestDataTable;
    //static inline, std::unordered_map<int, StarTowerBookFateCardRes> mStarTowerBookFateCardDataTable;
    //static inline, std::unordered_map<int, FateCardRes> mFateCardDataTable;

    //// ===== Infinity Tower =====
    //static inline, std::unordered_map<int, InfinityTowerLevelRes> mInfinityTowerLevelDataTable;
    //static inline, std::unordered_map<int, InfinityTowerDifficultyRes> mInfinityTowerDifficultyDataTable;

    //// ===== Vampire Survivor =====
    //static inline, std::unordered_map<int, VampireSurvivorRes> mVampireSurvivorDataTable;
    //static inline, std::unordered_map<int, VampireTalentRes> mVampireTalentDataTable;

    //// ===== Score Boss =====
    //static inline, std::unordered_map<int, ScoreBossControlRes> mScoreBossControlDataTable;
    //static inline, std::unordered_map<int, ScoreBossRewardRes> mScoreBossRewardDataTable;

    //// ===== Misc =====
    //static inline, std::unordered_map<int, WorldClassRes> mWorldClassDataTable;
    //static inline, std::unordered_map<int, GuideGroupRes> mGuideGroupDataTable;
    //static inline, std::unordered_map<int, HandbookRes> mHandbookDataTable;
    //static inline, std::unordered_map<int, SignInRes> mSignInDataTable;

    //// ===== Activity =====
    //static inline, std::unordered_map<int, ActivityRes> mActivityDataTable;

    //// Activity: Login Reward
    //static inline, std::unordered_map<int, LoginRewardGroupControlRes> mLoginRewardGroupControlDataTable;

    //// Activity: Tower Resense
    //static inline, std::unordered_map<int, TowerResenseLevelRes> mTowerResenseLevelDataTable;

    //// Activity: Trials
    //static inline, std::unordered_map<int, TrialControlRes> mTrialControlDataTable;
    //static inline, std::unordered_map<int, TrialGroupRes> mTrialGroupDataTable;

    //// Activity: Joint Drill
    //static inline, std::unordered_map<int, JointDrill2LevelRes> mJointDrill2LevelDataTable;

    //// Activity: Levels
    //static inline, std::unordered_map<int, ActivityLevelsLevelRes> mActivityLevelsLevelDataTable;

    //// Activity: Task
    //static inline, std::unordered_map<int, ActivityTaskRes> mActivityTaskDataTable;
    //static inline, std::unordered_map<int, ActivityTaskGroupRes> mActivityTaskGroupDataTable;

    //// Activity: Shop
    //static inline, std::unordered_map<int, ActivityShopRes> mActivityShopDataTable;
    //static inline, std::unordered_map<int, ActivityShopControlRes> mActivityShopControlDataTable;
    //static inline, std::unordered_map<int, ActivityGoodsRes> mActivityGoodsDataTable;
};
