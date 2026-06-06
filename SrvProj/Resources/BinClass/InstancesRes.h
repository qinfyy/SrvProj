#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

class DailyInstanceRes : public ResBase {
public:
    DailyInstanceRes() = default;
    ~DailyInstanceRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int AwardDropId;
    int PreLevelId;
    int PreLevelStar;
    int OneStarEnergyConsume;
    int NeedWorldClass;
};

class DailyInstanceRewardGroupRes : public ResBase {
public:
    DailyInstanceRewardGroupRes() = default;
    ~DailyInstanceRewardGroupRes() = default;

    std::string GetId() const override { return std::to_string(GroupId); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int GroupId;
    int DailyRewardType;
    std::string BaseAwardPreview;
};

class RegionBossLevelRes : public ResBase {
public:
    RegionBossLevelRes() = default;
    ~RegionBossLevelRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int PreLevelId;
    int PreLevelStar;
    int NeedWorldClass;
    int EnergyConsume;
    std::string BaseAwardPreview;
};

class SkillInstanceRes : public ResBase {
public:
    SkillInstanceRes() = default;
    ~SkillInstanceRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int PreLevelId;
    int PreLevelStar;
    int NeedWorldClass;
    int EnergyConsume;
    std::string BaseAwardPreview;
};

class CharGemInstanceRes : public ResBase {
public:
    CharGemInstanceRes() = default;
    ~CharGemInstanceRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int PreLevelId;
    int PreLevelStar;
    int NeedWorldClass;
    int EnergyConsume;
    std::string BaseAwardPreview;
};

class WeekBossLevelRes : public ResBase {
public:
    WeekBossLevelRes() = default;
    ~WeekBossLevelRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int Difficulty;
    int PreLevelId;
    int NeedWorldClass;
    std::string BaseAwardPreview;
};
