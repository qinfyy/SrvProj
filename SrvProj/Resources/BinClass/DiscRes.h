#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

class DiscRes : public ResBase {
public:
    DiscRes() = default;
	~DiscRes() = default;
	

    int GetId() const override { return Id; }

    void OnLoad() override;

    bool LoadFromPb(std::string data) override;

    int Id;
    bool Visible;
    bool Available;
    int EET;

    int StrengthenGroupId;
    int PromoteGroupId;
    int TransformItemId;
    std::vector<int> MaxStarTransformItem;
    std::vector<int> ReadReward;

    int SecondarySkillGroupId1;
    int SecondarySkillGroupId2;
    int SubNoteSkillGroupId;
};