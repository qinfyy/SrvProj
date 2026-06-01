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


};


class StarTowerStageRes : public ResBase {
public:
    StarTowerStageRes() = default;
    ~StarTowerStageRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;


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


};

class StarTowerTeamExpRes : public ResBase {
public:
    StarTowerTeamExpRes() = default;
    ~StarTowerTeamExpRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;


};

class StarTowerEventRes : public ResBase {
public:
    StarTowerEventRes() = default;
    ~StarTowerEventRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;


};







class StarTowerBuildRankRes : public ResBase {
public:
    StarTowerBuildRankRes() = default;
    ~StarTowerBuildRankRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;


};

class SubNoteSkillPromoteGroupRes : public ResBase {
public:
    SubNoteSkillPromoteGroupRes() = default;
    ~SubNoteSkillPromoteGroupRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;


};

class PotentialRes : public ResBase {
public:
    PotentialRes() = default;
    ~PotentialRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;


};

class CharPotentialRes : public ResBase {
public:
    CharPotentialRes() = default;
    ~CharPotentialRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;


};

class StarTowerBookFateCardBundleRes : public ResBase {
public:
    StarTowerBookFateCardBundleRes() = default;
    ~StarTowerBookFateCardBundleRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;


};

class StarTowerBookFateCardQuestRes : public ResBase {
public:
    StarTowerBookFateCardQuestRes() = default;
    ~StarTowerBookFateCardQuestRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;


};

class StarTowerBookFateCardRes : public ResBase {
public:
    StarTowerBookFateCardRes() = default;
    ~StarTowerBookFateCardRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;


};

class FateCardRes : public ResBase {
public:
    FateCardRes() = default;
    ~FateCardRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;


};

class InfinityTowerLevelRes : public ResBase {
public:
    InfinityTowerLevelRes() = default;
    ~InfinityTowerLevelRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;


};
