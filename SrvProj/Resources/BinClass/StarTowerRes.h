#pragma once

#include <nlohmann/json_fwd.hpp>
#include "../ResourceDerivedData.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

class StarTowerRes {
public:
    StarTowerRes() = default;
    ~StarTowerRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int GroupId;
    int Difficulty;
    int SubNoteSkillDropGroupId;
    std::vector<int> FloorNum;

    // 非序列化字段
    int MaxFloors = 0;

    int GetMaxFloor(int stageNum) const;
};

class StarTowerStageRes {
public:
    StarTowerStageRes() = default;
    ~StarTowerStageRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int Stage;
    int GroupId;
    int Floor;
    int InteriorCurrencyQuantity;
    int RoomType;
    int GuaranteedMapId;
    int GuaranteedMonsterPlanId;

    // 非序列化字段
};

class StarTowerGrowthNodeRes {
public:
    StarTowerGrowthNodeRes() = default;
    ~StarTowerGrowthNodeRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int NodeId;
    int Group;
    int ItemId1;
    int ItemQty1;

    // 非序列化字段
};

class StarTowerFloorExpRes {
public:
    StarTowerFloorExpRes() = default;
    ~StarTowerFloorExpRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int StarTowerId;
    int Stage;
    int NormalExp;
    int EliteExp;
    int BossExp;
    int FinalBossExp;

    // 非序列化字段
};

class StarTowerTeamExpRes {
public:
    StarTowerTeamExpRes() = default;
    ~StarTowerTeamExpRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int GroupId;
    int Level;
    int NeedExp;

    // 非序列化字段
};

class StarTowerEventRes {
public:
    StarTowerEventRes() = default;
    ~StarTowerEventRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int OptionsRulesId;
    int EventType;
    int GuaranteedMapId;
    std::vector<int> RelatedNPCs;
    int EventResType;

    // 非序列化字段
    std::vector<int> OptionIds;
};

class EventOptionsRes {
public:
    EventOptionsRes() = default;
    ~EventOptionsRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    int Id;
    std::string Desc;
    bool IgnoreInterActive = false;
};

class StarTowerBuildRankRes {
public:
    StarTowerBuildRankRes() = default;
    ~StarTowerBuildRankRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int MinGrade;
    int Rarity;

    // 非序列化字段
};

class SubNoteSkillPromoteGroupRes {
public:
    SubNoteSkillPromoteGroupRes() = default;
    ~SubNoteSkillPromoteGroupRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    std::string SubNoteSkills;

    // 非序列化字段
    ItemParamMap Items;
};

class SubNoteSkillDropGroupRes {
public:
    SubNoteSkillDropGroupRes() = default;
    ~SubNoteSkillDropGroupRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    int Id;
    int GroupId;
    int SubNoteSkillId;

    static int GetRandomDrop(int groupId);

private:
    static std::unordered_map<int, std::vector<int>> Groups;
};
class PotentialRes {
public:
    PotentialRes() = default;
    ~PotentialRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int CharId;
    int Build;
    int BranchType;
    int MaxLevel;
    std::vector<int> BuildScore;
    std::string BriefDesc;

    // 非序列化字段
    bool IsSpecial() const;
    int GetMaxLevel() const;
    int GetMaxLevel(int extraLevels) const;
    int GetBuildScore(int level) const;
};

class CharPotentialRes {
public:
    CharPotentialRes() = default;
    ~CharPotentialRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    std::vector<int> MasterSpecificPotentialIds;
    std::vector<int> AssistSpecificPotentialIds;
    std::vector<int> CommonPotentialIds;
    std::vector<int> MasterNormalPotentialIds;
    std::vector<int> AssistNormalPotentialIds;

    // 非序列化字段
    std::vector<int> GetPotentialList(bool main, bool special) const;
};

class StarTowerBookFateCardBundleRes {
public:
    StarTowerBookFateCardBundleRes() = default;
    ~StarTowerBookFateCardBundleRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;

    // 非序列化字段
    std::vector<int> CardIds;
};
class StarTowerBookFateCardQuestRes {
public:
    StarTowerBookFateCardQuestRes() = default;
    ~StarTowerBookFateCardQuestRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;

    // 非序列化字段
};

class StarTowerBookFateCardRes {
public:
    StarTowerBookFateCardRes() = default;
    ~StarTowerBookFateCardRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int BundleId;

    // 非序列化字段
};

class FateCardRes {
public:
    FateCardRes() = default;
    ~FateCardRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    bool IsTower;
    bool IsVampire;
    bool IsVampireSpecial;
    bool Removable;

    // 非序列化字段
    int BundleId = 0;
};

class NPCAffinityGroupRes {
public:
    NPCAffinityGroupRes() = default;
    ~NPCAffinityGroupRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int Level;
    int AffinityValue;
    int AffinityGroupId;
    std::string RelationshipName;
    std::string Icon;
    int AffinityLevelStage;
    std::string Reward;


    // 非序列化字段

};

class NPCAffinityPlotRes {
public:
    NPCAffinityPlotRes() = default;
    ~NPCAffinityPlotRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    std::string Name;
    std::string Desc;
    std::string PlotSum;
    std::string AvgId;
    int NPCId;
    int AffinityLevel;
    int ItemId;
    int ItemQty;

    // 非序列化字段
};

class InfinityTowerLevelRes {
public:
    InfinityTowerLevelRes() = default;
    ~InfinityTowerLevelRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int DifficultyId;
    std::string BaseAwardPreview;

    // 非序列化字段
};

class InfinityTowerDifficultyRes {
public:
    InfinityTowerDifficultyRes() = default;
    ~InfinityTowerDifficultyRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段
    int Id;
    int TowerId;

    // 非序列化字段
};
