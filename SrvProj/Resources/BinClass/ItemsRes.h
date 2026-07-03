#pragma once

#include <nlohmann/json_fwd.hpp>
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class ItemRes {
public:
    ItemRes() = default;
    ~ItemRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

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

class ProductionRes {
public:
    ProductionRes() = default;
    ~ProductionRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int UnlockWorldLevel;
    int ProductionId;
    int ProductionPerBatch;
    int RawMaterialId1;
    int RawMaterialCount1;

    // 非序列化字段
};

class PlayerHeadRes {
public:
    PlayerHeadRes() = default;
    ~PlayerHeadRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int HeadType;
    int UnlockChar;
    int UnlockSkin;

    // 非序列化字段
};

class TitleRes {
public:
    TitleRes() = default;
    ~TitleRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int ItemId;
    int TitleType;

    // 非序列化字段
};

class HonorRes {
public:
    HonorRes() = default;
    ~HonorRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int Type;
    std::vector<int> Params;

    // 非序列化字段
};

class DropPkgRes {
public:
    DropPkgRes() = default;
    ~DropPkgRes() = default;

    auto GetKey() const { return PkgId; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int PkgId;
    int ItemId;

    // 非序列化字段
};
