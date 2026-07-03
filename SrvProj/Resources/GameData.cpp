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

std::unordered_map<int, CharacterRes> GameData::CharacterDataTable;
std::unordered_map<int, CharacterAdvanceRes> GameData::CharacterAdvanceDataTable;
std::unordered_map<int, CharacterSkillUpgradeRes> GameData::CharacterSkillUpgradeDataTable;
std::unordered_map<int, CharacterUpgradeRes> GameData::CharacterUpgradeDataTable;
std::unordered_map<int, CharItemExpRes> GameData::CharItemExpDataTable;
std::unordered_map<int, CharacterSkinRes> GameData::CharacterSkinDataTable;
std::unordered_map<int, TalentGroupRes> GameData::TalentGroupDataTable;
std::unordered_map<int, TalentRes> GameData::TalentDataTable;

// Characters: Emblems
std::unordered_map<int, CharGemRes> GameData::CharGemDataTable;
std::unordered_map<int, CharGemSlotControlRes> GameData::CharGemSlotControlDataTable;
std::unordered_map<int, CharGemAttrValueRes> GameData::CharGemAttrValueDataTable;

// Characters: Affinity
std::unordered_map<int, AffinityLevelRes> GameData::AffinityLevelDataTable;
std::unordered_map<int, AffinityGiftRes> GameData::AffinityGiftDataTable;
std::unordered_map<int, PlotRes> GameData::PlotDataTable;

// Characters: Phone
std::unordered_map<int, ChatRes> GameData::ChatDataTable;

// Characters: Dating
std::unordered_map<int, DatingLandmarkRes> GameData::DatingLandmarkDataTable;
std::unordered_map<int, DatingLandmarkEventRes> GameData::DatingLandmarkEventDataTable;
std::unordered_map<int, DatingCharacterEventRes> GameData::DatingCharacterEventDataTable;

//// ===== Discs =====
std::unordered_map<int, DiscRes> GameData::DiscDataTable;
std::unordered_map<int, DiscStrengthenRes> GameData::DiscStrengthenDataTable;
std::unordered_map<int, DiscItemExpRes> GameData::DiscItemExpDataTable;
std::unordered_map<int, DiscPromoteRes> GameData::DiscPromoteDataTable;
std::unordered_map<int, DiscPromoteLimitRes> GameData::DiscPromoteLimitDataTable;

// Discs: Melody items
std::unordered_map<int, SecondarySkillRes> GameData::SecondarySkillDataTable;

// ===== Items =====
std::unordered_map<int, ItemRes> GameData::ItemDataTable;
std::unordered_map<int, ProductionRes> GameData::ProductionDataTable;
std::unordered_map<int, PlayerHeadRes> GameData::PlayerHeadDataTable;
std::unordered_map<int, TitleRes> GameData::TitleDataTable;
std::unordered_map<int, HonorRes> GameData::HonorDataTable;
std::unordered_map<int, DropPkgRes> GameData::DropPkgDataTable;

// ===== Shops =====
std::unordered_map<std::string, MallMonthlyCardRes> GameData::MallMonthlyCardDataTable;
std::unordered_map<int, MonthlyCardRes> GameData::MonthlyCardDataTable;
std::unordered_map<std::string, MallPackageRes> GameData::MallPackageDataTable;
std::unordered_map<std::string, MallShopRes> GameData::MallShopDataTable;
std::unordered_map<std::string, MallGemRes> GameData::MallGemDataTable;

std::unordered_map<int, ResidentShopRes> GameData::ResidentShopDataTable;
std::unordered_map<int, ResidentGoodsRes> GameData::ResidentGoodsDataTable;


// ===== Battle Pass =====
std::unordered_map<int, BattlePassRes> GameData::BattlePassDataTable;
std::unordered_map<int, BattlePassLevelRes> GameData::BattlePassLevelDataTable;
std::unordered_map<int, BattlePassQuestRes> GameData::BattlePassQuestDataTable;
std::unordered_map<std::pair<int, int>, BattlePassRewardRes, ResourcePairHash> GameData::BattlePassRewardDataTable;


// ===== Commissions =====
std::unordered_map<int, AgentRes> GameData::AgentDataTable;

// ===== Dictionary =====
std::unordered_map<int, DictionaryTabRes> GameData::DictionaryTabDataTable;
std::unordered_map<int, DictionaryEntryRes> GameData::DictionaryEntryDataTable;

// ===== Gacha =====
std::unordered_map<std::pair<int, int>, GachaATypeProbRes, ResourcePairHash> GameData::GachaATypeProbDataTable;
std::unordered_map<std::pair<int, int>, GachaPkgRes, ResourcePairHash> GameData::GachaPkgDataTable;
std::unordered_map<int, GachaRes> GameData::GachaDataTable;
std::unordered_map<int, GachaNewbieRes> GameData::GachaNewbieDataTable;
std::unordered_map<int, GachaStorageRes> GameData::GachaStorageDataTable;
std::unordered_map<int, GachaTypeRes> GameData::GachaTypeDataTable;

// ===== Story =====
std::unordered_map<int, StoryRes> GameData::StoryDataTable;
std::unordered_map<int, StorySetSectionRes> GameData::StorySetSectionDataTable;
std::unordered_map<int, StoryEvidenceRes> GameData::StoryEvidenceDataTable;

std::unordered_map<int, MainScreenCGRes> GameData::MainScreenCGDataTable;

