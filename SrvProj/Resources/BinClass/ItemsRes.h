#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

class ItemRes : public ResBase {
public:
    ItemRes() = default;
    ~ItemRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    std::string Title;
    int Type;
    int Stype;
    int Rarity;
    bool Stack;

    int UseMode;
    int UseAction;
    std::string UseArgs;
};

class ProductionRes : public ResBase {
public:
    ProductionRes() = default;
    ~ProductionRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int UnlockWorldLevel;
    int ProductionId;
    int ProductionPerBatch;
    int RawMaterialId1;
    int RawMaterialCount1;
};

class PlayerHeadRes : public ResBase {
public:
    PlayerHeadRes() = default;
    ~PlayerHeadRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int HeadType;
    int UnlockChar;
    int UnlockSkin;
};

class TitleRes : public ResBase {
public:
    TitleRes() = default;
    ~TitleRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int ItemId;
    int TitleType;

};

class HonorRes : public ResBase {
public:
    HonorRes() = default;
    ~HonorRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int Type;
    std::vector<int> Params;
};

class DropPkgRes : public ResBase {
private:
    DropPkgRes() = default;
    ~DropPkgRes() = default;

    std::string GetId() const override { return std::to_string(PkgId); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;


    int PkgId;
    int ItemId;
};
