#pragma once

#include <nlohmann/json_fwd.hpp>
#include "../ResourceDerivedData.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <string>

class GachaATypeProbRes {
public:
    GachaATypeProbRes() = default;
    ~GachaATypeProbRes() = default;

    auto GetKey() const { return std::pair<int, int>{Group, Times}; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);
    static int GetProb(int group, int times, int defaultProb);
    static int GetMaxProb();

    // 序列化字段

    int Group;
    int Times;
    int Prob;

    // 非序列化字段
};

class GachaPkgRes {
public:
    GachaPkgRes() = default;
    ~GachaPkgRes() = default;

    auto GetKey() const { return std::pair<int, int>{PkgId, GoodsId}; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    static void ClearPackages();
    static const WeightedList<int>* GetPackageById(int packageId);

    // 序列化字段

    int PkgId;
    int GoodsId;
    int Weight;

    // 非序列化字段

    static std::unordered_map<int, WeightedList<int>> Packages;
};

class GachaRes {
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

    auto GetKey() const { return Id; }
    void OnLoad();
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);
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

class GachaNewbieRes {
public:
    GachaNewbieRes() = default;
    ~GachaNewbieRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    int SpinCount;
    int SaveCount;

    // 非序列化字段
};

class GachaStorageRes {
public:
    GachaStorageRes() = default;
    ~GachaStorageRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

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

class GachaTypeRes {
public:
    GachaTypeRes() = default;
    ~GachaTypeRes() = default;

    auto GetKey() const { return Id; }
    void OnLoad() {};
    bool LoadFromPb(std::string data);
    bool LoadFromJson(const nlohmann::json& data);

    // 序列化字段

    int Id;
    std::vector<int> CoinItem;

    // 非序列化字段

};
