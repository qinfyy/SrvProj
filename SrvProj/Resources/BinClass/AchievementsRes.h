#pragma once

#include <nlohmann/json_fwd.hpp>
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class AchievementRes {
public:
    AchievementRes() = default;
    ~AchievementRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id = 0;
    int Type = 0;
    int CompleteCond = 0;
    int AimNumShow = 0;
    std::vector<int> Prerequisites;

    // Reward
    int Tid1 = 0;
    int Qty1 = 0;

    // 非序列化字段
    int Param1 = 0;
    int ParamCond1 = 0;
    int Param2 = 0;
    int ParamCond2 = 0;

    bool MatchParam1(int value) const;
    bool MatchParam2(int value) const;
    bool MatchParams(int param1, int param2) const;
};
