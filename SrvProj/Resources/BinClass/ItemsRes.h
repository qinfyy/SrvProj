#pragma once

#include "../ResBase.h"
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class ItemRes : public ResBase {
public:
    ItemRes() = default;
    ~ItemRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    std::string Title;
    int Type;
    int Stype;
    int Rarity;
    bool Stack;

    int UseMode;
    int UseAction;
    std::string UseArgs;

    // 非序列化字段
};

class ProductionRes : public ResBase {
public:
    ProductionRes() = default;
    ~ProductionRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int UnlockWorldLevel;
    int ProductionId;
    int ProductionPerBatch;
    int RawMaterialId1;
    int RawMaterialCount1;

    // 非序列化字段
};

class PlayerHeadRes : public ResBase {
public:
    PlayerHeadRes() = default;
    ~PlayerHeadRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int HeadType;
    int UnlockChar;
    int UnlockSkin;

    // 非序列化字段
};

class TitleRes : public ResBase {
public:
    TitleRes() = default;
    ~TitleRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int ItemId;
    int TitleType;

    // 非序列化字段
};

class HonorRes : public ResBase {
public:
    HonorRes() = default;
    ~HonorRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int Type;
    std::vector<int> Params;

    // 非序列化字段
};

class DropPkgRes : public ResBase {
public:
    DropPkgRes() = default;
    ~DropPkgRes() = default;

    std::string GetId() const override { return std::to_string(PkgId); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段


    int PkgId;
    int ItemId;

    // 非序列化字段
};
