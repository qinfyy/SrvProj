#pragma once

#include <nlohmann/json_fwd.hpp>
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class DailyQuestRes {
public:
    DailyQuestRes() = default;
    ~DailyQuestRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    bool Apear;
    int Active;
    int ItemTid;
    int ItemQty;
    int CompleteCond;
    int CompleteCondClient;
    std::string CompleteCondParams;

    // 非序列化字段
};

class DailyQuestActiveRes {
public:
    DailyQuestActiveRes() = default;
    ~DailyQuestActiveRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int Active;
    int ItemTid1;
    int Number1;
    int ItemTid2;
    int Number2;

    // 非序列化字段
};
class WeeklyQuestRes {
public:
    WeeklyQuestRes() = default;
    ~WeeklyQuestRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    bool Apear;
    int Active;
    int ItemTid;
    int ItemQty;
    int CompleteCond;
    int CompleteCondClient;
    std::string CompleteCondParams;

    // 非序列化字段
};

class WeeklyQuestActiveRes {
public:
    WeeklyQuestActiveRes() = default;
    ~WeeklyQuestActiveRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int Active;
    int ItemTid1;
    int Number1;
    int ItemTid2;
    int Number2;

    // 非序列化字段
};
