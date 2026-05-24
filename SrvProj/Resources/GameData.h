#pragma once

#include <unordered_map>
#include <memory>
#include ".\BinClass\Character.h"
//#include ".\BinClass\CharacterAdvanceDef.h"
//#include ".\BinClass\CharacterSkillUpgradeDef.h"
//#include ".\BinClass\CharacterUpgradeDef.h"
//#include ".\BinClass\CharItemExpDef.h"
//#include ".\BinClass\CharacterSkinDef.h"
//#include ".\BinClass\TalentGroupDef.h"
//#include ".\BinClass\TalentDef.h"
//#include ".\BinClass\CharGemDef.h"
//#include ".\BinClass\CharGemSlotControlDef.h"
//#include ".\BinClass\CharGemAttrGroupDef.h"
//#include ".\BinClass\CharGemAttrValueDef.h"
//#include ".\BinClass\AffinityLevelDef.h"
//#include ".\BinClass\AffinityGiftDef.h"
//#include ".\BinClass\PlotDef.h"
//#include ".\BinClass\ChatDef.h"
//#include ".\BinClass\DatingLandmarkDef.h"
//#include ".\BinClass\DatingLandmarkEventDef.h"
//#include ".\BinClass\DatingCharacterEventDef.h"
//#include ".\BinClass\DiscDef.h"
//#include ".\BinClass\DiscStrengthenDef.h"
//#include ".\BinClass\DiscItemExpDef.h"
//#include ".\BinClass\DiscPromoteDef.h"
//#include ".\BinClass\DiscPromoteLimitDef.h"
//#include ".\BinClass\SecondarySkillDef.h"
//#include ".\BinClass\ItemDef.h"
//#include ".\BinClass\ProductionDef.h"
//#include ".\BinClass\PlayerHeadDef.h"
//#include ".\BinClass\TitleDef.h"
//#include ".\BinClass\HonorDef.h"
//#include ".\BinClass\MallMonthlyCardDef.h"
//#include ".\BinClass\MallPackageDef.h"
//#include ".\BinClass\MallShopDef.h"
//#include ".\BinClass\MallGemDef.h"
//#include ".\BinClass\ResidentShopDef.h"
//#include ".\BinClass\ResidentGoodsDef.h"
//#include ".\BinClass\BattlePassDef.h"
//#include ".\BinClass\BattlePassLevelDef.h"
//#include ".\BinClass\BattlePassQuestDef.h"
//#include ".\BinClass\BattlePassRewardDef.h"
//#include ".\BinClass\AgentDef.h"
//#include ".\BinClass\DictionaryTabDef.h"
//#include ".\BinClass\DictionaryEntryDef.h"
//#include ".\BinClass\GachaATypeProbDef.h"
//#include ".\BinClass\GachaDef.h"
//#include ".\BinClass\GachaNewbieDef.h"
//#include ".\BinClass\GachaStorageDef.h"
//#include ".\BinClass\GachaTypeDef.h"
//#include ".\BinClass\StoryDef.h"
//#include ".\BinClass\StorySetSectionDef.h"
//#include ".\BinClass\StoryEvidenceDef.h"
//#include ".\BinClass\MainScreenCGDef.h"
//#include ".\BinClass\DailyQuestDef.h"
//#include ".\BinClass\DailyQuestActiveDef.h"
//#include ".\BinClass\WeeklyQuestDef.h"
//#include ".\BinClass\WeeklyQuestActiveDef.h"
//#include ".\BinClass\AchievementDef.h"
//#include ".\BinClass\TutorialLevelDef.h"
//#include ".\BinClass\DailyInstanceDef.h"
//#include ".\BinClass\DailyInstanceRewardGroupDef.h"
//#include ".\BinClass\RegionBossLevelDef.h"
//#include ".\BinClass\SkillInstanceDef.h"
//#include ".\BinClass\CharGemInstanceDef.h"
//#include ".\BinClass\WeekBossLevelDef.h"
//#include ".\BinClass\StarTowerDef.h"
//#include ".\BinClass\StarTowerStageDef.h"
//#include ".\BinClass\StarTowerGrowthNodeDef.h"
//#include ".\BinClass\StarTowerFloorExpDef.h"
//#include ".\BinClass\StarTowerTeamExpDef.h"
//#include ".\BinClass\StarTowerEventDef.h"
//#include ".\BinClass\StarTowerBuildRankDef.h"
//#include ".\BinClass\SubNoteSkillPromoteGroupDef.h"
//#include ".\BinClass\PotentialDef.h"
//#include ".\BinClass\CharPotentialDef.h"
//#include ".\BinClass\StarTowerBookFateCardBundleDef.h"
//#include ".\BinClass\StarTowerBookFateCardQuestDef.h"
//#include ".\BinClass\StarTowerBookFateCardDef.h"
//#include ".\BinClass\FateCardDef.h"
//#include ".\BinClass\InfinityTowerLevelDef.h"
//#include ".\BinClass\InfinityTowerDifficultyDef.h"
//#include ".\BinClass\VampireSurvivorDef.h"
//#include ".\BinClass\VampireTalentDef.h"
//#include ".\BinClass\ScoreBossControlDef.h"
//#include ".\BinClass\ScoreBossRewardDef.h"
//#include ".\BinClass\WorldClassDef.h"
//#include ".\BinClass\GuideGroupDef.h"
//#include ".\BinClass\HandbookDef.h"
//#include ".\BinClass\SignInDef.h"
//#include ".\BinClass\ActivityDef.h"
//#include ".\BinClass\LoginRewardGroupControlDef.h"
//#include ".\BinClass\TowerDefenseLevelDef.h"
//#include ".\BinClass\TrialControlDef.h"
//#include ".\BinClass\TrialGroupDef.h"
//#include ".\BinClass\JointDrill2LevelDef.h"
//#include ".\BinClass\ActivityLevelsLevelDef.h"
//#include ".\BinClass\ActivityTaskDef.h"
//#include ".\BinClass\ActivityTaskGroupDef.h"
//#include ".\BinClass\ActivityShopDef.h"
//#include ".\BinClass\ActivityShopControlDef.h"
//#include ".binActivityGoodsDef.h"

