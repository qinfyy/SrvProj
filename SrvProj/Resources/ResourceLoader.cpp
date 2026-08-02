#include "ResourceLoader.h"
#include "GameData.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <type_traits>
#include <utility>
#include <nlohmann/json.hpp>
#include <Archive.h>
#include "../Config.h"
#include "../Logger.h"
#include "../Util.h"

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
        catch (const std::exception&)
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

std::string GetJsonFileName(const std::string& typeName) {
    if (typeName == "LoginRewardGroupControlRes") {
        return "LoginRewardGroup.json";
    }
    if (typeName == "JointDrill2LevelRes") {
        return "JointDrill_2_Level.json";
    }

    return typeName.substr(0, typeName.length() - 3) + ".json";
}

struct ResourceLoadSource {
    bool useBytes = false;
    Archive* arc = nullptr;
    std::filesystem::path jsonBinPath;
};

std::filesystem::path ResolveJsonBinPath(const std::string& inputPath) {
    std::filesystem::path path(inputPath);
    auto binPath = path / "bin";
    if (std::filesystem::is_directory(binPath)) {
        return binPath;
    }

    return path;
}

template<typename T, typename Container>
void CheckResourceInterface() {
    static_assert(std::is_same_v<decltype(std::declval<T&>().LoadFromPb(std::declval<std::string>())), bool>,
        "资源类必须实现 bool LoadFromPb(std::string data)");
    static_assert(std::is_same_v<decltype(std::declval<T&>().LoadFromJson(std::declval<const nlohmann::json&>())), bool>,
        "资源类必须实现 bool LoadFromJson(const nlohmann::json& data)");
    static_assert(std::is_same_v<decltype(std::declval<const T&>().GetKey()), typename Container::key_type>,
        "资源类 GetKey 返回类型必须和资源表 key 类型一致");
}

template<typename T, typename Container>
void LoadBytesRes(Archive* arc, Container& container, const std::string& resName) {
    CheckResourceInterface<T, Container>();

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

    size_t loadedCount = 0;
    for (const auto& item : items) {
        try {
            T res;
            if (!res.LoadFromPb(item.data)) {
                throw std::runtime_error("从 protobuf 数据加载资源失败");
            }
            auto result = container.emplace(res.GetKey(), std::move(res));
            if (result.second)
            {
                result.first->second.OnLoad();
                ++loadedCount;
            }
        }
        catch (const std::exception& e) {
            //std::throw_with_nested(std::runtime_error("LoadRes: 解析 " + resName + "中 key = " + item.key + " 的记录失败"));
            LOG_ERROR("LoadRes: 解析 " + resName + "中 key = " + item.key + " 的记录失败, ERROR: " + std::string(e.what()));
        }
    }

    LOG_INFO("Loaded {} {}.", loadedCount, resName);
}

template<typename T, typename Container>
void LoadJsonRes(const std::filesystem::path& jsonBinPath, Container& container, const std::string& resName) {
    CheckResourceInterface<T, Container>();

    const auto jsonFileName = GetJsonFileName(resName);
    const auto jsonFilePath = jsonBinPath / jsonFileName;
    std::ifstream in(jsonFilePath);
    if (!in.is_open()) {
        throw std::runtime_error("无法读取 JSON 资源文件: " + jsonFilePath.string());
    }

    nlohmann::json root;
    in >> root;

    size_t loadedCount = 0;
    auto loadOne = [&](const std::string& key, const nlohmann::json& item) {
        try {
            T res;
            if (!item.is_object()) {
                throw std::runtime_error("JSON 记录不是对象");
            }
            if (!res.LoadFromJson(item)) {
                throw std::runtime_error("从 JSON 数据加载资源失败");
            }
            auto result = container.emplace(res.GetKey(), std::move(res));
            if (result.second)
            {
                result.first->second.OnLoad();
                ++loadedCount;
            }
        }
        catch (const std::exception& e) {
            LOG_ERROR("LoadJsonRes: 解析 " + resName + "中 key = " + key + " 的记录失败, FILE: " + jsonFilePath.string() + ", ERROR: " + std::string(e.what()));
        }
    };

    if (root.is_object()) {
        for (auto it = root.begin(); it != root.end(); ++it) {
            loadOne(it.key(), it.value());
        }
    }
    else if (root.is_array()) {
        for (size_t i = 0; i < root.size(); ++i) {
            loadOne(std::to_string(i + 1), root[i]);
        }
    }
    else {
        throw std::runtime_error("JSON 资源文件根节点必须是对象或数组: " + jsonFilePath.string());
    }

    LOG_INFO("Loaded {} {}.", loadedCount, resName);
}

