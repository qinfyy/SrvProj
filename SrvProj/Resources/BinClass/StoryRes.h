#pragma once

#include <nlohmann/json_fwd.hpp>
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class StoryRes {
public:
    StoryRes() = default;
    ~StoryRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int Chapter;
    std::string RewardDisplay;

    // 非序列化字段
    ItemParamMap Rewards;
};

class StorySetSectionRes {
public:
    StorySetSectionRes() = default;
    ~StorySetSectionRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int ChapterId;
    int RewardItem1Tid;
    int RewardItem1Qty;

    // 非序列化字段
    ItemParamMap Rewards;
};

class StoryEvidenceRes {
public:
    StoryEvidenceRes() = default;
    ~StoryEvidenceRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;

    // 非序列化字段
};

class MainScreenCGRes {
public:
    MainScreenCGRes() = default;
    ~MainScreenCGRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    bool IsShown;

    // 非序列化字段
};
