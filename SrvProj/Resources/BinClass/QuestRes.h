#pragma once

#include "../ResBase.h"
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class DailyQuestRes : public ResBase {
public:
    DailyQuestRes() = default;
    ~DailyQuestRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

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

class DailyQuestActiveRes : public ResBase {
public:
    DailyQuestActiveRes() = default;
    ~DailyQuestActiveRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int Active;

    int ItemTid1;
    int Number1;
    int ItemTid2;
    int Number2;

    // 非序列化字段
};

class WeeklyQuestRes : public ResBase {
public:
    WeeklyQuestRes() = default;
    ~WeeklyQuestRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

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

class WeeklyQuestActiveRes : public ResBase {
public:
    WeeklyQuestActiveRes() = default;
    ~WeeklyQuestActiveRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int Active;

    int ItemTid1;
    int Number1;
    int ItemTid2;
    int Number2;

    // 非序列化字段
};

