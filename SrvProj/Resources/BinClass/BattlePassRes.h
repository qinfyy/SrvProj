#pragma once
#include <nlohmann/json_fwd.hpp>
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class BattlePassRes {
public:
    BattlePassRes() = default;
    ~BattlePassRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

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

class BattlePassLevelRes {
public:
    BattlePassLevelRes() = default;
    ~BattlePassLevelRes() = default;

    auto GetKey() const { return ID; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int ID;
    int Exp;
    int Tid;
    int Qty;

    // 非序列化字段
};

class BattlePassQuestRes {
public:
    BattlePassQuestRes() = default;
    ~BattlePassQuestRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段
    int Id;
    int Type;
    int Exp;

    // 非序列化字段
};

class BattlePassRewardRes {
public:
    BattlePassRewardRes() = default;
    ~BattlePassRewardRes() = default;

    auto GetKey() const { return std::pair<int, int>{ID, Level}; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

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
