#pragma once

#include <nlohmann/json_fwd.hpp>
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class MallMonthlyCardRes {
public:
    MallMonthlyCardRes() = default;
    ~MallMonthlyCardRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    std::string Id;
    int MonthlyCardId;
    int Price;
    int BaseItemId;
    int BaseItemQty;
    int MaxDays;

    // 非序列化字段
    ItemParamMap Products;
};

class MonthlyCardRes {
public:
    MonthlyCardRes() = default;
    ~MonthlyCardRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    int Id;
    int CardId;
    int RewardId1;
    int RewardNum1;
    int RewardId2;
    int RewardNum2;

    ItemParamMap Rewards;
};

class MallPackageRes {
public:
    MallPackageRes() = default;
    ~MallPackageRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    std::string Id;
    int Stock;
    int CurrencyType;
    int CurrencyItemId;
    int CurrencyItemQty;
    int Tag;
    int RefreshType;
    std::string Items;
    int ListCondType;
    std::string ListCondParams;
    int OrderCondType;
    std::string OrderCondParams;
    std::string ListTime;
    std::string DeListTime;

    // 非序列化字段
    ItemParamMap Products;
    std::vector<int> ListCond;
    std::vector<int> OrderCond;
    long long ListTimeSeconds = 0;
    long long DeListTimeSeconds = 0;
};

class MallShopRes {
public:
    MallShopRes() = default;
    ~MallShopRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    std::string Id;
    int Stock;
    int ExchangeItemId;
    int ExchangeItemQty;
    int ItemId;
    int ItemQty;
    int RefreshType;
    int ListCondType;
    std::string ListCondParams;
    int OrderCondType;
    std::string OrderCondParams;
    std::string ListTime;
    std::string DeListTime;

    // 非序列化字段
    ItemParamMap Products;
    std::vector<int> ListCond;
    std::vector<int> OrderCond;
    long long ListTimeSeconds = 0;
    long long DeListTimeSeconds = 0;
};

class MallGemRes {
public:
    MallGemRes() = default;
    ~MallGemRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    std::string Id;
    int BaseItemId;
    int BaseItemQty;
    int ExperiencedBonusItemId;
    int ExperiencedBonusItemQty;
    int MaidenBonusItemID;
    int MaidenBonusItemQty;
    int Price;

    // 非序列化字段
};

class ResidentShopRes {
public:
    ResidentShopRes() = default;
    ~ResidentShopRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int RefreshTimeType;
    int RefreshInterval;
    std::string OpenTime;

    // 非序列化字段
    long long OpenTimeSeconds = 0;
};

class ResidentGoodsRes {
public:
    ResidentGoodsRes() = default;
    ~ResidentGoodsRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int ShopId;
    int MaximumLimit;
    int ItemId;
    int ItemQuantity;
    int CurrencyItemId;
    int Price;
    int AppearCondType;
    std::string AppearCondParams;

    // 非序列化字段
    ItemParamMap Products;
    std::vector<int> AppearCond;
};
