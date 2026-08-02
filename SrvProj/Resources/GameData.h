#pragma once

#include <functional>
#include <unordered_map>
#include <memory>
#include <string>
#include <utility>

// 这里使用前置声明，在GameData.cpp和其他使用的地方中包含具体的头文件，避免在这里引入过多的依赖，导致后面触发循环依赖。
class CharacterRes;
class CharacterDesRes;
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
class MonthlyCardRes;
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
class GachaPkgRes;
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
class EventOptionsRes;
class StarTowerBuildRankRes;
class SubNoteSkillDropGroupRes;
class SubNoteSkillPromoteGroupRes;
class PotentialRes;
class CharPotentialRes;
class NPCAffinityGroupRes;
class NPCAffinityPlotRes;
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
class TowerDefenseLevelRes;
class TrialControlRes;
class TrialGroupRes;
class JointDrill2LevelRes;
class ActivityLevelsLevelRes;
class ActivityTaskRes;
class ActivityTaskGroupRes;
class ActivityShopRes;
class ActivityShopControlRes;
class ActivityGoodsRes;
class DropPkgRes;

class ResourcePairHash {
public:
    size_t operator()(const std::pair<int, int>& value) const noexcept {
        return std::hash<int>{}(value.first) * 1315423911u + std::hash<int>{}(value.second);
    }
};

class GameData {
public:
    // 禁止实例化
    GameData() = delete;
    ~GameData() = delete;
    GameData(const GameData&) = delete;
    GameData& operator=(const GameData&) = delete;

    // Characters
    static std::unordered_map<int, CharacterRes> CharacterDataTable;
    static std::unordered_map<int, CharacterDesRes> CharacterDesDataTable;
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

    // ===== Discs =====
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
    static std::unordered_map<int, DropPkgRes> DropPkgDataTable;
    // ===== Shops =====
    static std::unordered_map<std::string, MallMonthlyCardRes> MallMonthlyCardDataTable;
    static std::unordered_map<int, MonthlyCardRes> MonthlyCardDataTable;
    static std::unordered_map<std::string, MallPackageRes> MallPackageDataTable;
    static std::unordered_map<std::string, MallShopRes> MallShopDataTable;
    static std::unordered_map<std::string, MallGemRes> MallGemDataTable;

    static std::unordered_map<int, ResidentShopRes> ResidentShopDataTable;
    static std::unordered_map<int, ResidentGoodsRes> ResidentGoodsDataTable;

    // ===== Battle Pass =====
    static std::unordered_map<int, BattlePassRes> BattlePassDataTable;
    static std::unordered_map<int, BattlePassLevelRes> BattlePassLevelDataTable;
    static std::unordered_map<int, BattlePassQuestRes> BattlePassQuestDataTable;
    static std::unordered_map<std::pair<int, int>, BattlePassRewardRes, ResourcePairHash> BattlePassRewardDataTable;

    // ===== Commissions =====
    static std::unordered_map<int, AgentRes> AgentDataTable;

    // ===== Dictionary =====
    static std::unordered_map<int, DictionaryTabRes> DictionaryTabDataTable;
    static std::unordered_map<int, DictionaryEntryRes> DictionaryEntryDataTable;

    // ===== Gacha =====
    static std::unordered_map<std::pair<int, int>, GachaATypeProbRes, ResourcePairHash> GachaATypeProbDataTable;
    static std::unordered_map<std::pair<int, int>, GachaPkgRes, ResourcePairHash> GachaPkgDataTable;
    static std::unordered_map<int, GachaRes> GachaDataTable;
    static std::unordered_map<int, GachaNewbieRes> GachaNewbieDataTable;
    static std::unordered_map<int, GachaStorageRes> GachaStorageDataTable;
    static std::unordered_map<int, GachaTypeRes> GachaTypeDataTable;

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

    // ===== Achievements =====
    static std::unordered_map<int, AchievementRes> AchievementDataTable;

    // ===== Tutorials =====
    static std::unordered_map<int, TutorialLevelRes> TutorialLevelDataTable;

    // ===== Instances =====
    static std::unordered_map<int, DailyInstanceRes> DailyInstanceDataTable;
    static std::unordered_map<int, DailyInstanceRewardGroupRes> DailyInstanceRewardGroupDataTable;
    static std::unordered_map<int, RegionBossLevelRes> RegionBossLevelDataTable;
    static std::unordered_map<int, SkillInstanceRes> SkillInstanceDataTable;
    static std::unordered_map<int, CharGemInstanceRes> CharGemInstanceDataTable;
    static std::unordered_map<int, WeekBossLevelRes> WeekBossLevelDataTable;

