#pragma once

#include <nlohmann/json_fwd.hpp>
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_set>

class ScoreBossControlRes {
public:
    ScoreBossControlRes() = default;
    ~ScoreBossControlRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    std::string StartTime;
    std::string EndTime;
    std::unordered_set<int> LevelGroup;

    // 非序列化字段
};

class ScoreBossRewardRes {
public:
    ScoreBossRewardRes() = default;
    ~ScoreBossRewardRes() = default;

    auto GetKey() const { return StarNeed; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int StarNeed;
    int RewardItemId1;
    int RewardNum1;

    // 非序列化字段
};
