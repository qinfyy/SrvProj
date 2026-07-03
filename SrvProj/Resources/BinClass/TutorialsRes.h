#pragma once

#include <nlohmann/json_fwd.hpp>
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class TutorialLevelRes {
public:
    TutorialLevelRes() = default;
    ~TutorialLevelRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int WorldClass;
    int Item1;
    int Qty1;

    // 非序列化字段
};