    // ===== Star Tower =====
    static std::unordered_map<int, StarTowerRes> StarTowerDataTable;
    static std::unordered_map<int, StarTowerStageRes> StarTowerStageDataTable;
    static std::unordered_map<int, StarTowerGrowthNodeRes> StarTowerGrowthNodeDataTable;
    static std::unordered_map<int, StarTowerFloorExpRes> StarTowerFloorExpDataTable;
    static std::unordered_map<int, StarTowerTeamExpRes> StarTowerTeamExpDataTable;
    static std::unordered_map<int, StarTowerEventRes> StarTowerEventDataTable;
    static std::unordered_map<int, EventOptionsRes> EventOptionsDataTable;
    static std::unordered_map<int, StarTowerBuildRankRes> StarTowerBuildRankDataTable;
    static std::unordered_map<int, SubNoteSkillDropGroupRes> SubNoteSkillDropGroupDataTable;
    static std::unordered_map<int, SubNoteSkillPromoteGroupRes> SubNoteSkillPromoteGroupDataTable;

    static std::unordered_map<int, PotentialRes> PotentialDataTable;
    static std::unordered_map<int, CharPotentialRes> CharPotentialDataTable;
    static std::unordered_map<int, NPCAffinityGroupRes> NPCAffinityGroupDataTable;
    static std::unordered_map<int, NPCAffinityPlotRes> NPCAffinityPlotDataTable;

    static std::unordered_map<int, StarTowerBookFateCardBundleRes> StarTowerBookFateCardBundleDataTable;
    static std::unordered_map<int, StarTowerBookFateCardQuestRes> StarTowerBookFateCardQuestDataTable;
    static std::unordered_map<int, StarTowerBookFateCardRes> StarTowerBookFateCardDataTable;
    static std::unordered_map<int, FateCardRes> FateCardDataTable;

    // ===== Infinity Tower =====
    static std::unordered_map<int, InfinityTowerLevelRes> InfinityTowerLevelDataTable;
    static std::unordered_map<int, InfinityTowerDifficultyRes> InfinityTowerDifficultyDataTable;

    // ===== Vampire Survivor =====
    static std::unordered_map<int, VampireSurvivorRes> VampireSurvivorDataTable;
    static std::unordered_map<int, VampireTalentRes> VampireTalentDataTable;

    //// ===== Score Boss =====
    static std::unordered_map<int, ScoreBossControlRes> ScoreBossControlDataTable;
    static std::unordered_map<int, ScoreBossRewardRes> ScoreBossRewardDataTable;

    // ===== Misc =====
    static std::unordered_map<int, WorldClassRes> WorldClassDataTable;
    static std::unordered_map<int, GuideGroupRes> GuideGroupDataTable;
    static std::unordered_map<int, HandbookRes> HandbookDataTable;
    static std::unordered_map<std::pair<int, int>, SignInRes, ResourcePairHash> SignInDataTable;

    // ===== Activity =====
    static std::unordered_map<int, ActivityRes> ActivityDataTable;

    // Activity: Login Reward
    static std::unordered_map<int, LoginRewardGroupControlRes> LoginRewardGroupControlDataTable;

    // Activity: Tower Defense
    static std::unordered_map<int, TowerDefenseLevelRes> TowerDefenseLevelDataTable;

    // Activity: Trials
    static std::unordered_map<int, TrialControlRes> TrialControlDataTable;
    static std::unordered_map<int, TrialGroupRes> TrialGroupDataTable;

    // Activity: Joint Drill
    static std::unordered_map<int, JointDrill2LevelRes> JointDrill2LevelDataTable;

    // Activity: Levels
    static std::unordered_map<int, ActivityLevelsLevelRes> ActivityLevelsLevelDataTable;

    // Activity: Task
    static std::unordered_map<int, ActivityTaskRes> ActivityTaskDataTable;
    static std::unordered_map<int, ActivityTaskGroupRes> ActivityTaskGroupDataTable;

    // Activity: Shop
    static std::unordered_map<int, ActivityShopRes> ActivityShopDataTable;
    static std::unordered_map<int, ActivityShopControlRes> ActivityShopControlDataTable;
    static std::unordered_map<int, ActivityGoodsRes> ActivityGoodsDataTable;
};
