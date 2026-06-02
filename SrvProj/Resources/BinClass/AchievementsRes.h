#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

class AchievementRes : public ResBase {
public:
    AchievementRes() = default;
    ~AchievementRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int Type;
    int CompleteCond;
    int AimNumShow;
    std::vector<int> Prerequisites;

    // Reward
    int Tid1;
    int Qty1;
};