// ===== Daily/Weekly Quests =====
std::unordered_map<int, DailyQuestRes> GameData::DailyQuestDataTable;
std::unordered_map<int, DailyQuestActiveRes> GameData::DailyQuestActiveDataTable;
std::unordered_map<int, WeeklyQuestRes> GameData::WeeklyQuestDataTable;
std::unordered_map<int, WeeklyQuestActiveRes> GameData::WeeklyQuestActiveDataTable;



// ===== Achievements =====
std::unordered_map<int, AchievementRes> GameData::AchievementDataTable;

// ===== Tutorials =====
std::unordered_map<int, TutorialLevelRes> GameData::TutorialLevelDataTable;

// ===== Instances =====
std::unordered_map<int, DailyInstanceRes> GameData::DailyInstanceDataTable;
std::unordered_map<int, DailyInstanceRewardGroupRes> GameData::DailyInstanceRewardGroupDataTable;
std::unordered_map<int, RegionBossLevelRes> GameData::RegionBossLevelDataTable;
std::unordered_map<int, SkillInstanceRes> GameData::SkillInstanceDataTable;
std::unordered_map<int, CharGemInstanceRes> GameData::CharGemInstanceDataTable;
std::unordered_map<int, WeekBossLevelRes> GameData::WeekBossLevelDataTable;

// ===== Star Tower =====
std::unordered_map<int, StarTowerRes> GameData::StarTowerDataTable;
std::unordered_map<int, StarTowerStageRes> GameData::StarTowerStageDataTable;
std::unordered_map<int, StarTowerGrowthNodeRes> GameData::StarTowerGrowthNodeDataTable;
std::unordered_map<int, StarTowerFloorExpRes> GameData::StarTowerFloorExpDataTable;
std::unordered_map<int, StarTowerTeamExpRes> GameData::StarTowerTeamExpDataTable;
std::unordered_map<int, StarTowerEventRes> GameData::StarTowerEventDataTable;
std::unordered_map<int, EventOptionsRes> GameData::EventOptionsDataTable;
std::unordered_map<int, StarTowerBuildRankRes> GameData::StarTowerBuildRankDataTable;
std::unordered_map<int, SubNoteSkillDropGroupRes> GameData::SubNoteSkillDropGroupDataTable;
std::unordered_map<int, SubNoteSkillPromoteGroupRes> GameData::SubNoteSkillPromoteGroupDataTable;

std::unordered_map<int, PotentialRes> GameData::PotentialDataTable;
std::unordered_map<int, CharPotentialRes> GameData::CharPotentialDataTable;
std::unordered_map<int, NPCAffinityGroupRes> GameData::NPCAffinityGroupDataTable;
std::unordered_map<int, NPCAffinityPlotRes> GameData::NPCAffinityPlotDataTable;

std::unordered_map<int, StarTowerBookFateCardBundleRes> GameData::StarTowerBookFateCardBundleDataTable;
std::unordered_map<int, StarTowerBookFateCardQuestRes> GameData::StarTowerBookFateCardQuestDataTable;
std::unordered_map<int, StarTowerBookFateCardRes> GameData::StarTowerBookFateCardDataTable;
std::unordered_map<int, FateCardRes> GameData::FateCardDataTable;

// ===== Infinity Tower =====
std::unordered_map<int, InfinityTowerLevelRes> GameData::InfinityTowerLevelDataTable;
std::unordered_map<int, InfinityTowerDifficultyRes> GameData::InfinityTowerDifficultyDataTable;

// ===== Vampire Survivor =====
std::unordered_map<int, VampireSurvivorRes> GameData::VampireSurvivorDataTable;
std::unordered_map<int, VampireTalentRes> GameData::VampireTalentDataTable;

// ===== Score Boss =====
std::unordered_map<int, ScoreBossControlRes> GameData::ScoreBossControlDataTable;
std::unordered_map<int, ScoreBossRewardRes> GameData::ScoreBossRewardDataTable;


// ===== Misc =====
std::unordered_map<int, WorldClassRes> GameData::WorldClassDataTable;
std::unordered_map<int, GuideGroupRes> GameData::GuideGroupDataTable;
std::unordered_map<int, HandbookRes> GameData::HandbookDataTable;
std::unordered_map<std::pair<int, int>, SignInRes, ResourcePairHash> GameData::SignInDataTable;

// ===== Activity =====
std::unordered_map<int, ActivityRes> GameData::ActivityDataTable;

// Activity: Login Reward
std::unordered_map<int, LoginRewardGroupControlRes> GameData::LoginRewardGroupControlDataTable;

// Activity: Tower Defense
std::unordered_map<int, TowerDefenseLevelRes> GameData::TowerDefenseLevelDataTable;

// Activity: Trials
std::unordered_map<int, TrialControlRes> GameData::TrialControlDataTable;
std::unordered_map<int, TrialGroupRes> GameData::TrialGroupDataTable;

// Activity: Joint Drill
std::unordered_map<int, JointDrill2LevelRes> GameData::JointDrill2LevelDataTable;

// Activity: Levels
std::unordered_map<int, ActivityLevelsLevelRes> GameData::ActivityLevelsLevelDataTable;

// Activity: Task
std::unordered_map<int, ActivityTaskRes> GameData::ActivityTaskDataTable;
std::unordered_map<int, ActivityTaskGroupRes> GameData::ActivityTaskGroupDataTable;

// Activity: Shop
std::unordered_map<int, ActivityShopRes> GameData::ActivityShopDataTable;
std::unordered_map<int, ActivityShopControlRes> GameData::ActivityShopControlDataTable;
std::unordered_map<int, ActivityGoodsRes> GameData::ActivityGoodsDataTable;
