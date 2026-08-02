#pragma once

#include <nlohmann/json_fwd.hpp>
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

class AgentRes {
public:
    AgentRes() = default;
    ~AgentRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int Level;
    int MemberLimit;
    std::vector<int> Tags;
    std::vector<int> ExtraTags;
    int Time1;
    std::string RewardPreview1;
    std::string BonusPreview1;
    int Time2;
    std::string RewardPreview2;
    std::string BonusPreview2;
    int Time3;
    std::string RewardPreview3;
    std::string BonusPreview3;
    int Time4;
    std::string RewardPreview4;
    std::string BonusPreview4;

    // 非序列化字段
    std::unordered_map<int, ItemRewardList> DurationRewards;
    std::unordered_map<int, ItemRewardList> DurationBonusRewards;
    std::unordered_map<int, int> TagCounts;
    std::unordered_map<int, int> ExtraTagCounts;
};
