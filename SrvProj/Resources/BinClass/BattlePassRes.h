#pragma once
#include "../ResBase.h"
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class BattlePassRes : public ResBase {
public:
    BattlePassRes() = default;
    ~BattlePassRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    std::string StartTimeText;
    std::string EndTimeText;
    std::string LuxuryProductId;
    std::string PremiumProductId;
    int LuxuryBonusLevel = 0;
    int LuxuryTid = 0;
    int LuxuryQty = 0;
    int ComplementaryTid = 0;
    int ComplementaryQty = 0;

    // 非序列化字段
    int64_t StartTime = 0;
    int64_t EndTime = 0;
};

class BattlePassLevelRes : public ResBase {
public:
    BattlePassLevelRes() = default;
    ~BattlePassLevelRes() = default;

    std::string GetId() const override { return std::to_string(ID); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int ID;
    int Exp;
    int Tid;
    int Qty;

    // 非序列化字段
};

class BattlePassQuestRes : public ResBase {
public:
    BattlePassQuestRes() = default;
    ~BattlePassQuestRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段
    int Id;
    int Type;
    int Exp;

    // 非序列化字段
};

class BattlePassRewardRes : public ResBase {
public:
    BattlePassRewardRes() = default;
    ~BattlePassRewardRes() = default;

    std::string GetId() const override { return std::to_string(ID) + "|" + std::to_string(Level); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int ID;
    int Level;
    int Tid1;
    int Qty1;
    int Tid2;
    int Qty2;
    int Tid3;
    int Qty3;

    // 非序列化字段

};