template<typename T, typename Container>
void LoadRes(ResourceLoadSource* source, Container& container) {
    auto resName = GetTypeName<T>();
    if (source->useBytes) {
        LoadBytesRes<T>(source->arc, container, resName);
    }
    else {
        LoadJsonRes<T>(source->jsonBinPath, container, resName);
    }
}

void LoadResources() {
    LOG_INFO("Starting to load resources");

    ResourceLoadSource source;
    std::unique_ptr<Archive> arc;
    const auto resourceType = ToLower(Config::Get().resourceConfig.type);

    if (resourceType == "arcx") {
        source.useBytes = true;
        arc = Archive::Open(Config::Get().resourceConfig.path);
        source.arc = arc.get();
    }
    else if (resourceType == "json") {
        source.jsonBinPath = ResolveJsonBinPath(Config::Get().resourceConfig.path);
    }
    else {
        throw std::runtime_error("资源加载类型无效，仅支持 arcx 或 json: " + Config::Get().resourceConfig.type);
    }

    // HIGHEST

    // HIGH

    // ===== Gacha =====
    LoadRes<GachaATypeProbRes>(&source, GameData::GachaATypeProbDataTable);
    LoadRes<GachaStorageRes>(&source, GameData::GachaStorageDataTable);

    // NORMAL

    // Characters
    LoadRes<CharacterRes>(&source, GameData::CharacterDataTable);
    LoadRes<CharacterSkillUpgradeRes>(&source, GameData::CharacterSkillUpgradeDataTable);
    LoadRes<CharacterUpgradeRes>(&source, GameData::CharacterUpgradeDataTable);
    LoadRes<CharItemExpRes>(&source, GameData::CharItemExpDataTable);
    LoadRes<CharacterSkinRes>(&source, GameData::CharacterSkinDataTable);
    LoadRes<TalentGroupRes>(&source, GameData::TalentGroupDataTable);

    // Characters: Emblems
    LoadRes<CharGemRes>(&source, GameData::CharGemDataTable);
    LoadRes<CharGemSlotControlRes>(&source, GameData::CharGemSlotControlDataTable);

    // Characters: Affinity
    LoadRes<AffinityLevelRes>(&source, GameData::AffinityLevelDataTable);
    LoadRes<AffinityGiftRes>(&source, GameData::AffinityGiftDataTable);
    LoadRes<PlotRes>(&source, GameData::PlotDataTable);

    // Characters: Dating
    LoadRes<DatingLandmarkRes>(&source, GameData::DatingLandmarkDataTable);

    // ===== Discs =====
    LoadRes<DiscRes>(&source, GameData::DiscDataTable);
    LoadRes<DiscStrengthenRes>(&source, GameData::DiscStrengthenDataTable);
    LoadRes<DiscItemExpRes>(&source, GameData::DiscItemExpDataTable);
    LoadRes<DiscPromoteLimitRes>(&source, GameData::DiscPromoteLimitDataTable);

    // Discs: Melody items
    LoadRes<SecondarySkillRes>(&source, GameData::SecondarySkillDataTable);

    // ===== Items =====
    LoadRes<ItemRes>(&source, GameData::ItemDataTable);
    LoadRes<ProductionRes>(&source, GameData::ProductionDataTable);
    LoadRes<PlayerHeadRes>(&source, GameData::PlayerHeadDataTable);
    LoadRes<DropPkgRes>(&source, GameData::DropPkgDataTable);

    // ===== Shops =====
    LoadRes<MallMonthlyCardRes>(&source, GameData::MallMonthlyCardDataTable);
    LoadRes<MonthlyCardRes>(&source, GameData::MonthlyCardDataTable);
    LoadRes<MallPackageRes>(&source, GameData::MallPackageDataTable);
    LoadRes<MallShopRes>(&source, GameData::MallShopDataTable);
    LoadRes<MallGemRes>(&source, GameData::MallGemDataTable);

    LoadRes<ResidentShopRes>(&source, GameData::ResidentShopDataTable);
    LoadRes<ResidentGoodsRes>(&source, GameData::ResidentGoodsDataTable);

    // ===== Battle Pass =====
    LoadRes<BattlePassRes>(&source, GameData::BattlePassDataTable);
    LoadRes<BattlePassLevelRes>(&source, GameData::BattlePassLevelDataTable);
    LoadRes<BattlePassQuestRes>(&source, GameData::BattlePassQuestDataTable);
    LoadRes<BattlePassRewardRes>(&source, GameData::BattlePassRewardDataTable);

    // ===== Commissions =====
    LoadRes<AgentRes>(&source, GameData::AgentDataTable);

    // ===== Dictionary =====
    LoadRes<DictionaryTabRes>(&source, GameData::DictionaryTabDataTable);

    // ===== Gacha =====
    LoadRes<GachaNewbieRes>(&source, GameData::GachaNewbieDataTable);
    LoadRes<GachaTypeRes>(&source, GameData::GachaTypeDataTable);

    // ===== Story =====
    LoadRes<StoryRes>(&source, GameData::StoryDataTable);
    LoadRes<StorySetSectionRes>(&source, GameData::StorySetSectionDataTable);
    LoadRes<StoryEvidenceRes>(&source, GameData::StoryEvidenceDataTable);

    // ===== Daily/Weekly Quests =====
    LoadRes<DailyQuestRes>(&source, GameData::DailyQuestDataTable);
    LoadRes<DailyQuestActiveRes>(&source, GameData::DailyQuestActiveDataTable);
    LoadRes<WeeklyQuestRes>(&source, GameData::WeeklyQuestDataTable);
    LoadRes<WeeklyQuestActiveRes>(&source, GameData::WeeklyQuestActiveDataTable);

    // ===== Achievements =====
    LoadRes<AchievementRes>(&source, GameData::AchievementDataTable);

    // ===== Tutorials =====
    LoadRes<TutorialLevelRes>(&source, GameData::TutorialLevelDataTable);

    // ===== Instances =====
    LoadRes<DailyInstanceRes>(&source, GameData::DailyInstanceDataTable);
    LoadRes<DailyInstanceRewardGroupRes>(&source, GameData::DailyInstanceRewardGroupDataTable);
    LoadRes<RegionBossLevelRes>(&source, GameData::RegionBossLevelDataTable);
    LoadRes<SkillInstanceRes>(&source, GameData::SkillInstanceDataTable);
    LoadRes<CharGemInstanceRes>(&source, GameData::CharGemInstanceDataTable);

    // ===== Star Tower =====
    LoadRes<StarTowerRes>(&source, GameData::StarTowerDataTable);
    LoadRes<StarTowerStageRes>(&source, GameData::StarTowerStageDataTable);
    LoadRes<StarTowerGrowthNodeRes>(&source, GameData::StarTowerGrowthNodeDataTable);
    LoadRes<StarTowerFloorExpRes>(&source, GameData::StarTowerFloorExpDataTable);
    LoadRes<StarTowerTeamExpRes>(&source, GameData::StarTowerTeamExpDataTable);
    LoadRes<StarTowerEventRes>(&source, GameData::StarTowerEventDataTable);
    LoadRes<EventOptionsRes>(&source, GameData::EventOptionsDataTable);
    LoadRes<StarTowerBuildRankRes>(&source, GameData::StarTowerBuildRankDataTable);
    LoadRes<SubNoteSkillDropGroupRes>(&source, GameData::SubNoteSkillDropGroupDataTable);
    LoadRes<SubNoteSkillPromoteGroupRes>(&source, GameData::SubNoteSkillPromoteGroupDataTable);

    LoadRes<PotentialRes>(&source, GameData::PotentialDataTable);
    LoadRes<CharPotentialRes>(&source, GameData::CharPotentialDataTable);
    LoadRes<NPCAffinityGroupRes>(&source, GameData::NPCAffinityGroupDataTable);
    LoadRes<NPCAffinityPlotRes>(&source, GameData::NPCAffinityPlotDataTable);

    LoadRes<StarTowerBookFateCardBundleRes>(&source, GameData::StarTowerBookFateCardBundleDataTable);
    LoadRes<StarTowerBookFateCardQuestRes>(&source, GameData::StarTowerBookFateCardQuestDataTable);
    LoadRes<FateCardRes>(&source, GameData::FateCardDataTable);

    // ===== Infinity Tower =====
    LoadRes<InfinityTowerLevelRes>(&source, GameData::InfinityTowerLevelDataTable);
    LoadRes<InfinityTowerDifficultyRes>(&source, GameData::InfinityTowerDifficultyDataTable);

    // ===== Vampire Survivor =====
    LoadRes<VampireSurvivorRes>(&source, GameData::VampireSurvivorDataTable);
    LoadRes<VampireTalentRes>(&source, GameData::VampireTalentDataTable);

    // ===== Score Boss =====
    LoadRes<ScoreBossControlRes>(&source, GameData::ScoreBossControlDataTable);
    LoadRes<ScoreBossRewardRes>(&source, GameData::ScoreBossRewardDataTable);

    // ===== Misc =====
    LoadRes<WorldClassRes>(&source, GameData::WorldClassDataTable);
    LoadRes<GuideGroupRes>(&source, GameData::GuideGroupDataTable);
    LoadRes<HandbookRes>(&source, GameData::HandbookDataTable);
    LoadRes<SignInRes>(&source, GameData::SignInDataTable);

    // ===== Activity =====
    LoadRes<ActivityRes>(&source, GameData::ActivityDataTable);

    // Activity: Login Reward
    LoadRes<LoginRewardGroupControlRes>(&source, GameData::LoginRewardGroupControlDataTable);

    // Activity: Tower Defense
    LoadRes<TowerDefenseLevelRes>(&source, GameData::TowerDefenseLevelDataTable);

    // Activity: Trials
    LoadRes<TrialControlRes>(&source, GameData::TrialControlDataTable);
    LoadRes<TrialGroupRes>(&source, GameData::TrialGroupDataTable);

    // Activity: Joint Drill
    LoadRes<JointDrill2LevelRes>(&source, GameData::JointDrill2LevelDataTable);

    // Activity: Levels
    LoadRes<ActivityLevelsLevelRes>(&source, GameData::ActivityLevelsLevelDataTable);

    // Activity: Task
    LoadRes<ActivityTaskRes>(&source, GameData::ActivityTaskDataTable);
    LoadRes<ActivityTaskGroupRes>(&source, GameData::ActivityTaskGroupDataTable);

    // Activity: Shop
    LoadRes<ActivityShopRes>(&source, GameData::ActivityShopDataTable);
    LoadRes<ActivityShopControlRes>(&source, GameData::ActivityShopControlDataTable);

    // LOW

    // Characters
    LoadRes<CharacterDesRes>(&source, GameData::CharacterDesDataTable);
    LoadRes<CharacterAdvanceRes>(&source, GameData::CharacterAdvanceDataTable);
    LoadRes<TalentRes>(&source, GameData::TalentDataTable);

    // Characters: Emblems
    LoadRes<CharGemAttrValueRes>(&source, GameData::CharGemAttrValueDataTable);

    // Characters: Phone
    LoadRes<ChatRes>(&source, GameData::ChatDataTable);

    // Characters: Dating
    LoadRes<DatingLandmarkEventRes>(&source, GameData::DatingLandmarkEventDataTable);
    LoadRes<DatingCharacterEventRes>(&source, GameData::DatingCharacterEventDataTable);

    // ===== Discs =====
    LoadRes<DiscPromoteRes>(&source, GameData::DiscPromoteDataTable);

    // ===== Items =====
    LoadRes<TitleRes>(&source, GameData::TitleDataTable);
    LoadRes<HonorRes>(&source, GameData::HonorDataTable);

    // ===== Dictionary =====
    LoadRes<DictionaryEntryRes>(&source, GameData::DictionaryEntryDataTable);

    // ===== Story =====
    LoadRes<MainScreenCGRes>(&source, GameData::MainScreenCGDataTable);

    // ===== Instances =====
    LoadRes<WeekBossLevelRes>(&source, GameData::WeekBossLevelDataTable);

    // ===== Star Tower =====
    LoadRes<StarTowerBookFateCardRes>(&source, GameData::StarTowerBookFateCardDataTable);

    // Activity: Shop
    LoadRes<ActivityGoodsRes>(&source, GameData::ActivityGoodsDataTable);

    // LOWEST

    // ===== Gacha =====
    GachaPkgRes::ClearPackages();
    LoadRes<GachaPkgRes>(&source, GameData::GachaPkgDataTable);
    LoadRes<GachaRes>(&source, GameData::GachaDataTable);

    LOG_INFO("Resource loading complete.");
}
