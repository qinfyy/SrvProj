#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

class StarTowerRes : public ResBase {
public:
    StarTowerRes() = default;
    ~StarTowerRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int GroupId;
    int Difficulty;
    int SubNoteSkillDropGroupId;
    std::vector<int> FloorNum;
};


class StarTowerStageRes : public ResBase {
public:
    StarTowerStageRes() = default;
    ~StarTowerStageRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;


    int Id;
    int Stage;
    int Floor;
    int InteriorCurrencyQuantity;
    int RoomType;
};

class StarTowerGrowthNodeRes : public ResBase {
public:
    StarTowerGrowthNodeRes() = default;
    ~StarTowerGrowthNodeRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int NodeId;
    int Group;

    int ItemId1;
    int ItemQty1;

};

class StarTowerFloorExpRes : public ResBase {
public:
    StarTowerFloorExpRes() = default;
    ~StarTowerFloorExpRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int StarTowerId;
    int Stage;
    int NormalExp;
    int EliteExp;
    int BossExp;
    int FinalBossExp;
};

class StarTowerTeamExpRes : public ResBase {
public:
    StarTowerTeamExpRes() = default;
    ~StarTowerTeamExpRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int GroupId;
    int Level;
    int NeedExp;
};

class StarTowerEventRes : public ResBase {
public:
    StarTowerEventRes() = default;
    ~StarTowerEventRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    std::vector<int> RelatedNPCs;
};

class StarTowerBuildRankRes : public ResBase {
public:
    StarTowerBuildRankRes() = default;
    ~StarTowerBuildRankRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int MinGrade;
    int Rarity;
};

class SubNoteSkillPromoteGroupRes : public ResBase {
public:
    SubNoteSkillPromoteGroupRes() = default;
    ~SubNoteSkillPromoteGroupRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    std::string SubNoteSkills;

};

class PotentialRes : public ResBase {
public:
    PotentialRes() = default;
    ~PotentialRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int CharId;
    int Build;
    int BranchType;
    int MaxLevel;
    std::vector<int> BuildScore;
};

class CharPotentialRes : public ResBase {
public:
    CharPotentialRes() = default;
    ~CharPotentialRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;

    std::vector<int> MasterSpecificPotentialIds;
    std::vector<int> AssistSpecificPotentialIds;
    std::vector<int> CommonPotentialIds;
    std::vector<int> MasterNormalPotentialIds;
    std::vector<int> AssistNormalPotentialIds;
};

class StarTowerBookFateCardBundleRes : public ResBase {
public:
    StarTowerBookFateCardBundleRes() = default;
    ~StarTowerBookFateCardBundleRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    //int BundleId;

};

class StarTowerBookFateCardQuestRes : public ResBase {
public:
    StarTowerBookFateCardQuestRes() = default;
    ~StarTowerBookFateCardQuestRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
};

class StarTowerBookFateCardRes : public ResBase {
public:
    StarTowerBookFateCardRes() = default;
    ~StarTowerBookFateCardRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int BundleId;
};

class FateCardRes : public ResBase {
public:
    FateCardRes() = default;
    ~FateCardRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;

    bool IsTower;
    bool IsVampire;
    bool IsVampireSpecial;
    bool Removable;
};

class InfinityTowerLevelRes : public ResBase {
public:
    InfinityTowerLevelRes() = default;
    ~InfinityTowerLevelRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int DifficultyId;
    std::string BaseAwardPreview;
};

class InfinityTowerDifficultyRes : public ResBase {
public:
    InfinityTowerDifficultyRes() = default;
    ~InfinityTowerDifficultyRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int TowerId;
};
