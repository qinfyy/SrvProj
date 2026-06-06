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

    // 非序列化字段
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
    std::string Items;

    // 非序列化字段
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

    // 非序列化字段
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
    //int Stock;
    //int ItemId;
    //int CurrencyItemId;
    //int ItemQty;

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

    // 非序列化字段
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

    // 非序列化字段
};

