#pragma once

#include "../ResBase.h"
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class AchievementRes : public ResBase {
public:
    AchievementRes() = default;
    ~AchievementRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

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
