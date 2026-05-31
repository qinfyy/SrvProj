#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

class StoryRes : public ResBase {
public:
    StoryRes() = default;
    ~StoryRes() = default;

    int GetId() const override { return Id; }
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

    int GetId() const override { return Id; }
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

    int GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
};

class MainScreenCGRes : public ResBase {
public:
    MainScreenCGRes() = default;
    ~MainScreenCGRes() = default;

    int GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    bool IsShown;
};