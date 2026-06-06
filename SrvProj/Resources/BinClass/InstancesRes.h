#pragma once

#include "../ResBase.h"
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class DailyInstanceRes : public ResBase {
public:
    DailyInstanceRes() = default;
    ~DailyInstanceRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int AwardDropId;
    int PreLevelId;
    int PreLevelStar;
    int OneStarEnergyConsume;
    int NeedWorldClass;

    // 非序列化字段
};

class DailyInstanceRewardGroupRes : public ResBase {
public:
    DailyInstanceRewardGroupRes() = default;
    ~DailyInstanceRewardGroupRes() = default;

    std::string GetId() const override { return std::to_string(GroupId); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int GroupId;
    int DailyRewardType;
    std::string BaseAwardPreview;

    // 非序列化字段
};

class RegionBossLevelRes : public ResBase {
public:
    RegionBossLevelRes() = default;
    ~RegionBossLevelRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int PreLevelId;
    int PreLevelStar;
    int NeedWorldClass;
    int EnergyConsume;
    std::string BaseAwardPreview;

    // 非序列化字段
};

class SkillInstanceRes : public ResBase {
public:
    SkillInstanceRes() = default;
    ~SkillInstanceRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int PreLevelId;
    int PreLevelStar;
    int NeedWorldClass;
    int EnergyConsume;
    std::string BaseAwardPreview;

    // 非序列化字段
};

class CharGemInstanceRes : public ResBase {
public:
    CharGemInstanceRes() = default;
    ~CharGemInstanceRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int PreLevelId;
    int PreLevelStar;
    int NeedWorldClass;
    int EnergyConsume;
    std::string BaseAwardPreview;

    // 非序列化字段
};

class WeekBossLevelRes : public ResBase {
public:
    WeekBossLevelRes() = default;
    ~WeekBossLevelRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int Difficulty;
    int PreLevelId;
    int NeedWorldClass;
    std::string BaseAwardPreview;

    // 非序列化字段
};
