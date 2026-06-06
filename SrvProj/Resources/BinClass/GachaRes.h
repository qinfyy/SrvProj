#pragma once

#include "../ResBase.h"
#include "../ResourceDerivedData.h"
#include <vector>
#include <memory>
#include <string>

class GachaATypeProbRes : public ResBase {
public:
    GachaATypeProbRes() = default;
    ~GachaATypeProbRes() = default;

    std::string GetId() const override { return std::to_string((Group << 16) | (Times & 0xFFFF)); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;
     
    // 序列化字段

    int Group;
    int Times;
    int Prob;

    // 非序列化字段
};

class GachaRes : public ResBase {
public:
    GachaRes() = default;
    ~GachaRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int StorageId;
    int GachaType;

    int GuaranteeTimes;
    int GuaranteeTid;
    int GuaranteeQty;
    int ATypeGuaranteeTimes;

    int SpecificTid;
    int SpecificQty;

    int FirstTenShow;
    std::string StartTime;
    std::string EndTime;

    // Packages
    int ATypePkg;
    int BTypePkg;
    int CTypePkg;

    int ATypeUpPkg;
    int BTypeUpPkg;

    int BGuaranteePkg;

    // 非序列化字段
};

class GachaNewbieRes : public ResBase {
public:
    GachaNewbieRes() = default;
    ~GachaNewbieRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    int SpinCount;
    int SaveCount;

    // 非序列化字段
};

class GachaStorageRes : public ResBase {
public:
    GachaStorageRes() = default;
    ~GachaStorageRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;

    int DefaultId;
    int DefaultQty;
    int CostId;
    int CostQty;

    int ATypeGroup;
    int AUpGuaranteeTimes;
    int ATypeUpProb;
    int ATypeUpShowProb;

    int BTypeProb;
    int BTypeUpProb;
    int BTypeUpShowProb;
    int BTypeGuaranteeProb;

    std::string GiveItems;

    // 非序列化字段
};

class GachaTypeRes : public ResBase {
public:
    GachaTypeRes() = default;
    ~GachaTypeRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    std::vector<int> CoinItem;

    // 非序列化字段
};
