#pragma once

#include "../ResBase.h"
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class DiscRes : public ResBase {
public:
    DiscRes() = default;
    ~DiscRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

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

    // 非序列化字段
};

class DiscStrengthenRes : public ResBase {
public:
    DiscStrengthenRes() = default;
    ~DiscStrengthenRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    bool LoadFromPb(std::string data) override;
    void OnLoad() override {};

    // 序列化字段

    int Id;
    int Exp;

    // 非序列化字段

};

class DiscItemExpRes : public ResBase {
public:
    DiscItemExpRes() = default;
    ~DiscItemExpRes() = default;

    std::string GetId() const override { return std::to_string(ItemId); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int ItemId;
    int Exp;

    // 非序列化字段
};

class DiscPromoteRes : public ResBase {
public:
    DiscPromoteRes() = default;
    ~DiscPromoteRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int ItemId1;
    int Num1;
    int ItemId2;
    int Num2;
    int ItemId3;
    int Num3;
    int ExpenseGold;

    // 非序列化字段

};

class DiscPromoteLimitRes : public ResBase {
public:
    DiscPromoteLimitRes() = default;
    ~DiscPromoteLimitRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int Rarity;
    std::string Phase;
    std::string MaxLevel;
    int WorldClassLimit;

    // 非序列化字段
};

class SecondarySkillRes : public ResBase {
public:
    SecondarySkillRes() = default;
    ~SecondarySkillRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int GroupId;
    int Score;
    std::string NeedSubNoteSkills;

    // 非序列化字段
};
