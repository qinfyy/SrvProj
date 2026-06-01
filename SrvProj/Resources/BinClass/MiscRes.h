#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

class WorldClassRes : public ResBase {
public:
    WorldClassRes() = default;
    ~WorldClassRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;


};


class GuideGroupRes : public ResBase {
public:
    GuideGroupRes() = default;
    ~GuideGroupRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;


};

class HandbookRes : public ResBase {
public:
    HandbookRes() = default;
    ~HandbookRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int NodeId;
    int Group;

    int ItemId1;
    int ItemQty1;

};

class SignInRes : public ResBase {
public:
    SignInRes() = default;
    ~SignInRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;


};
