#pragma once

#include "../ResBase.h"
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class StoryRes : public ResBase {
public:
    StoryRes() = default;
    ~StoryRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段
    
    int Id;
    int Chapter;
    std::string RewardDisplay;

    // 非序列化字段
};

class StorySetSectionRes : public ResBase {
public:
    StorySetSectionRes() = default;
    ~StorySetSectionRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int ChapterId;

    int RewardItem1Tid;
    int RewardItem1Qty;

    // 非序列化字段
};

class StoryEvidenceRes : public ResBase {
public:
    StoryEvidenceRes() = default;
    ~StoryEvidenceRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;

    // 非序列化字段
};

class MainScreenCGRes : public ResBase {
public:
    MainScreenCGRes() = default;
    ~MainScreenCGRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    bool IsShown;

    // 非序列化字段
};
