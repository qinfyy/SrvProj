#include "ResourceLoader.h"
#include "GameData.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <Archive.h>
#include "ResBase.h"
#include "../Logger.h"

#include "BinClass\AchievementsRes.h"
#include "BinClass\ActivityRes.h"
#include "BinClass\BattlePass.h"
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

template<typename T>
static bool Read(std::istream& inputStream, T& out) {
    static_assert(std::is_arithmetic_v<T>, "Read 仅支持基本类型");

    inputStream.read(reinterpret_cast<char*>(&out), sizeof(T));
    return inputStream.gcount() == sizeof(T);
}

static bool ReadBytes(std::istream& inputStream, std::string& out, size_t len) {
    out.resize(len);
    inputStream.read(out.data(), len);
    return inputStream.gcount() == len;
}

template<typename T>
static bool ReadBigEndian(std::istream& inputStream, T& outValue) {
    static_assert(std::is_integral_v<T>, "ReadBigEndian 仅支持整数类型");

    std::string buffer;
    if (!ReadBytes(inputStream, buffer, sizeof(T))) {
        return false;
    }

    outValue = 0;
    const unsigned char* data = reinterpret_cast<const unsigned char*>(buffer.data());

    for (size_t i = 0; i < sizeof(T); ++i) {
        outValue |= static_cast<T>(data[i]) << ((sizeof(T) - 1 - i) * 8);
    }

    return true;
}

void ParseBytesFile(std::istream& inputStream, BytesFileHeader& outHeader, std::vector<GeneralItem>& outItems) {
    if (!inputStream) {
        throw std::runtime_error("输入流无效");
    }

    if (!ReadBigEndian(inputStream, outHeader.magic)) {
        throw std::runtime_error("读取 magic 失败");
    }

    if (outHeader.magic != 0x54930300) {
        throw std::runtime_error("magic 不匹配，文件格式错误");
    }

    if (!Read(inputStream, outHeader.versionLen)) {
        throw std::runtime_error("读取 versionLen 失败");
    }

    if (!ReadBytes(inputStream, outHeader.versionText, outHeader.versionLen)) {
        throw std::runtime_error("读取 versionText 失败");
    }

    if (!Read(inputStream, outHeader.mode1)) {
        throw std::runtime_error("读取 mode1 失败");
    }

    if (!Read(inputStream, outHeader.mode2)) {
        throw std::runtime_error("读取 mode2 失败");
    }

    if (!Read(inputStream, outHeader.count)) {
        throw std::runtime_error("读取 count 失败");
    }

    outItems.clear();
    outItems.reserve(outHeader.count);

    for (uint32_t i = 0; i < outHeader.count; i++) {
        GeneralItem item{};

        try
        {
            // map
            if (outHeader.mode1 == 1) {
                if (outHeader.mode2 == 1) {
                    // int32 key
                    int32_t key;
                    if (!Read(inputStream, key)) {
                        throw std::runtime_error("读取 int32 key 失败");
                    }

                    item.key = std::to_string(key);

                    uint16_t len;
                    if (!Read(inputStream, len)) {
                        throw std::runtime_error("读取 value len 失败");
                    }

                    item.len = len;

                    if (!ReadBytes(inputStream, item.data, len)) {
                        throw std::runtime_error("读取 value 数据失败");
                    }
                }
                else if (outHeader.mode2 == 2) {
                    // int64 key
                    int64_t key;
                    if (!Read(inputStream, key)) {
                        throw std::runtime_error("读取 int64 key 失败");
                    }

                    item.key = std::to_string(key);

                    uint16_t len;
                    if (!Read(inputStream, len)) {
                        throw std::runtime_error("读取 value len 失败");
                    }

                    item.len = len;
                    if (!ReadBytes(inputStream, item.data, len)) {
                        throw std::runtime_error("读取 value 数据失败");
                    }
                }
                else {
                    // string key
                    uint16_t klen;
                    if (!Read(inputStream, klen)) {
                        throw std::runtime_error("读取 string key len 失败");
                    }

                    std::string key;
                    if (!ReadBytes(inputStream, key, klen)) {
                        throw std::runtime_error("读取 string key 失败");
                    }

                    item.key = key;

                    uint16_t vlen;
                    if (!Read(inputStream, vlen)) {
                        throw std::runtime_error("读取 value len 失败");
                    }

                    item.len = vlen;
                    if (!ReadBytes(inputStream, item.data, vlen)) {
                        throw std::runtime_error("读取 value 数据失败");
                    }
                }
            }
            else { // list
                item.key = std::to_string(i + 1);

                uint16_t len;
                if (!Read(inputStream, len)) {
                    throw std::runtime_error("读取 list len 失败");
                }

                item.len = len;
                if (!ReadBytes(inputStream, item.data, len)) {
                    throw std::runtime_error("读取 list 数据失败");
                }
            }
        }
        catch (const std::exception& e)
        {
            std::throw_with_nested(std::runtime_error("ParseBytesFile: 第 " + std::to_string(i) + " 条记录解析失败"));
        }

        outItems.emplace_back(std::move(item));
    }
}

