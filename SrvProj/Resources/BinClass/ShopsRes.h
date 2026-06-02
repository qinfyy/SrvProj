#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

class MallMonthlyCardRes : public ResBase {
public:
    MallMonthlyCardRes() = default;
    ~MallMonthlyCardRes() = default;

    std::string GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    std::string Id;
    int MonthlyCardId;
    int Price;
    int BaseItemId;
    int BaseItemQty;
};

class MallPackageRes : public ResBase {
public:
    MallPackageRes() = default;
    ~MallPackageRes() = default;

    std::string GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    std::string Id;
    int Stock;
    int CurrencyType;
    int CurrencyItemId;
    int CurrencyItemQty;
    std::string Items;
};

class MallShopRes : public ResBase {
public:
    MallShopRes() = default;
    ~MallShopRes() = default;
    
    std::string GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    std::string Id;
    int Stock;

    int ExchangeItemId;
    int ExchangeItemQty;

    int ItemId;
    int ItemQty;
};

class MallGemRes : public ResBase {
public:
    MallGemRes() = default;
    ~MallGemRes() = default;

    std::string GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    std::string Id;
    //int Stock;
    //int ItemId;
    //int CurrencyItemId;
    //int ItemQty;
};

class ResidentShopRes : public ResBase {
public:
    ResidentShopRes() = default;
    ~ResidentShopRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
};

class ResidentGoodsRes : public ResBase {
public:
    ResidentGoodsRes() = default;
    ~ResidentGoodsRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;


    int Id;
    int ShopId;
    int MaximumLimit;

    int ItemId;
    int ItemQuantity;

    int CurrencyItemId;
    int Price;
};

