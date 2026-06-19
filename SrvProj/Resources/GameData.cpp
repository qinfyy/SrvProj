#include "GameData.h"

#include "BinClass\AchievementsRes.h"
#include "BinClass\ActivityRes.h"
#include "BinClass\BattlePassRes.h"
#include "BinClass\CharacterRes.h"
#include "BinClass\CommissionsRes.h"
#include "BinClass\DictionaryRes.h"
#include "BinClass\DiscRes.h"
#include "BinClass\GachaRes.h"
#include "BinClass\InstancesRes.h"
#include "BinClass\ItemsRes.h"
#include "BinClass\MiscRes.h"
#include "BinClass\QuestRes.h"
#include "BinClass\ShopsRes.h"
#include "BinClass\ScoreBossRes.h"
#include "BinClass\StarTowerRes.h"
#include "BinClass\StoryRes.h"
#include "BinClass\TutorialsRes.h"
#include "BinClass\VampireSurvivorRes.h"

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
std::unordered_map<std::string, DropPkgRes> GameData::DropPkgDataTable;

// ===== Shops =====
std::unordered_map<std::string, MallMonthlyCardRes> GameData::MallMonthlyCardDataTable;
std::unordered_map<std::string, MonthlyCardRes> GameData::MonthlyCardDataTable;
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

// ===== Dictionary =====
std::unordered_map<std::string, DictionaryTabRes> GameData::DictionaryTabDataTable;
std::unordered_map<std::string, DictionaryEntryRes> GameData::DictionaryEntryDataTable;

// ===== Gacha =====
std::unordered_map<std::string, GachaATypeProbRes> GameData::GachaATypeProbDataTable;
std::unordered_map<std::string, GachaPkgRes> GameData::GachaPkgDataTable;
std::unordered_map<std::string, GachaRes> GameData::GachaDataTable;
std::unordered_map<std::string, GachaNewbieRes> GameData::GachaNewbieDataTable;
std::unordered_map<std::string, GachaStorageRes> GameData::GachaStorageDataTable;
std::unordered_map<std::string, GachaTypeRes> GameData::GachaTypeDataTable;

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



// ===== Achievements =====
std::unordered_map<std::string, AchievementRes> GameData::AchievementDataTable;

// ===== Tutorials =====
std::unordered_map<std::string, TutorialLevelRes> GameData::TutorialLevelDataTable;

// ===== Instances =====
std::unordered_map<std::string, DailyInstanceRes> GameData::DailyInstanceDataTable;
std::unordered_map<std::string, DailyInstanceRewardGroupRes> GameData::DailyInstanceRewardGroupDataTable;
std::unordered_map<std::string, RegionBossLevelRes> GameData::RegionBossLevelDataTable;
std::unordered_map<std::string, SkillInstanceRes> GameData::SkillInstanceDataTable;
std::unordered_map<std::string, CharGemInstanceRes> GameData::CharGemInstanceDataTable;
std::unordered_map<std::string, WeekBossLevelRes> GameData::WeekBossLevelDataTable;

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

// ===== Vampire Survivor =====
std::unordered_map<std::string, VampireSurvivorRes> GameData::VampireSurvivorDataTable;
std::unordered_map<std::string, VampireTalentRes> GameData::VampireTalentDataTable;

// ===== Score Boss =====
std::unordered_map<std::string, ScoreBossControlRes> GameData::ScoreBossControlDataTable;
std::unordered_map<std::string, ScoreBossRewardRes> GameData::ScoreBossRewardDataTable;


// ===== Misc =====
std::unordered_map<std::string, WorldClassRes> GameData::WorldClassDataTable;
std::unordered_map<std::string, GuideGroupRes> GameData::GuideGroupDataTable;
std::unordered_map<std::string, HandbookRes> GameData::HandbookDataTable;
std::unordered_map<std::string, SignInRes> GameData::SignInDataTable;

// ===== Activity =====
std::unordered_map<std::string, ActivityRes> GameData::ActivityDataTable;

// Activity: Login Reward
std::unordered_map<std::string, LoginRewardGroupControlRes> GameData::LoginRewardGroupControlDataTable;

// Activity: Tower Defense
std::unordered_map<std::string, TowerDefenseLevelRes> GameData::TowerDefenseLevelDataTable;

// Activity: Trials
std::unordered_map<std::string, TrialControlRes> GameData::TrialControlDataTable;
std::unordered_map<std::string, TrialGroupRes> GameData::TrialGroupDataTable;

// Activity: Joint Drill
std::unordered_map<std::string, JointDrill2LevelRes> GameData::JointDrill2LevelDataTable;

// Activity: Levels
std::unordered_map<std::string, ActivityLevelsLevelRes> GameData::ActivityLevelsLevelDataTable;

// Activity: Task
std::unordered_map<std::string, ActivityTaskRes> GameData::ActivityTaskDataTable;
std::unordered_map<std::string, ActivityTaskGroupRes> GameData::ActivityTaskGroupDataTable;

// Activity: Shop
std::unordered_map<std::string, ActivityShopRes> GameData::ActivityShopDataTable;
std::unordered_map<std::string, ActivityShopControlRes> GameData::ActivityShopControlDataTable;
std::unordered_map<std::string, ActivityGoodsRes> GameData::ActivityGoodsDataTable;
