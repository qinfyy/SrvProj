#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_set>

class ActivityRes : public ResBase {
public:
    ActivityRes() = default;
    ~ActivityRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int ActivityType;
};

class LoginRewardGroupControlRes : public ResBase {
public:
    LoginRewardGroupControlRes() = default;
    ~LoginRewardGroupControlRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;

    int RewardId1;
    int Qty1;
    int RewardId2;
    int Qty2;
};

class TowerDefenseLevelRes : public ResBase {
public:
    TowerDefenseLevelRes() = default;
    ~TowerDefenseLevelRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int Condition2;
    int Condition3;
    int Item1;
    int Qty1;
    int Item2;
    int Qty2;
};

class TrialControlRes : public ResBase {
public:
    TrialControlRes() = default;
    ~TrialControlRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
	std::unordered_set<int> GroupIdSet;
};

class TrialGroupRes : public ResBase {
public:
    TrialGroupRes() = default;
    ~TrialGroupRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;

    int RewardId1;
    int Qty1;
    int RewardId2;
    int Qty2;
    int RewardId3;
    int Qty3;
};

class JointDrill2LevelRes : public ResBase {
public:
    JointDrill2LevelRes() = default;
    ~JointDrill2LevelRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int BattleTime;

    int TimeScore;
    int ScorePerSec;
    int LevelScore;
    int BaseHpScore;

    std::string RewardPreview;
};

class ActivityLevelsLevelRes : public ResBase {
public:
    ActivityLevelsLevelRes() = default;
    ~ActivityLevelsLevelRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int ActivityId;
    int EnergyConsume;
    std::string CompleteRewardPreview;
};

class ActivityTaskRes : public ResBase {
public:
    ActivityTaskRes() = default;
    ~ActivityTaskRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int ActivityTaskGroupId;

    int CompleteCond;
    int AimNumShow;

    int Tid1;
    int Qty1;
    int Tid2;
    int Qty2;
};

class ActivityTaskGroupRes : public ResBase {
public:
    ActivityTaskGroupRes() = default;
    ~ActivityTaskGroupRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

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
};

class ActivityShopRes : public ResBase {
public:
    ActivityShopRes() = default;
    ~ActivityShopRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int CurrencyItemId;
    int ExchangeItemId;
    double Rate;
};

class ActivityShopControlRes : public ResBase {
public:
    ActivityShopControlRes() = default;
    ~ActivityShopControlRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    std::string ShopIds;
};

class ActivityGoodsRes : public ResBase {
public:
    ActivityGoodsRes() = default;
    ~ActivityGoodsRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int ShopId;

    int ItemId;
    int ItemQuantity;
    int MaximumLimit;
    int Price;
};