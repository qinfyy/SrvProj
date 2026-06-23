#pragma once

#include "../ResBase.h"
#include "../ResourceDerivedData.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

class StarTowerRes : public ResBase {
public:
    StarTowerRes() = default;
    ~StarTowerRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

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

class StarTowerStageRes : public ResBase {
public:
    StarTowerStageRes() = default;
    ~StarTowerStageRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

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

class StarTowerGrowthNodeRes : public ResBase {
public:
    StarTowerGrowthNodeRes() = default;
    ~StarTowerGrowthNodeRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int NodeId;
    int Group;
    int ItemId1;
    int ItemQty1;

    // 非序列化字段
};

class StarTowerFloorExpRes : public ResBase {
public:
    StarTowerFloorExpRes() = default;
    ~StarTowerFloorExpRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

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

class StarTowerTeamExpRes : public ResBase {
public:
    StarTowerTeamExpRes() = default;
    ~StarTowerTeamExpRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int GroupId;
    int Level;
    int NeedExp;

    // 非序列化字段
};

class StarTowerEventRes : public ResBase {
public:
    StarTowerEventRes() = default;
    ~StarTowerEventRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

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

class EventOptionsRes : public ResBase {
public:
    EventOptionsRes() = default;
    ~EventOptionsRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    int Id;
    std::string Desc;
    bool IgnoreInterActive = false;
};

class StarTowerBuildRankRes : public ResBase {
public:
    StarTowerBuildRankRes() = default;
    ~StarTowerBuildRankRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int MinGrade;
    int Rarity;

    // 非序列化字段
};

class SubNoteSkillPromoteGroupRes : public ResBase {
public:
    SubNoteSkillPromoteGroupRes() = default;
    ~SubNoteSkillPromoteGroupRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    std::string SubNoteSkills;

    // 非序列化字段
    ItemParamMap Items;
};

class SubNoteSkillDropGroupRes : public ResBase {
public:
    SubNoteSkillDropGroupRes() = default;
    ~SubNoteSkillDropGroupRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    int Id;
    int GroupId;
    int SubNoteSkillId;

    static int GetRandomDrop(int groupId);

private:
    static std::unordered_map<int, std::vector<int>> Groups;
};
class PotentialRes : public ResBase {
public:
    PotentialRes() = default;
    ~PotentialRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

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

class CharPotentialRes : public ResBase {
public:
    CharPotentialRes() = default;
    ~CharPotentialRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

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

class StarTowerBookFateCardBundleRes : public ResBase {
public:
    StarTowerBookFateCardBundleRes() = default;
    ~StarTowerBookFateCardBundleRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;

    // 非序列化字段
    std::vector<int> CardIds;
};
class StarTowerBookFateCardQuestRes : public ResBase {
public:
    StarTowerBookFateCardQuestRes() = default;
    ~StarTowerBookFateCardQuestRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;

    // 非序列化字段
};

class StarTowerBookFateCardRes : public ResBase {
public:
    StarTowerBookFateCardRes() = default;
    ~StarTowerBookFateCardRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int BundleId;

    // 非序列化字段
};

class FateCardRes : public ResBase {
public:
    FateCardRes() = default;
    ~FateCardRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    bool IsTower;
    bool IsVampire;
    bool IsVampireSpecial;
    bool Removable;

    // 非序列化字段
    int BundleId = 0;
};

class NPCAffinityGroupRes : public ResBase {
public:
    NPCAffinityGroupRes() = default;
    ~NPCAffinityGroupRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

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

class NPCAffinityPlotRes : public ResBase {
public:
    NPCAffinityPlotRes() = default;
    ~NPCAffinityPlotRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

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

class InfinityTowerLevelRes : public ResBase {
public:
    InfinityTowerLevelRes() = default;
    ~InfinityTowerLevelRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int DifficultyId;
    std::string BaseAwardPreview;

    // 非序列化字段
};

class InfinityTowerDifficultyRes : public ResBase {
public:
    InfinityTowerDifficultyRes() = default;
    ~InfinityTowerDifficultyRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段
    int Id;
    int TowerId;

    // 非序列化字段
};
