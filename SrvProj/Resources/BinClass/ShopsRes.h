#pragma once

#include "../ResBase.h"
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class MallMonthlyCardRes : public ResBase {
public:
    MallMonthlyCardRes() = default;
    ~MallMonthlyCardRes() = default;

    std::string GetId() const override { return Id; }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

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

class MonthlyCardRes : public ResBase {
public:
    MonthlyCardRes() = default;
    ~MonthlyCardRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    int Id;
    int CardId;
    int RewardId1;
    int RewardNum1;
    int RewardId2;
    int RewardNum2;

    ItemParamMap Rewards;
};

class MallPackageRes : public ResBase {
public:
    MallPackageRes() = default;
    ~MallPackageRes() = default;

    std::string GetId() const override { return Id; }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

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

class MallShopRes : public ResBase {
public:
    MallShopRes() = default;
    ~MallShopRes() = default;

    std::string GetId() const override { return Id; }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

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

class MallGemRes : public ResBase {
public:
    MallGemRes() = default;
    ~MallGemRes() = default;

    std::string GetId() const override { return Id; }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

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

class ResidentShopRes : public ResBase {
public:
    ResidentShopRes() = default;
    ~ResidentShopRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int RefreshTimeType;
    int RefreshInterval;
    std::string OpenTime;

    // 非序列化字段
    long long OpenTimeSeconds = 0;
};

class ResidentGoodsRes : public ResBase {
public:
    ResidentGoodsRes() = default;
    ~ResidentGoodsRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

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
