#include "GameData.h"

#include "BinClass\BattlePass.h"
#include "BinClass\CharacterRes.h"
#include "BinClass\Commissions.h"
#include "BinClass\DiscRes.h"
#include "BinClass\ItemsRes.h"
#include "BinClass\MiscRes.h"
#include "BinClass\QuestRes.h"
#include "BinClass\ShopsRes.h"
#include "BinClass\StarTower.h"
#include "BinClass\StoryRes.h"

std::unordered_map<std::string, CharacterRes> GameData::CharacterDataTable;
std::unordered_map<std::string, CharacterAdvanceRes> GameData::CharacterAdvanceDataTable;
std::unordered_map<std::string, CharacterSkillUpgradeRes> GameData::CharacterSkillUpgradeDataTable;
std::unordered_map<std::string, CharacterUpgradeRes> GameData::CharacterUpgradeDataTable;
std::unordered_map<std::string, CharItemExpRes> GameData::CharItemExpDataTable;
std::unordered_map<std::string, CharacterSkinRes> GameData::CharacterSkinDataTable;
std::unordered_map<std::string, TalentGroupRes> GameData::TalentGroupDataTable;
std::unordered_map<std::string, TalentRes> GameData::TalentDataTable;

// Characters: Emblems
std::unordered_map<std::string, CharGemRes> GameData::CharGemDataTable;
std::unordered_map<std::string, CharGemSlotControlRes> GameData::CharGemSlotControlDataTable;
std::unordered_map<std::string, CharGemAttrValueRes> GameData::CharGemAttrValueDataTable;

// Characters: Affinity
std::unordered_map<std::string, AffinityLevelRes> GameData::AffinityLevelDataTable;
std::unordered_map<std::string, AffinityGiftRes> GameData::AffinityGiftDataTable;
std::unordered_map<std::string, PlotRes> GameData::PlotDataTable;

// Characters: Phone
std::unordered_map<std::string, ChatRes> GameData::ChatDataTable;

// Characters: Dating
std::unordered_map<std::string, DatingLandmarkRes> GameData::DatingLandmarkDataTable;
std::unordered_map<std::string, DatingLandmarkEventRes> GameData::DatingLandmarkEventDataTable;
std::unordered_map<std::string, DatingCharacterEventRes> GameData::DatingCharacterEventDataTable;

//// ===== Discs =====
std::unordered_map<std::string, DiscRes> GameData::DiscDataTable;
std::unordered_map<std::string, DiscStrengthenRes> GameData::DiscStrengthenDataTable;
std::unordered_map<std::string, DiscItemExpRes> GameData::DiscItemExpDataTable;
std::unordered_map<std::string, DiscPromoteRes> GameData::DiscPromoteDataTable;
std::unordered_map<std::string, DiscPromoteLimitRes> GameData::DiscPromoteLimitDataTable;

// Discs: Melody items
std::unordered_map<std::string, SecondarySkillRes> GameData::SecondarySkillDataTable;

// ===== Items =====
std::unordered_map<std::string, ItemRes> GameData::ItemDataTable;
std::unordered_map<std::string, ProductionRes> GameData::ProductionDataTable;
std::unordered_map<std::string, PlayerHeadRes> GameData::PlayerHeadDataTable;
std::unordered_map<std::string, TitleRes> GameData::TitleDataTable;
std::unordered_map<std::string, HonorRes> GameData::HonorDataTable;

// ===== Shops =====
std::unordered_map<std::string, MallMonthlyCardRes> GameData::MallMonthlyCardDataTable;
std::unordered_map<std::string, MallPackageRes> GameData::MallPackageDataTable;
std::unordered_map<std::string, MallShopRes> GameData::MallShopDataTable;
std::unordered_map<std::string, MallGemRes> GameData::MallGemDataTable;

std::unordered_map<std::string, ResidentShopRes> GameData::ResidentShopDataTable;
std::unordered_map<std::string, ResidentGoodsRes> GameData::ResidentGoodsDataTable;


// ===== Battle Pass =====
std::unordered_map<std::string, BattlePassRes> GameData::BattlePassDataTable;
std::unordered_map<std::string, BattlePassLevelRes> GameData::BattlePassLevelDataTable;
std::unordered_map<std::string, BattlePassQuestRes> GameData::BattlePassQuestDataTable;
std::unordered_map<std::string, BattlePassRewardRes> GameData::BattlePassRewardDataTable;


// ===== Commissions =====
std::unordered_map<std::string, AgentRes> GameData::AgentDataTable;

//// ===== Dictionary =====
//static std::unordered_map<std::string, DictionaryTabRes> mDictionaryTabDataTable;
//static std::unordered_map<std::string, DictionaryEntryRes> mDictionaryEntryDataTable;

//// ===== Gacha =====
//static std::unordered_map<std::string, GachaATypeProbRes> mGachaATypeProbDataTable;
//static std::unordered_map<std::string, GachaRes> mGachaDataTable;
//static std::unordered_map<std::string, GachaNewbieRes> mGachaNewbieDataTable;
//static std::unordered_map<std::string, GachaStorageRes> mGachaStorageDataTable;
//static std::unordered_map<std::string, GachaTypeRes> mGachaTypeDataTable;

// ===== Story =====
std::unordered_map<std::string, StoryRes> GameData::StoryDataTable;
std::unordered_map<std::string, StorySetSectionRes> GameData::StorySetSectionDataTable;
std::unordered_map<std::string, StoryEvidenceRes> GameData::StoryEvidenceDataTable;

