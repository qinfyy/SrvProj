#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

class AgentRes : public ResBase {
public:
    AgentRes() = default;
    ~AgentRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

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
};