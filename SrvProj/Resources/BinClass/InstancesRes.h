#pragma once

#include <nlohmann/json_fwd.hpp>
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class DailyInstanceRes {
public:
    DailyInstanceRes() = default;
    ~DailyInstanceRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int AwardDropId;
    int PreLevelId;
    int PreLevelStar;
    int OneStarEnergyConsume;
    int NeedWorldClass;

    // 非序列化字段
};

class DailyInstanceRewardGroupRes {
public:
    DailyInstanceRewardGroupRes() = default;
    ~DailyInstanceRewardGroupRes() = default;

    auto GetKey() const { return GroupId; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int GroupId;
    int DailyRewardType;
    std::string BaseAwardPreview;

    // 非序列化字段
};

class RegionBossLevelRes {
public:
    RegionBossLevelRes() = default;
    ~RegionBossLevelRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int PreLevelId;
    int PreLevelStar;
    int NeedWorldClass;
    int EnergyConsume;
    std::string BaseAwardPreview;

    // 非序列化字段
};

class SkillInstanceRes {
public:
    SkillInstanceRes() = default;
    ~SkillInstanceRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int PreLevelId;
    int PreLevelStar;
    int NeedWorldClass;
    int EnergyConsume;
    std::string BaseAwardPreview;

    // 非序列化字段
};

class CharGemInstanceRes {
public:
    CharGemInstanceRes() = default;
    ~CharGemInstanceRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int PreLevelId;
    int PreLevelStar;
    int NeedWorldClass;
    int EnergyConsume;
    std::string BaseAwardPreview;

    // 非序列化字段
};

class WeekBossLevelRes {
public:
    WeekBossLevelRes() = default;
    ~WeekBossLevelRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int Difficulty;
    int PreLevelId;
    int NeedWorldClass;
    std::string BaseAwardPreview;

    // 非序列化字段
};
