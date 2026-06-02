#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

class StoryRes : public ResBase {
public:
    StoryRes() = default;
    ~StoryRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;
    
    int Id;
    int Chapter;
    std::string RewardDisplay;

};

class StorySetSectionRes : public ResBase {
public:
    StorySetSectionRes() = default;
    ~StorySetSectionRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int ChapterId;

    int RewardItem1Tid;
    int RewardItem1Qty;
};

class StoryEvidenceRes : public ResBase {
public:
    StoryEvidenceRes() = default;
    ~StoryEvidenceRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
};

class MainScreenCGRes : public ResBase {
public:
    MainScreenCGRes() = default;
    ~MainScreenCGRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    bool IsShown;
};