std::string GetBytesFileNameName(const std::string& typeName) {
    if (typeName == "LoginRewardGroupControlRes") {
        return "bin/LoginRewardGroup.bytes";
    }
    if (typeName == "JointDrill2LevelRes") {
        return "bin/JointDrill_2_Level.bytes";
    }

    auto fileName = "bin/" + typeName.substr(0, typeName.length() - 3) + ".bytes";
    return fileName;
}

template<typename T, typename Container>
void LoadRes(Archive* arc, Container& container) {
    auto resName = GetTypeName<T>();
    BytesFileHeader header;
    std::vector<GeneralItem> items;
    auto bytesFileName = GetBytesFileNameName(resName);
    auto inFile = arc->ReadFile(bytesFileName);
    if (inFile.empty()) {
        throw std::runtime_error("无法读取文件: " + bytesFileName);
    }

    std::stringstream inStream;
    inStream.write(reinterpret_cast<const char*>(inFile.data()), inFile.size());
    ParseBytesFile(inStream, header, items);

    for (const auto& item : items) {
        try {
            T res;
            if (!res.LoadFromPb(item.data)) {
                throw std::runtime_error("从 protobuf 数据加载资源失败");
            }
            res.OnLoad();
            container.emplace(res.GetId(), std::move(res));
        }
        catch (const std::exception& e) {
            //std::throw_with_nested(std::runtime_error("LoadRes: 解析 " + resName + "中 key = " + item.key + " 的记录失败"));
            LOG_ERROR("LoadRes: 解析 " + resName + "中 key = " + item.key + " 的记录失败, ERROR: " + std::string(e.what()));
        }
    }
}

