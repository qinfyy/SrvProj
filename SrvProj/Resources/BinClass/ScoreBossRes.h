#pragma once

#include "../ResBase.h"
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_set>

class ScoreBossControlRes : public ResBase {
public:
    ScoreBossControlRes() = default;
    ~ScoreBossControlRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    std::string StartTime;
    std::string EndTime;
    std::unordered_set<int> LevelGroup;

    // 非序列化字段
};

class ScoreBossRewardRes : public ResBase {
public:
    ScoreBossRewardRes() = default;
    ~ScoreBossRewardRes() = default;

    std::string GetId() const override { return std::to_string(StarNeed); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int StarNeed;
    int RewardItemId1;
    int RewardNum1;

    // 非序列化字段
};