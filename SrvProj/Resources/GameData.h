#pragma once

#include <unordered_map>
#include <memory>
#include <string>

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
class DropPkgRes;

class GameData {
public:
    // 禁止实例化
    GameData() = delete;
    ~GameData() = delete;
    GameData(const GameData&) = delete;
    GameData& operator=(const GameData&) = delete;

    // Characters
    static std::unordered_map<std::string, CharacterRes> CharacterDataTable;
    static std::unordered_map<std::string, CharacterAdvanceRes> CharacterAdvanceDataTable;
    static std::unordered_map<std::string, CharacterSkillUpgradeRes> CharacterSkillUpgradeDataTable;
    static std::unordered_map<std::string, CharacterUpgradeRes> CharacterUpgradeDataTable;
    static std::unordered_map<std::string, CharItemExpRes> CharItemExpDataTable;
    static std::unordered_map<std::string, CharacterSkinRes> CharacterSkinDataTable;
    static std::unordered_map<std::string, TalentGroupRes> TalentGroupDataTable;
    static std::unordered_map<std::string, TalentRes> TalentDataTable;

    // Characters: Emblems
    static std::unordered_map<std::string, CharGemRes> CharGemDataTable;
    static std::unordered_map<std::string, CharGemSlotControlRes> CharGemSlotControlDataTable;

    static std::unordered_map<std::string, CharGemAttrValueRes> CharGemAttrValueDataTable;

    // Characters: Affinity
    static std::unordered_map<std::string, AffinityLevelRes> AffinityLevelDataTable;
    static std::unordered_map<std::string, AffinityGiftRes> AffinityGiftDataTable;
    static std::unordered_map<std::string, PlotRes> PlotDataTable;

    // Characters: Phone
    static std::unordered_map<std::string, ChatRes> ChatDataTable;

    // Characters: Dating
    static std::unordered_map<std::string, DatingLandmarkRes> DatingLandmarkDataTable;
    static std::unordered_map<std::string, DatingLandmarkEventRes> DatingLandmarkEventDataTable;
    static std::unordered_map<std::string, DatingCharacterEventRes> DatingCharacterEventDataTable;

    //// ===== Discs =====
    static std::unordered_map<std::string, DiscRes> DiscDataTable;
    static std::unordered_map<std::string, DiscStrengthenRes> DiscStrengthenDataTable;
    static std::unordered_map<std::string, DiscItemExpRes> DiscItemExpDataTable;
    static std::unordered_map<std::string, DiscPromoteRes> DiscPromoteDataTable;
    static std::unordered_map<std::string, DiscPromoteLimitRes> DiscPromoteLimitDataTable;

    // Discs: Melody items
    static std::unordered_map<std::string, SecondarySkillRes> SecondarySkillDataTable;

    // ===== Items =====
    static std::unordered_map<std::string, ItemRes> ItemDataTable;
    static std::unordered_map<std::string, ProductionRes> ProductionDataTable;
    static std::unordered_map<std::string, PlayerHeadRes> PlayerHeadDataTable;
    static std::unordered_map<std::string, TitleRes> TitleDataTable;
    static std::unordered_map<std::string, HonorRes> HonorDataTable;
    static std::unordered_map<std::string, DropPkgRes> DropPkgDataTable;
    // ===== Shops =====
    static std::unordered_map<std::string, MallMonthlyCardRes> MallMonthlyCardDataTable;
    static std::unordered_map<std::string, MallPackageRes> MallPackageDataTable;
    static std::unordered_map<std::string, MallShopRes> MallShopDataTable;
    static std::unordered_map<std::string, MallGemRes> MallGemDataTable;

    static std::unordered_map<std::string, ResidentShopRes> ResidentShopDataTable;
    static std::unordered_map<std::string, ResidentGoodsRes> ResidentGoodsDataTable;

    // ===== Battle Pass =====
    static std::unordered_map<std::string, BattlePassRes> BattlePassDataTable;
    static std::unordered_map<std::string, BattlePassLevelRes> BattlePassLevelDataTable;
    static std::unordered_map<std::string, BattlePassQuestRes> BattlePassQuestDataTable;
    static std::unordered_map<std::string, BattlePassRewardRes> BattlePassRewardDataTable;

