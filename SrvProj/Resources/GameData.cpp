#include "GameData.h"

#include "BinClass\CharacterRes.h"
#include "BinClass\DiscRes.h"
#include "BinClass\ItemsRes.h"
#include "BinClass\QuestRes.h"
#include "BinClass\StoryRes.h"
#include "BinClass\StarTower.h"


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


std::unordered_map<std::string, StarTowerGrowthNodeRes> GameData::StarTowerGrowthNodeDataTable;