class GameData {
public:
    // 禁止实例化
    GameData() = delete;
    ~GameData() = delete;
    GameData(const GameData&) = delete;
    GameData& operator=(const GameData&) = delete;

    // ===== Characters =====
    static inline std::unordered_map<int, CharacterDef> mCharacterDataTable;
    //static inline std::unordered_map<int, CharacterAdvanceDef> mCharacterAdvanceDataTable;
    //static inline std::unordered_map<int, CharacterSkillUpgradeDef> mCharacterSkillUpgradeDataTable;
    //static inline std::unordered_map<int, CharacterUpgradeDef> mCharacterUpgradeDataTable;
    //static inline std::unordered_map<int, CharItemExpDef> mCharItemExpDataTable;
    //static inline std::unordered_map<int, CharacterSkinDef> mCharacterSkinDataTable;
    //static inline std::unordered_map<int, TalentGroupDef> mTalentGroupDataTable;
    //static inline std::unordered_map<int, TalentDef> mTalentDataTable;

    //// Characters: Emblems
    //static inline std::unordered_map<int, CharGemDef> mCharGemDataTable;
    //static inline std::unordered_map<int, CharGemSlotControlDef> mCharGemSlotControlDataTable;
    //static inline std::unordered_map<int, CharGemAttrGroupDef> mCharGemAttrGroupDataTable;
    //static inline std::unordered_map<int, CharGemAttrValueDef> mCharGemAttrValueDataTable;

    //// Characters: Affinity
    //static inline std::unordered_map<int, AffinityLevelDef> mAffinityLevelDataTable;
    //static inline std::unordered_map<int, AffinityGiftDef> mAffinityGiftDataTable;
    //static inline std::unordered_map<int, PlotDef> mPlotDataTable;

    //// Characters: Phone
    //static inline std::unordered_map<int, ChatDef> mChatDataTable;

    //// Characters: Dating
    //static inline std::unordered_map<int, DatingLandmarkDef> mDatingLandmarkDataTable;
    //static inline std::unordered_map<int, DatingLandmarkEventDef> mDatingLandmarkEventDataTable;
    //static inline std::unordered_map<int, DatingCharacterEventDef> mDatingCharacterEventDataTable;

