#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

class BattlePassRes : public ResBase {
public:
    BattlePassRes() = default;
    ~BattlePassRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
};

class BattlePassLevelRes : public ResBase {
public:
    BattlePassLevelRes() = default;
    ~BattlePassLevelRes() = default;

    std::string GetId() const override { return std::to_string(ID); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int ID;
    int Exp;

    int Tid;
    int Qty;
};

class BattlePassQuestRes : public ResBase {
public:
    BattlePassQuestRes() = default;
    ~BattlePassQuestRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int Type;
    int Exp;

};

class BattlePassRewardRes : public ResBase {
public:
    BattlePassRewardRes() = default;
    ~BattlePassRewardRes() = default;

    std::string GetId() const override { return std::to_string(ID); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int ID;
    int Level;

    int Tid1;
    int Qty1;
    int Tid2;
    int Qty2;
    int Tid3;
    int Qty3;
};