    // ===== Commissions =====
    static std::unordered_map<std::string, AgentRes> AgentDataTable;

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
    static std::unordered_map<std::string, StoryRes> StoryDataTable;
    static std::unordered_map<std::string, StorySetSectionRes> StorySetSectionDataTable;
    static std::unordered_map<std::string, StoryEvidenceRes> StoryEvidenceDataTable;

    static std::unordered_map<std::string, MainScreenCGRes> MainScreenCGDataTable;

    // ===== Daily/Weekly Quests =====
    static std::unordered_map<std::string, DailyQuestRes> DailyQuestDataTable;
    static std::unordered_map<std::string, DailyQuestActiveRes> DailyQuestActiveDataTable;
    static std::unordered_map<std::string, WeeklyQuestRes> WeeklyQuestDataTable;
    static std::unordered_map<std::string, WeeklyQuestActiveRes> WeeklyQuestActiveDataTable;

    // ===== Achievements =====
    static std::unordered_map<std::string, AchievementRes> AchievementDataTable;

    // ===== Tutorials =====
    static std::unordered_map<std::string, TutorialLevelRes> TutorialLevelDataTable;

    //// ===== Instances =====
    //static std::unordered_map<std::string, DailyInstanceRes> mDailyInstanceDataTable;
    //static std::unordered_map<std::string, DailyInstanceRewardGroupRes> mDailyInstanceRewardGroupDataTable;
    //static std::unordered_map<std::string, RegionBossLevelRes> mRegionBossLevelDataTable;
    //static std::unordered_map<std::string, SkillInstanceRes> mSkillInstanceDataTable;
    //static std::unordered_map<std::string, CharGemInstanceRes> mCharGemInstanceDataTable;
    //static std::unordered_map<std::string, WeekBossLevelRes> mWeekBossLevelDataTable;

    // ===== Star Tower =====
    static std::unordered_map<std::string, StarTowerRes> StarTowerDataTable;
    static std::unordered_map<std::string, StarTowerStageRes> StarTowerStageDataTable;
    static std::unordered_map<std::string, StarTowerGrowthNodeRes> StarTowerGrowthNodeDataTable;
    static std::unordered_map<std::string, StarTowerFloorExpRes> StarTowerFloorExpDataTable;
    static std::unordered_map<std::string, StarTowerTeamExpRes> StarTowerTeamExpDataTable;
    static std::unordered_map<std::string, StarTowerEventRes> StarTowerEventDataTable;
    static std::unordered_map<std::string, StarTowerBuildRankRes> StarTowerBuildRankDataTable;
    static std::unordered_map<std::string, SubNoteSkillPromoteGroupRes> SubNoteSkillPromoteGroupDataTable;

    static std::unordered_map<std::string, PotentialRes> PotentialDataTable;
    static std::unordered_map<std::string, CharPotentialRes> CharPotentialDataTable;

    static std::unordered_map<std::string, StarTowerBookFateCardBundleRes> StarTowerBookFateCardBundleDataTable;
    static std::unordered_map<std::string, StarTowerBookFateCardQuestRes> StarTowerBookFateCardQuestDataTable;
    static std::unordered_map<std::string, StarTowerBookFateCardRes> StarTowerBookFateCardDataTable;
    static std::unordered_map<std::string, FateCardRes> FateCardDataTable;

    // ===== Infinity Tower =====
    static std::unordered_map<std::string, InfinityTowerLevelRes> InfinityTowerLevelDataTable;
    static std::unordered_map<std::string, InfinityTowerDifficultyRes> InfinityTowerDifficultyDataTable;

    //// ===== Vampire Survivor =====
    //static std::unordered_map<std::string, VampireSurvivorRes> mVampireSurvivorDataTable;
    //static std::unordered_map<std::string, VampireTalentRes> mVampireTalentDataTable;

    //// ===== Score Boss =====
    //static std::unordered_map<std::string, ScoreBossControlRes> mScoreBossControlDataTable;
    //static std::unordered_map<std::string, ScoreBossRewardRes> mScoreBossRewardDataTable;

    // ===== Misc =====
    static std::unordered_map<std::string, WorldClassRes> WorldClassDataTable;
    static std::unordered_map<std::string, GuideGroupRes> GuideGroupDataTable;
    static std::unordered_map<std::string, HandbookRes> HandbookDataTable;
    static std::unordered_map<std::string, SignInRes> SignInDataTable;

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
};