    //// ===== Discs =====
    //static inline std::unordered_map<int, DiscDef> mDiscDataTable;
    //static inline std::unordered_map<int, DiscStrengthenDef> mDiscStrengthenDataTable;
    //static inline std::unordered_map<int, DiscItemExpDef> mDiscItemExpDataTable;
    //static inline std::unordered_map<int, DiscPromoteDef> mDiscPromoteDataTable;
    //static inline std::unordered_map<int, DiscPromoteLimitDef> mDiscPromoteLimitDataTable;

    //// Discs: Melody items
    //static inline std::unordered_map<int, SecondarySkillDef> mSecondarySkillDataTable;

    //// ===== Items =====
    //static inline std::unordered_map<int, ItemDef> mItemDataTable;
    //static inline std::unordered_map<int, ProductionDef> mProductionDataTable;
    //static inline std::unordered_map<int, PlayerHeadDef> mPlayerHeadDataTable;
    //static inline std::unordered_map<int, TitleDef> mTitleDataTable;
    //static inline std::unordered_map<int, HonorDef> mHonorDataTable;

    //// ===== Shops =====
    //static inline std::unordered_map<int, MallMonthlyCardDef> mMallMonthlyCardDataTable;
    //static inline std::unordered_map<int, MallPackageDef> mMallPackageDataTable;
    //static inline std::unordered_map<int, MallShopDef> mMallShopDataTable;
    //static inline std::unordered_map<int, MallGemDef> mMallGemDataTable;

    //static inline std::unordered_map<int, ResidentShopDef> mResidentShopDataTable;
    //static inline std::unordered_map<int, ResidentGoodsDef> mResidentGoodsDataTable;

    //// ===== Battle Pass =====
    //static inline std::unordered_map<int, BattlePassDef> mBattlePassDataTable;
    //static inline std::unordered_map<int, BattlePassLevelDef> mBattlePassLevelDataTable;
    //static inline std::unordered_map<int, BattlePassQuestDef> mBattlePassQuestDataTable;
    //static inline std::unordered_map<int, BattlePassRewardDef> mBattlePassRewardDataTable;

    //// ===== Commissions =====
    //static inline std::unordered_map<int, AgentDef> mAgentDataTable;

    //// ===== Dictionary =====
    //static inline std::unordered_map<int, DictionaryTabDef> mDictionaryTabDataTable;
    //static inline std::unordered_map<int, DictionaryEntryDef> mDictionaryEntryDataTable;

    //// ===== Gacha =====
    //static inline std::unordered_map<int, GachaATypeProbDef> mGachaATypeProbDataTable;
    //static inline std::unordered_map<int, GachaDef> mGachaDataTable;
    //static inline std::unordered_map<int, GachaNewbieDef> mGachaNewbieDataTable;
    //static inline std::unordered_map<int, GachaStorageDef> mGachaStorageDataTable;
    //static inline std::unordered_map<int, GachaTypeDef> mGachaTypeDataTable;

    //// ===== Story =====
    //static inline std::unordered_map<int, StoryDef> mStoryDataTable;
    //static inline std::unordered_map<int, StorySetSectionDef> mStorySetSectionDataTable;
    //static inline std::unordered_map<int, StoryEvidenceDef> mStoryEvidenceDataTable;

    //static inline std::unordered_map<int, MainScreenCGDef> mMainScreenCGDataTable;

    //// ===== Daily/Weekly Quests =====
    //static inline std::unordered_map<int, DailyQuestDef> mDailyQuestDataTable;
    //static inline std::unordered_map<int, DailyQuestActiveDef> mDailyQuestActiveDataTable;
    //static inline std::unordered_map<int, WeeklyQuestDef> mWeeklyQuestDataTable;
    //static inline std::unordered_map<int, WeeklyQuestActiveDef> mWeeklyQuestActiveDataTable;

    //// ===== Achievements =====
    //static inline std::unordered_map<int, AchievementDef> mAchievementDataTable;

    //// ===== Tutorials =====
    //static inline std::unordered_map<int, TutorialLevelDef> mTutorialLevelDataTable;

    //// ===== Instances =====
    //static inline std::unordered_map<int, DailyInstanceDef> mDailyInstanceDataTable;
    //static inline std::unordered_map<int, DailyInstanceRewardGroupDef> mDailyInstanceRewardGroupDataTable;
    //static inline std::unordered_map<int, RegionBossLevelDef> mRegionBossLevelDataTable;
    //static inline std::unordered_map<int, SkillInstanceDef> mSkillInstanceDataTable;
    //static inline std::unordered_map<int, CharGemInstanceDef> mCharGemInstanceDataTable;
    //static inline std::unordered_map<int, WeekBossLevelDef> mWeekBossLevelDataTable;

