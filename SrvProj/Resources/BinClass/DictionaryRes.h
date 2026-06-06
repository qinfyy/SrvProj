#pragma once

#include "../ResBase.h"
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class DictionaryTabRes : public ResBase {
public:
    DictionaryTabRes() = default;
    ~DictionaryTabRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;

    //List<DictionaryEntryDef> entries;

    // 非序列化字段
};

class DictionaryEntryRes : public ResBase {
public:
    DictionaryEntryRes() = default;
    ~DictionaryEntryRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int Tab;
    int Index;

    // 非序列化字段
};
