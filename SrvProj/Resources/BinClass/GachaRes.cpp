#include "GachaRes.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

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

