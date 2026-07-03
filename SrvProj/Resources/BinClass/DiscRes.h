#pragma once

#include <nlohmann/json_fwd.hpp>
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class DiscRes {
public:
    DiscRes() = default;
    ~DiscRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

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

class DiscStrengthenRes {
public:
    DiscStrengthenRes() = default;
    ~DiscStrengthenRes() = default;

    auto GetKey() const { return Id; }
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);
    void OnLoad() {};

    // 序列化字段

    int Id;
    int Exp;

    // 非序列化字段

};

class DiscItemExpRes {
public:
    DiscItemExpRes() = default;
    ~DiscItemExpRes() = default;

    auto GetKey() const { return ItemId; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int ItemId;
    int Exp;

    // 非序列化字段
};

class DiscPromoteRes {
public:
    DiscPromoteRes() = default;
    ~DiscPromoteRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

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

class DiscPromoteLimitRes {
public:
    DiscPromoteLimitRes() = default;
    ~DiscPromoteLimitRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int Rarity;
    std::string Phase;
    std::string MaxLevel;
    int WorldClassLimit;

    // 非序列化字段
};

class SecondarySkillRes {
public:
    SecondarySkillRes() = default;
    ~SecondarySkillRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int GroupId;
    int Level;
    int Score;
    std::string NeedSubNoteSkills;

    // 非序列化字段
    ItemParamMap NeedSubNotes;

    bool Match(const ItemParamMap& subNotes) const;
    static int GetSecondarySkill(const ItemParamMap& subNotes, int groupId);
    static std::vector<int> CalculateSecondarySkills(const std::vector<uint32_t>& discIds, const ItemParamMap& subNotes);
};