    //// ===== Star Tower =====
    //static inline std::unordered_map<int, StarTowerDef> mStarTowerDataTable;
    //static inline std::unordered_map<int, StarTowerStageDef> mStarTowerStageDataTable;
    //static inline std::unordered_map<int, StarTowerGrowthNodeDef> mStarTowerGrowthNodeDataTable;
    //static inline std::unordered_map<int, StarTowerFloorExpDef> mStarTowerFloorExpDataTable;
    //static inline std::unordered_map<int, StarTowerTeamExpDef> mStarTowerTeamExpDataTable;
    //static inline std::unordered_map<int, StarTowerEventDef> mStarTowerEventDataTable;
    //static inline std::unordered_map<int, StarTowerBuildRankDef> mStarTowerBuildRankDataTable;
    //static inline std::unordered_map<int, SubNoteSkillPromoteGroupDef> mSubNoteSkillPromoteGroupDataTable;

    //static inline std::unordered_map<int, PotentialDef> mPotentialDataTable;
    //static inline std::unordered_map<int, CharPotentialDef> mCharPotentialDataTable;

    //static inline std::unordered_map<int, StarTowerBookFateCardBundleDef> mStarTowerBookFateCardBundleDataTable;
    //static inline std::unordered_map<int, StarTowerBookFateCardQuestDef> mStarTowerBookFateCardQuestDataTable;
    //static inline std::unordered_map<int, StarTowerBookFateCardDef> mStarTowerBookFateCardDataTable;
    //static inline std::unordered_map<int, FateCardDef> mFateCardDataTable;

    //// ===== Infinity Tower =====
    //static inline std::unordered_map<int, InfinityTowerLevelDef> mInfinityTowerLevelDataTable;
    //static inline std::unordered_map<int, InfinityTowerDifficultyDef> mInfinityTowerDifficultyDataTable;

    //// ===== Vampire Survivor =====
    //static inline std::unordered_map<int, VampireSurvivorDef> mVampireSurvivorDataTable;
    //static inline std::unordered_map<int, VampireTalentDef> mVampireTalentDataTable;

    //// ===== Score Boss =====
    //static inline std::unordered_map<int, ScoreBossControlDef> mScoreBossControlDataTable;
    //static inline std::unordered_map<int, ScoreBossRewardDef> mScoreBossRewardDataTable;

    //// ===== Misc =====
    //static inline std::unordered_map<int, WorldClassDef> mWorldClassDataTable;
    //static inline std::unordered_map<int, GuideGroupDef> mGuideGroupDataTable;
    //static inline std::unordered_map<int, HandbookDef> mHandbookDataTable;
    //static inline std::unordered_map<int, SignInDef> mSignInDataTable;

    //// ===== Activity =====
    //static inline std::unordered_map<int, ActivityDef> mActivityDataTable;

    //// Activity: Login Reward
    //static inline std::unordered_map<int, LoginRewardGroupControlDef> mLoginRewardGroupControlDataTable;

    //// Activity: Tower Defense
    //static inline std::unordered_map<int, TowerDefenseLevelDef> mTowerDefenseLevelDataTable;

    //// Activity: Trials
    //static inline std::unordered_map<int, TrialControlDef> mTrialControlDataTable;
    //static inline std::unordered_map<int, TrialGroupDef> mTrialGroupDataTable;

    //// Activity: Joint Drill
    //static inline std::unordered_map<int, JointDrill2LevelDef> mJointDrill2LevelDataTable;

    //// Activity: Levels
    //static inline std::unordered_map<int, ActivityLevelsLevelDef> mActivityLevelsLevelDataTable;

    //// Activity: Task
    //static inline std::unordered_map<int, ActivityTaskDef> mActivityTaskDataTable;
    //static inline std::unordered_map<int, ActivityTaskGroupDef> mActivityTaskGroupDataTable;

    //// Activity: Shop
    //static inline std::unordered_map<int, ActivityShopDef> mActivityShopDataTable;
    //static inline std::unordered_map<int, ActivityShopControlDef> mActivityShopControlDataTable;
    //static inline std::unordered_map<int, ActivityGoodsDef> mActivityGoodsDataTable;
};
