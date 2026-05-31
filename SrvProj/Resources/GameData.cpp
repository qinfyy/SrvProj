#include "GameData.h"

#include "BinClass\CharacterRes.h"
#include "BinClass\DiscRes.h"
#include "BinClass\ItemsRes.h"
#include "BinClass\QuestRes.h"
#include "BinClass\StoryRes.h"

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
