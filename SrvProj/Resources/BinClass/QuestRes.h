#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

class DailyQuestRes : public ResBase {
public:
    DailyQuestRes() = default;
    ~DailyQuestRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    bool Apear;
    int Active;
    int ItemTid;
    int ItemQty;

    int CompleteCond;
    int CompleteCondClient;
    std::string CompleteCondParams;
};

class DailyQuestActiveRes : public ResBase {
public:
    DailyQuestActiveRes() = default;
    ~DailyQuestActiveRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int Active;

    int ItemTid1;
    int Number1;
    int ItemTid2;
    int Number2;
};

class WeeklyQuestRes : public ResBase {
public:
    WeeklyQuestRes() = default;
    ~WeeklyQuestRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    bool Apear;
    int Active;
    int ItemTid;
    int ItemQty;

    int CompleteCond;
    int CompleteCondClient;
    std::string CompleteCondParams;
};

class WeeklyQuestActiveRes : public ResBase {
public:
    WeeklyQuestActiveRes() = default;
    ~WeeklyQuestActiveRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int Active;

    int ItemTid1;
    int Number1;
    int ItemTid2;
    int Number2;
};

