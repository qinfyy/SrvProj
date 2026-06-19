#pragma once

#include "../ResBase.h"
#include "../ResourceDerivedData.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <string>

class GachaATypeProbRes : public ResBase {
public:
    GachaATypeProbRes() = default;
    ~GachaATypeProbRes() = default;

    std::string GetId() const override { return std::to_string((Group << 16) | (Times & 0xFFFF)); }
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;
    static int GetProb(int group, int times, int defaultProb);
    static int GetMaxProb();

    // 序列化字段

    int Group;
    int Times;
    int Prob;

    // 非序列化字段
};

class GachaPkgRes : public ResBase {
public:
    GachaPkgRes() = default;
    ~GachaPkgRes() = default;

    std::string GetId() const override { return std::to_string(PkgId) + ":" + std::to_string(GoodsId); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;

    static void ClearPackages();
    static const WeightedList<int>* GetPackageById(int packageId);

    // 序列化字段

    int PkgId;
    int GoodsId;
    int Weight;

    // 非序列化字段

    static std::unordered_map<int, WeightedList<int>> Packages;
};

class GachaRes : public ResBase {
public:
    enum class GachaPackageType {
        A,
        AUp,
        B,
        BUp,
        C
    };

    struct GachaPackage {
        GachaPackageType Type = GachaPackageType::C;
        int Id = 0;
    };

    GachaRes() = default;
    ~GachaRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override;
    bool LoadFromPb(std::string data) override;
    bool CanGuarantee() const { return GuaranteeTimes > 0; }
    bool IsActiveAt(long long now) const { return Valid && now >= StartTimeSeconds && now <= EndTimeSeconds; }
    bool ContainsAllowedCoinItem(int itemId) const { return AllowedCoinItems.find(itemId) != AllowedCoinItems.end(); }
    int GetDisplayAUpGuaranteeTimes() const;

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

    WeightedList<GachaPackage> PackageA;
    WeightedList<GachaPackage> PackageB;
    WeightedList<GachaPackage> PackageC;
    std::unordered_set<int> AllowedCoinItems;
    long long StartTimeSeconds = 0;
    long long EndTimeSeconds = 0;
    bool Valid = true;

private:
    bool HasValidPackage(int packageId) const;
    bool IsSpinConfigValid() const;
    void MarkInvalid(const std::string& message);
};

class GachaNewbieRes : public ResBase {
public:
    GachaNewbieRes() = default;
    ~GachaNewbieRes() = default;

    std::string GetId() const override { return std::to_string(Id); }
    void OnLoad() override {};
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
    void OnLoad() override {};
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
    void OnLoad() override {};
    bool LoadFromPb(std::string data) override;

    // 序列化字段

    int Id;
    std::vector<int> CoinItem;

    // 非序列化字段

};
