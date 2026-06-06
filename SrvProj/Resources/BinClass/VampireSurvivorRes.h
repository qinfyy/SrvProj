#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

class VampireSurvivorRes : public ResBase {
public:
    VampireSurvivorRes() = default;
    ~VampireSurvivorRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int Mode;
    int NeedWorldClass;
    std::vector<int> FateCardBundle;

    int NormalScore1;
    int EliteScore1;
    int BossScore1;
    int TimeScore1;
    int TimeLimit1;

    int NormalScore2;
    int EliteScore2;
    int BossScore2;
    int TimeScore2;
    int TimeLimit2;

};

class VampireTalentRes : public ResBase {
public:
    VampireTalentRes() = default;
    ~VampireTalentRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    std::vector<int> Prev;
    int Point;
};
