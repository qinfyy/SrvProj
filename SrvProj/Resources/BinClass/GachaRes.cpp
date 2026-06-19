#include "GachaRes.h"
#include "../GameData.h"
#include "../../GameConstants.h"
#include "../../Logger.h"
#include "../../Util.h"
#include "../../proto/table_cpp/client_table.pb.h"

#include <algorithm>
#include <limits>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

using namespace nova::client;

std::unordered_map<int, WeightedList<int>> GachaPkgRes::Packages;

int GachaATypeProbRes::GetProb(int group, int times, int defaultProb)
{
    const int key = (group << 16) | (times & 0xFFFF);
    const auto it = GameData::GachaATypeProbDataTable.find(std::to_string(key));
    if (it == GameData::GachaATypeProbDataTable.end())
    {
        return defaultProb;
    }

    return it->second.Prob;
}

int GachaATypeProbRes::GetMaxProb()
{
    int maxProb = 0;
    for (const auto& [_, data] : GameData::GachaATypeProbDataTable)
    {
        maxProb = (std::max)(maxProb, data.Prob);
    }

    return maxProb;
}

bool GachaATypeProbRes::LoadFromPb(std::string data)
{
    GachaATypeProb gachaProb;
    if (!gachaProb.ParseFromString(data)) {
        return false;
    }

    Group = gachaProb.group();
    Times = gachaProb.times();
    Prob = gachaProb.prob();

    return true;
}

void GachaPkgRes::ClearPackages()
{
    Packages.clear();
}

const WeightedList<int>* GachaPkgRes::GetPackageById(int packageId)
{
    const auto it = Packages.find(packageId);
    if (it == Packages.end())
    {
        return nullptr;
    }

    return &it->second;
}

void GachaPkgRes::OnLoad()
{
    Packages[PkgId].Add(Weight, GoodsId);
}

bool GachaPkgRes::LoadFromPb(std::string data)
{
    GachaPkg gachaPkg;
    if (!gachaPkg.ParseFromString(data)) {
        return false;
    }

    PkgId = gachaPkg.pkgid();
    GoodsId = gachaPkg.goodsid();
    Weight = gachaPkg.weight();

    return true;
}

bool GachaRes::HasValidPackage(int packageId) const
{
    if (packageId <= 0)
    {
        return false;
    }

    const auto* package = GachaPkgRes::GetPackageById(packageId);
    return package != nullptr && !package->Empty();
}

bool GachaRes::IsSpinConfigValid() const
{
    const bool hasValidA = (ATypePkg > 0 && HasValidPackage(ATypePkg)) ||
        (ATypeUpPkg > 0 && HasValidPackage(ATypeUpPkg));
    const bool hasValidB = (BTypePkg > 0 && HasValidPackage(BTypePkg)) ||
        (BGuaranteePkg > 0 && HasValidPackage(BGuaranteePkg)) ||
        (BTypeUpPkg > 0 && HasValidPackage(BTypeUpPkg));
    const bool hasValidC = CTypePkg > 0 && HasValidPackage(CTypePkg);

    return !PackageA.Empty() && !PackageB.Empty() && !PackageC.Empty()
        && hasValidA && hasValidB && hasValidC;
}

void GachaRes::MarkInvalid(const std::string& message)
{
    Valid = false;
    StartTimeSeconds = 1;
    EndTimeSeconds = 0;
    LOG_ERROR("{} banner: {}, {}", U8("抽卡配置无效"), Id, message);
}

int GachaRes::GetDisplayAUpGuaranteeTimes() const
{
    if (ATypeGuaranteeTimes > 0)
    {
        return ATypeGuaranteeTimes;
    }

    const auto it = GameData::GachaStorageDataTable.find(std::to_string(StorageId));
    if (it == GameData::GachaStorageDataTable.end())
    {
        return 0;
    }

    return it->second.AUpGuaranteeTimes;
}

