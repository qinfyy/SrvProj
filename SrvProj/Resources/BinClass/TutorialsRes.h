#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

class TutorialLevelRes : public ResBase {
public:
    TutorialLevelRes() = default;
    ~TutorialLevelRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int WorldClass;
    int Item1;
    int Qty1;

};