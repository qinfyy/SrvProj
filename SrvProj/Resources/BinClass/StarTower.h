#pragma once

#include "../ResBase.h"
#include <vector>
#include <memory>
#include <string>

class StarTowerGrowthNodeRes : public ResBase {
public:
    StarTowerGrowthNodeRes() = default;
    ~StarTowerGrowthNodeRes() = default;

    int GetId() const override { return Id; }
    void OnLoad() override {}
    bool LoadFromPb(std::string data) override;

    int Id;
    int NodeId;
    int Group;

    int ItemId1;
    int ItemQty1;

};
