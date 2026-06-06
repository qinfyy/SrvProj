#pragma once

#include "../ResBase.h"
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class TutorialLevelRes : public ResBase {
public:
    TutorialLevelRes() = default;
    ~TutorialLevelRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int WorldClass;
    int Item1;
    int Qty1;

    // 非序列化字段
};