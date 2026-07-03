#pragma once

#include <nlohmann/json_fwd.hpp>
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_set>

class ActivityRes {
public:
    ActivityRes() = default;
    ~ActivityRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int ActivityType;

    // 非序列化字段
};

class LoginRewardGroupControlRes {
public:
    LoginRewardGroupControlRes() = default;
    ~LoginRewardGroupControlRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int RewardId1;
    int Qty1;
    int RewardId2;
    int Qty2;

    // 非序列化字段
};

class TowerDefenseLevelRes {
public:
    TowerDefenseLevelRes() = default;
    ~TowerDefenseLevelRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int Condition2;
    int Condition3;
    int Item1;
    int Qty1;
    int Item2;
    int Qty2;

    // 非序列化字段
};

class TrialControlRes {
public:
    TrialControlRes() = default;
    ~TrialControlRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    std::unordered_set<int> GroupIdSet;

    // 非序列化字段
};

class TrialGroupRes {
public:
    TrialGroupRes() = default;
    ~TrialGroupRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int RewardId1;
    int Qty1;
    int RewardId2;
    int Qty2;
    int RewardId3;
    int Qty3;

    // 非序列化字段
};

class JointDrill2LevelRes {
public:
    JointDrill2LevelRes() = default;
    ~JointDrill2LevelRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int BattleTime;
    int TimeScore;
    int ScorePerSec;
    int LevelScore;
    int BaseHpScore;
    std::string RewardPreview;

    // 非序列化字段
};

class ActivityLevelsLevelRes {
public:
    ActivityLevelsLevelRes() = default;
    ~ActivityLevelsLevelRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int ActivityId;
    int EnergyConsume;
    std::string CompleteRewardPreview;

    // 非序列化字段
};

class ActivityTaskRes {
public:
    ActivityTaskRes() = default;
    ~ActivityTaskRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int ActivityTaskGroupId;
    int CompleteCond;
    int AimNumShow;
    int Tid1;
    int Qty1;
    int Tid2;
    int Qty2;

    // 非序列化字段
};

class ActivityTaskGroupRes {
public:
    ActivityTaskGroupRes() = default;
    ~ActivityTaskGroupRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int ActivityId;
    int Reward1;
    int RewardQty1;
    int Reward2;
    int RewardQty2;
    int Reward3;
    int RewardQty3;
    int Reward4;
    int RewardQty4;
    int Reward5;
    int RewardQty5;
    int Reward6;
    int RewardQty6;

    // 非序列化字段
};

class ActivityShopRes {
public:
    ActivityShopRes() = default;
    ~ActivityShopRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int CurrencyItemId;
    int ExchangeItemId;
    double Rate;

    // 非序列化字段
};

class ActivityShopControlRes {
public:
    ActivityShopControlRes() = default;
    ~ActivityShopControlRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    std::vector<int> ShopIds;

    // 非序列化字段
};

class ActivityGoodsRes {
public:
    ActivityGoodsRes() = default;
    ~ActivityGoodsRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int ShopId;
    int ItemId;
    int ItemQuantity;
    int MaximumLimit;
    int Price;

    // 非序列化字段
};
