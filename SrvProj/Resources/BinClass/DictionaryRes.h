#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

class DictionaryTabRes : public ResBase {
public:
    DictionaryTabRes() = default;
    ~DictionaryTabRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;

    //List<DictionaryEntryDef> entries;
};

class DictionaryEntryRes : public ResBase {
public:
    DictionaryEntryRes() = default;
    ~DictionaryEntryRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int Tab;
    int Index;
};
