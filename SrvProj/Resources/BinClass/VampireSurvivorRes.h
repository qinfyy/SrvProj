#pragma once

#include <nlohmann/json_fwd.hpp>
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class VampireSurvivorRes {
public:
    VampireSurvivorRes() = default;
    ~VampireSurvivorRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

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

    // 非序列化字段
};
class VampireTalentRes {
public:
    VampireTalentRes() = default;
    ~VampireTalentRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    std::vector<int> Prev;
    int Point;

    // 非序列化字段
};
