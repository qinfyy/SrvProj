#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

class DiscRes : public ResBase {
public:
    DiscRes() = default;
	~DiscRes() = default;
	

    std::string GetId() const override { return std::to_string(Id); }

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

class DiscStrengthenRes : public ResBase {
public:
	DiscStrengthenRes() = default;
	~DiscStrengthenRes() = default;

	std::string GetId() const override { return std::to_string(Id); }
	bool LoadFromPb(std::string data) override;

	int Id;
	int Exp;
};
class DiscItemExpRes : public ResBase {
public:
    public:
    DiscItemExpRes() = default;
	~DiscItemExpRes() = default;
	std::string GetId() const override { return std::to_string(ItemId); }
	bool LoadFromPb(std::string data) override;

	int ItemId;
	int Exp;
};
class DiscPromoteRes : public ResBase {
public:
	DiscPromoteRes() = default;
	~DiscPromoteRes() = default;
	std::string GetId() const override { return std::to_string(Id); }
	bool LoadFromPb(std::string data) override;
	int Id;
	int Group;
	int AdvanceLvl;
	int ItemId1;
	int Num1;
	int ItemId2;
	int Num2;
	int ItemId3;
	int Num3;
	int ExpenseGold;
};
class DiscPromoteLimitRes : public ResBase {
public:
	DiscPromoteLimitRes() = default;
	~DiscPromoteLimitRes() = default;
	std::string GetId() const override { return std::to_string(Id); }
	bool LoadFromPb(std::string data) override;
	int Id;
	int Rarity;
	std::string Phase;
	std::string MaxLevel;
	int WorldClassLimit;
};
class SecondarySkillRes : public ResBase {
public:
	SecondarySkillRes() = default;
	~SecondarySkillRes() = default;
	std::string GetId() const override { return std::to_string(Id); }
	bool LoadFromPb(std::string data) override;
	int Id;
	int GroupId;
	int Score;
	std::string NeedSubNoteSkills;

};