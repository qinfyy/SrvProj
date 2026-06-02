#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

class WorldClassRes : public ResBase {
public:
    WorldClassRes() = default;
    ~WorldClassRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int Exp;
    std::string Reward;
};


class GuideGroupRes : public ResBase {
public:
    GuideGroupRes() = default;
    ~GuideGroupRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    bool IsActive;
};

class HandbookRes : public ResBase {
public:
    HandbookRes() = default;
    ~HandbookRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int Index;
    int Type;

};

class SignInRes : public ResBase {
public:
    SignInRes() = default;
    ~SignInRes() = default;

    std::string GetId() const override { return std::to_string((Group << 16) + Day); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Group;
    int Day;
    int ItemId;
    int ItemQty;
};
