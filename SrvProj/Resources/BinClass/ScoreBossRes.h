#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_set>

class ScoreBossControlRes : public ResBase {
public:
    ScoreBossControlRes() = default;
    ~ScoreBossControlRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    std::string StartTime;
    std::string EndTime;
    std::unordered_set<int> LevelGroup;
};

class ScoreBossRewardRes : public ResBase {
public:
    ScoreBossRewardRes() = default;
    ~ScoreBossRewardRes() = default;

    std::string GetId() const override { return std::to_string(StarNeed); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int StarNeed;
    int RewardItemId1;
    int RewardNum1;
};