void LoadResources() {
    auto inputFilePath = "./data.arcx";
    auto arc = Archive::Open(inputFilePath);

    LoadRes<CharacterRes>(arc.get(), GameData::CharacterDataTable);
    LoadRes<CharacterAdvanceRes>(arc.get(), GameData::CharacterAdvanceDataTable);
    LoadRes<CharacterSkillUpgradeRes>(arc.get(), GameData::CharacterSkillUpgradeDataTable);
    LoadRes<CharacterUpgradeRes>(arc.get(), GameData::CharacterUpgradeDataTable);
    LoadRes<CharItemExpRes>(arc.get(), GameData::CharItemExpDataTable);
    LoadRes<CharacterSkinRes>(arc.get(), GameData::CharacterSkinDataTable);
    LoadRes<TalentGroupRes>(arc.get(), GameData::TalentGroupDataTable);
    LoadRes<TalentRes>(arc.get(), GameData::TalentDataTable);

    // Characters: Emblems
    LoadRes<CharGemRes>(arc.get(), GameData::CharGemDataTable);
    LoadRes<CharGemSlotControlRes>(arc.get(), GameData::CharGemSlotControlDataTable);
    LoadRes<CharGemAttrValueRes>(arc.get(), GameData::CharGemAttrValueDataTable);

    // Characters: Affinity
    LoadRes<AffinityLevelRes>(arc.get(), GameData::AffinityLevelDataTable);
    LoadRes<AffinityGiftRes>(arc.get(), GameData::AffinityGiftDataTable);
    LoadRes<PlotRes>(arc.get(), GameData::PlotDataTable);

    // Characters: Phone
    LoadRes<ChatRes>(arc.get(), GameData::ChatDataTable);

    // Characters: Dating
    LoadRes<DatingLandmarkRes>(arc.get(), GameData::DatingLandmarkDataTable);
    LoadRes<DatingLandmarkEventRes>(arc.get(), GameData::DatingLandmarkEventDataTable);
    LoadRes<DatingCharacterEventRes>(arc.get(), GameData::DatingCharacterEventDataTable);

    //// ===== Discs =====
    LoadRes<DiscRes>(arc.get(), GameData::DiscDataTable);
    LoadRes<DiscStrengthenRes>(arc.get(), GameData::DiscStrengthenDataTable);
    LoadRes<DiscItemExpRes>(arc.get(), GameData::DiscItemExpDataTable);
    LoadRes<DiscPromoteRes>(arc.get(), GameData::DiscPromoteDataTable);
    LoadRes<DiscPromoteLimitRes>(arc.get(), GameData::DiscPromoteLimitDataTable);

    // Discs: Melody items
    LoadRes<SecondarySkillRes>(arc.get(), GameData::SecondarySkillDataTable);

    // ===== Items =====
    LoadRes<ItemRes>(arc.get(), GameData::ItemDataTable);
    LoadRes<ProductionRes>(arc.get(), GameData::ProductionDataTable);
    LoadRes<PlayerHeadRes>(arc.get(), GameData::PlayerHeadDataTable);
    LoadRes<TitleRes>(arc.get(), GameData::TitleDataTable);
    LoadRes<HonorRes>(arc.get(), GameData::HonorDataTable);
    LoadRes<DropPkgRes>(arc.get(), GameData::DropPkgDataTable);

    // ===== Shops =====
    LoadRes<MallMonthlyCardRes>(arc.get(), GameData::MallMonthlyCardDataTable);
    LoadRes<MallPackageRes>(arc.get(), GameData::MallPackageDataTable);
    LoadRes<MallShopRes>(arc.get(), GameData::MallShopDataTable);
    LoadRes<MallGemRes>(arc.get(), GameData::MallGemDataTable);

    LoadRes<ResidentShopRes>(arc.get(), GameData::ResidentShopDataTable);
    LoadRes<ResidentGoodsRes>(arc.get(), GameData::ResidentGoodsDataTable);


    // ===== Battle Pass =====
    LoadRes<BattlePassRes>(arc.get(), GameData::BattlePassDataTable);
    LoadRes<BattlePassLevelRes>(arc.get(), GameData::BattlePassLevelDataTable);
    LoadRes<BattlePassQuestRes>(arc.get(), GameData::BattlePassQuestDataTable);
    LoadRes<BattlePassRewardRes>(arc.get(), GameData::BattlePassRewardDataTable);

    // ===== Commissions =====
    LoadRes<AgentRes>(arc.get(), GameData::AgentDataTable);

    // ===== Dictionary =====
    LoadRes<DictionaryTabRes>(arc.get(), GameData::DictionaryTabDataTable);
    LoadRes<DictionaryEntryRes>(arc.get(), GameData::DictionaryEntryDataTable);

    // ===== Gacha =====
    LoadRes<GachaATypeProbRes>(arc.get(), GameData::GachaATypeProbDataTable);
    LoadRes<GachaRes>(arc.get(), GameData::GachaDataTable);
    LoadRes<GachaNewbieRes>(arc.get(), GameData::GachaNewbieDataTable);
    LoadRes<GachaStorageRes>(arc.get(), GameData::GachaStorageDataTable);
    LoadRes<GachaTypeRes>(arc.get(), GameData::GachaTypeDataTable);

    // ===== Story =====
    LoadRes<StoryRes>(arc.get(), GameData::StoryDataTable);
    LoadRes<StorySetSectionRes>(arc.get(), GameData::StorySetSectionDataTable);
    LoadRes<StoryEvidenceRes>(arc.get(), GameData::StoryEvidenceDataTable);

    LoadRes<MainScreenCGRes>(arc.get(), GameData::MainScreenCGDataTable);

    // ===== Daily/Weekly Quests =====
    LoadRes<DailyQuestRes>(arc.get(), GameData::DailyQuestDataTable);
    LoadRes<DailyQuestActiveRes>(arc.get(), GameData::DailyQuestActiveDataTable);
    LoadRes<WeeklyQuestRes>(arc.get(), GameData::WeeklyQuestDataTable);
    LoadRes<WeeklyQuestActiveRes>(arc.get(), GameData::WeeklyQuestActiveDataTable);

    // ===== Achievements =====
    LoadRes<AchievementRes>(arc.get(), GameData::AchievementDataTable);

    // ===== Tutorials =====
    LoadRes<TutorialLevelRes>(arc.get(), GameData::TutorialLevelDataTable);

    // ===== Instances =====
    LoadRes<DailyInstanceRes>(arc.get(), GameData::DailyInstanceDataTable);
    LoadRes<DailyInstanceRewardGroupRes>(arc.get(), GameData::DailyInstanceRewardGroupDataTable);
    LoadRes<RegionBossLevelRes>(arc.get(), GameData::RegionBossLevelDataTable);
    LoadRes<SkillInstanceRes>(arc.get(), GameData::SkillInstanceDataTable);
    LoadRes<CharGemInstanceRes>(arc.get(), GameData::CharGemInstanceDataTable);
    LoadRes<WeekBossLevelRes>(arc.get(), GameData::WeekBossLevelDataTable);

    // ===== Star Tower =====
    LoadRes<StarTowerRes>(arc.get(), GameData::StarTowerDataTable);
    LoadRes<StarTowerStageRes>(arc.get(), GameData::StarTowerStageDataTable);
    LoadRes<StarTowerGrowthNodeRes>(arc.get(), GameData::StarTowerGrowthNodeDataTable);
    LoadRes<StarTowerFloorExpRes>(arc.get(), GameData::StarTowerFloorExpDataTable);
    LoadRes<StarTowerTeamExpRes>(arc.get(), GameData::StarTowerTeamExpDataTable);
    LoadRes<StarTowerEventRes>(arc.get(), GameData::StarTowerEventDataTable);
    LoadRes<StarTowerBuildRankRes>(arc.get(), GameData::StarTowerBuildRankDataTable);
    LoadRes<SubNoteSkillPromoteGroupRes>(arc.get(), GameData::SubNoteSkillPromoteGroupDataTable);

    LoadRes<PotentialRes>(arc.get(), GameData::PotentialDataTable);
    LoadRes<CharPotentialRes>(arc.get(), GameData::CharPotentialDataTable);

    LoadRes<StarTowerBookFateCardBundleRes>(arc.get(), GameData::StarTowerBookFateCardBundleDataTable);
    LoadRes<StarTowerBookFateCardQuestRes>(arc.get(), GameData::StarTowerBookFateCardQuestDataTable);
    LoadRes<StarTowerBookFateCardRes>(arc.get(), GameData::StarTowerBookFateCardDataTable);
    LoadRes<FateCardRes>(arc.get(), GameData::FateCardDataTable);

    // ===== Infinity Tower =====
    LoadRes<InfinityTowerLevelRes>(arc.get(), GameData::InfinityTowerLevelDataTable);
    LoadRes<InfinityTowerDifficultyRes>(arc.get(), GameData::InfinityTowerDifficultyDataTable);

    // ===== Vampire Survivor =====
    LoadRes<VampireSurvivorRes>(arc.get(), GameData::VampireSurvivorDataTable);
    LoadRes<VampireTalentRes>(arc.get(), GameData::VampireTalentDataTable);

    // ===== Score Boss =====
    LoadRes<ScoreBossControlRes>(arc.get(), GameData::ScoreBossControlDataTable);
    LoadRes<ScoreBossRewardRes>(arc.get(), GameData::ScoreBossRewardDataTable);


    // ===== Misc =====
    LoadRes<WorldClassRes>(arc.get(), GameData::WorldClassDataTable);
    LoadRes<GuideGroupRes>(arc.get(), GameData::GuideGroupDataTable);
    LoadRes<HandbookRes>(arc.get(), GameData::HandbookDataTable);
    LoadRes<SignInRes>(arc.get(), GameData::SignInDataTable);

    // ===== Activity =====
    LoadRes<ActivityRes>(arc.get(), GameData::ActivityDataTable);

    // Activity: Login Reward
    LoadRes<LoginRewardGroupControlRes>(arc.get(), GameData::LoginRewardGroupControlDataTable);

    // Activity: Tower Defense
    LoadRes<TowerDefenseLevelRes>(arc.get(), GameData::TowerDefenseLevelDataTable);

    // Activity: Trials
    LoadRes<TrialControlRes>(arc.get(), GameData::TrialControlDataTable);
    LoadRes<TrialGroupRes>(arc.get(), GameData::TrialGroupDataTable);

    // Activity: Joint Drill
    LoadRes<JointDrill2LevelRes>(arc.get(), GameData::JointDrill2LevelDataTable);

    // Activity: Levels
    LoadRes<ActivityLevelsLevelRes>(arc.get(), GameData::ActivityLevelsLevelDataTable);

    // Activity: Task
    LoadRes<ActivityTaskRes>(arc.get(), GameData::ActivityTaskDataTable);
    LoadRes<ActivityTaskGroupRes>(arc.get(), GameData::ActivityTaskGroupDataTable);

    // Activity: Shop
    LoadRes<ActivityShopRes>(arc.get(), GameData::ActivityShopDataTable);
    LoadRes<ActivityShopControlRes>(arc.get(), GameData::ActivityShopControlDataTable);
    LoadRes<ActivityGoodsRes>(arc.get(), GameData::ActivityGoodsDataTable);

}
