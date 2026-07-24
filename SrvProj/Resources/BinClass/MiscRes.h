#pragma once

#include <nlohmann/json_fwd.hpp>
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class WorldClassRes {
public:
    WorldClassRes() = default;
    ~WorldClassRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int Exp;
    std::string Reward;

    // 非序列化字段
    ItemParamMap Rewards;
};

class GuideGroupRes {
public:
    GuideGroupRes() = default;
    ~GuideGroupRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    bool IsActive;

    // 非序列化字段
};

class HandbookRes {
public:
    HandbookRes() = default;
    ~HandbookRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int Index;
    int Type;

    // 非序列化字段
};

class SignInRes {
public:
    SignInRes() = default;
    ~SignInRes() = default;

    auto GetKey() const { return std::pair<int, int>{Group, Day}; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Group;
    int Day;
    int ItemId;
    int ItemQty;

    // 非序列化字段
};
