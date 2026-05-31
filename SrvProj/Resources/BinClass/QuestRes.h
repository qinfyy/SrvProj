#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

class DailyQuestRes : public ResBase {
public:
    DailyQuestRes() = default;
    ~DailyQuestRes() = default;

    int GetId() const override { return Id; }
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

    int GetId() const override { return Id; }
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

    int GetId() const override { return Id; }
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

    int GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int Active;

    int ItemTid1;
    int Number1;
    int ItemTid2;
    int Number2;
};

