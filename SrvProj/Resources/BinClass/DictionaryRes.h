#pragma once

#include <nlohmann/json_fwd.hpp>
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class DictionaryTabRes {
public:
    DictionaryTabRes() = default;
    ~DictionaryTabRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;

    //List<DictionaryEntryDef> entries;

    // 非序列化字段
};

class DictionaryEntryRes {
public:
    DictionaryEntryRes() = default;
    ~DictionaryEntryRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int Tab;
    int Index;

    // 非序列化字段
};