void GachaRes::OnLoad()
{
    Valid = true;
    StartTimeSeconds = 0;
    EndTimeSeconds = std::numeric_limits<long long>::max();
    PackageA.Clear();
    PackageB.Clear();
    PackageC.Clear();
    AllowedCoinItems.clear();

    const auto typeIt = GameData::GachaTypeDataTable.find(std::to_string(GachaType));
    if (typeIt == GameData::GachaTypeDataTable.end())
    {
        MarkInvalid("invalid GachaType " + std::to_string(GachaType));
        return;
    }

    for (int itemId : typeIt->second.CoinItem)
    {
        AllowedCoinItems.insert(itemId);
    }

    if (!StartTime.empty())
    {
        StartTimeSeconds = DateToSeconds(StartTime);
    }
    if (!EndTime.empty())
    {
        EndTimeSeconds = DateToSeconds(EndTime);
    }

    const auto storageIt = GameData::GachaStorageDataTable.find(std::to_string(StorageId));
    if (storageIt == GameData::GachaStorageDataTable.end())
    {
        MarkInvalid("invalid StorageId " + std::to_string(StorageId));
        return;
    }

    const auto& storage = storageIt->second;
    const bool isNewbieBanner = GameData::GachaNewbieDataTable.find(std::to_string(Id)) != GameData::GachaNewbieDataTable.end();
    if (!isNewbieBanner && !ContainsAllowedCoinItem(storage.CostId))
    {
        MarkInvalid("CostId not allowed by GachaType: " + std::to_string(storage.CostId));
        return;
    }

    if (ATypePkg > 0)
    {
        PackageA.Add(GameConstants::GachaProbabilityBase - storage.ATypeUpProb, { GachaPackageType::A, ATypePkg });
    }
    if (ATypeUpPkg > 0)
    {
        PackageA.Add(storage.ATypeUpProb, { GachaPackageType::AUp, ATypeUpPkg });
    }

    if (BTypePkg > 0)
    {
        PackageB.Add(storage.BTypeProb, { GachaPackageType::B, BTypePkg });
    }
    if (BGuaranteePkg > 0)
    {
        PackageB.Add(storage.BTypeGuaranteeProb, { GachaPackageType::B, BGuaranteePkg });
    }
    if (BTypeUpPkg > 0)
    {
        PackageB.Add(storage.BTypeUpProb, { GachaPackageType::BUp, BTypeUpPkg });
    }

    if (CTypePkg > 0)
    {
        PackageC.Add(GameConstants::GachaProbabilityBase, { GachaPackageType::C, CTypePkg });
    }

    if (!IsSpinConfigValid())
    {
        MarkInvalid("spin package/probability composition is invalid");
    }
}

bool GachaRes::LoadFromPb(std::string data)
{
    Gacha gacha;
    if (!gacha.ParseFromString(data)) {
        return false;
    }

    Id = gacha.id();
    StorageId = gacha.storageid();
    GachaType = gacha.gachatype();
    GuaranteeTimes = gacha.guaranteetimes();
    GuaranteeTid = gacha.guaranteetid();
    GuaranteeQty = gacha.guaranteeqty();
    ATypeGuaranteeTimes = gacha.atypeguaranteetimes();
    SpecificTid = gacha.specifictid();
    SpecificQty = gacha.specificqty();
    FirstTenShow = gacha.firsttenshow();
    StartTime = gacha.starttime();
    EndTime = gacha.endtime();
    ATypePkg = gacha.atypepkg();
    BTypePkg = gacha.btypepkg();
    CTypePkg = gacha.ctypepkg();
    ATypeUpPkg = gacha.atypeuppkg();
    BTypeUpPkg = gacha.btypeuppkg();
    BGuaranteePkg = gacha.bguaranteepkg();

    return true;
}

bool GachaNewbieRes::LoadFromPb(std::string data)
{
    GachaNewbie gachaNewbie;
    if (!gachaNewbie.ParseFromString(data)) {
        return false;
    }

    Id = gachaNewbie.id();
    SpinCount = gachaNewbie.spincount();
    SaveCount = gachaNewbie.savecount();

    return true;
}

bool GachaStorageRes::LoadFromPb(std::string data)
{
    GachaStorage gachaStorage;
    if (!gachaStorage.ParseFromString(data)) {
        return false;
    }

    Id = gachaStorage.id();
    DefaultId = gachaStorage.defaultid();
    DefaultQty = gachaStorage.defaultqty();
    CostId = gachaStorage.costid();
    CostQty = gachaStorage.costqty();
    ATypeGroup = gachaStorage.atypegroup();
    AUpGuaranteeTimes = gachaStorage.aupguaranteetimes();
    ATypeUpProb = gachaStorage.atypeupprob();
    ATypeUpShowProb = gachaStorage.atypeupshowprob();
    BTypeProb = gachaStorage.btypeprob();
    BTypeUpProb = gachaStorage.btypeupprob();
    BTypeUpShowProb = gachaStorage.btypeupshowprob();
    BTypeGuaranteeProb = gachaStorage.btypeguaranteeprob();
    GiveItems = gachaStorage.giveitems();

    return true;
}

bool GachaTypeRes::LoadFromPb(std::string data)
{
    GachaType gachaType;
    if (!gachaType.ParseFromString(data)) {
        return false;
    }

    Id = gachaType.id();
    CoinItem.clear();
    for (int itemId : gachaType.coinitem()) {
        CoinItem.push_back(itemId);
    }

    return true;
}