std::unordered_map<std::string, MainScreenCGRes> GameData::MainScreenCGDataTable;

// ===== Daily/Weekly Quests =====
std::unordered_map<std::string, DailyQuestRes> GameData::DailyQuestDataTable;
std::unordered_map<std::string, DailyQuestActiveRes> GameData::DailyQuestActiveDataTable;
std::unordered_map<std::string, WeeklyQuestRes> GameData::WeeklyQuestDataTable;
std::unordered_map<std::string, WeeklyQuestActiveRes> GameData::WeeklyQuestActiveDataTable;



//// ===== Achievements =====
//static std::unordered_map<std::string, AchievementRes> mAchievementDataTable;

//// ===== Tutorials =====
//static std::unordered_map<std::string, TutorialLevelRes> mTutorialLevelDataTable;

//// ===== Instances =====
//static std::unordered_map<std::string, DailyInstanceRes> mDailyInstanceDataTable;
//static std::unordered_map<std::string, DailyInstanceRewardGroupRes> mDailyInstanceRewardGroupDataTable;
//static std::unordered_map<std::string, RegionBossLevelRes> mRegionBossLevelDataTable;
//static std::unordered_map<std::string, SkillInstanceRes> mSkillInstanceDataTable;
//static std::unordered_map<std::string, CharGemInstanceRes> mCharGemInstanceDataTable;
//static std::unordered_map<std::string, WeekBossLevelRes> mWeekBossLevelDataTable;

// ===== Star Tower =====
std::unordered_map<std::string, StarTowerRes> GameData::StarTowerDataTable;
std::unordered_map<std::string, StarTowerStageRes> GameData::StarTowerStageDataTable;
std::unordered_map<std::string, StarTowerGrowthNodeRes> GameData::StarTowerGrowthNodeDataTable;
std::unordered_map<std::string, StarTowerFloorExpRes> GameData::StarTowerFloorExpDataTable;
std::unordered_map<std::string, StarTowerTeamExpRes> GameData::StarTowerTeamExpDataTable;
std::unordered_map<std::string, StarTowerEventRes> GameData::StarTowerEventDataTable;
std::unordered_map<std::string, StarTowerBuildRankRes> GameData::StarTowerBuildRankDataTable;
std::unordered_map<std::string, SubNoteSkillPromoteGroupRes> GameData::SubNoteSkillPromoteGroupDataTable;

std::unordered_map<std::string, PotentialRes> GameData::PotentialDataTable;
std::unordered_map<std::string, CharPotentialRes> GameData::CharPotentialDataTable;

std::unordered_map<std::string, StarTowerBookFateCardBundleRes> GameData::StarTowerBookFateCardBundleDataTable;
std::unordered_map<std::string, StarTowerBookFateCardQuestRes> GameData::StarTowerBookFateCardQuestDataTable;
std::unordered_map<std::string, StarTowerBookFateCardRes> GameData::StarTowerBookFateCardDataTable;
std::unordered_map<std::string, FateCardRes> GameData::FateCardDataTable;

// ===== Infinity Tower =====
std::unordered_map<std::string, InfinityTowerLevelRes> GameData::InfinityTowerLevelDataTable;
std::unordered_map<std::string, InfinityTowerDifficultyRes> GameData::InfinityTowerDifficultyDataTable;

//// ===== Vampire Survivor =====
//static std::unordered_map<std::string, VampireSurvivorRes> mVampireSurvivorDataTable;
//static std::unordered_map<std::string, VampireTalentRes> mVampireTalentDataTable;

//// ===== Score Boss =====
//static std::unordered_map<std::string, ScoreBossControlRes> mScoreBossControlDataTable;
//static std::unordered_map<std::string, ScoreBossRewardRes> mScoreBossRewardDataTable;


// ===== Misc =====
std::unordered_map<std::string, WorldClassRes> GameData::WorldClassDataTable;
std::unordered_map<std::string, GuideGroupRes> GameData::GuideGroupDataTable;
std::unordered_map<std::string, HandbookRes> GameData::HandbookDataTable;
std::unordered_map<std::string, SignInRes> GameData::SignInDataTable;

//// ===== Activity =====
    //static std::unordered_map<std::string, ActivityRes> mActivityDataTable;

    //// Activity: Login Reward
    //static std::unordered_map<std::string, LoginRewardGroupControlRes> mLoginRewardGroupControlDataTable;

    //// Activity: Tower Resense
    //static std::unordered_map<std::string, TowerResenseLevelRes> mTowerResenseLevelDataTable;

    //// Activity: Trials
    //static std::unordered_map<std::string, TrialControlRes> mTrialControlDataTable;
    //static std::unordered_map<std::string, TrialGroupRes> mTrialGroupDataTable;

    //// Activity: Joint Drill
    //static std::unordered_map<std::string, JointDrill2LevelRes> mJointDrill2LevelDataTable;

    //// Activity: Levels
    //static std::unordered_map<std::string, ActivityLevelsLevelRes> mActivityLevelsLevelDataTable;

    //// Activity: Task
    //static std::unordered_map<std::string, ActivityTaskRes> mActivityTaskDataTable;
    //static std::unordered_map<std::string, ActivityTaskGroupRes> mActivityTaskGroupDataTable;

    //// Activity: Shop
    //static std::unordered_map<std::string, ActivityShopRes> mActivityShopDataTable;
    //static std::unordered_map<std::string, ActivityShopControlRes> mActivityShopControlDataTable;
    //static std::unordered_map<std::string, ActivityGoodsRes> mActivityGoodsDataTable;
