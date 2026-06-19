#include "ShopsRes.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

bool MallMonthlyCardRes::LoadFromPb(std::string data)
{
    MallMonthlyCard mmc;
    if (!mmc.ParseFromString(data)) {
        return false;
    }

    Id = mmc.id();
    MonthlyCardId = mmc.monthlycardid();
    Price = mmc.price();
    BaseItemId = mmc.baseitemid();
    BaseItemQty = mmc.baseitemqty();
    MaxDays = mmc.maxdays();
    Products.Add(BaseItemId, BaseItemQty);

    return true;
}

void MallMonthlyCardRes::OnLoad()
{
}

bool MonthlyCardRes::LoadFromPb(std::string data)
{
    MonthlyCard monthlyCard;
    if (!monthlyCard.ParseFromString(data)) {
        return false;
    }

    Id = monthlyCard.id();
    CardId = monthlyCard.cardid();
    RewardId1 = monthlyCard.rewardid1();
    RewardNum1 = monthlyCard.rewardnum1();
    RewardId2 = monthlyCard.rewardid2();
    RewardNum2 = monthlyCard.rewardnum2();
    return true;
}

void MonthlyCardRes::OnLoad()
{
    Rewards.Add(RewardId1, RewardNum1);
    Rewards.Add(RewardId2, RewardNum2);
}

bool MallPackageRes::LoadFromPb(std::string data)
{
    MallPackage mp;
    if (!mp.ParseFromString(data)) {
        return false;
    }

    Id = mp.id();
    Stock = mp.stock();
    CurrencyType = mp.currencytype();
    CurrencyItemId = mp.currencyitemid();
    CurrencyItemQty = mp.currencyitemqty();
    Tag = mp.tag();
    RefreshType = mp.refreshtype();
    Items = mp.items();
    ListCondType = mp.listcondtype();
    ListCondParams = mp.listcondparams();
    OrderCondType = mp.ordercondtype();
    OrderCondParams = mp.ordercondparams();
    ListTime = mp.listtime();
    DeListTime = mp.delisttime();
    return true;
}

void MallPackageRes::OnLoad()
{
    Products = ItemParamMap::FromJsonString(Items);
    ListCond = ParseIntArrayText(ListCondParams);
    OrderCond = ParseIntArrayText(OrderCondParams);
    ListTimeSeconds = DateToSeconds(ListTime);
    DeListTimeSeconds = DateToSeconds(DeListTime);
}

bool MallShopRes::LoadFromPb(std::string data)
{
    MallShop ms;
    if (!ms.ParseFromString(data)) {
        return true;
    }

    Id = ms.id();
    Stock = ms.stock();
    ExchangeItemId = ms.exchangeitemid();
    ExchangeItemQty = ms.exchangeitemqty();
    ItemId = ms.itemid();
    ItemQty = ms.itemqty();
    RefreshType = ms.refreshtype();
    ListCondType = ms.listcondtype();
    ListCondParams = ms.listcondparams();
    OrderCondType = ms.ordercondtype();
    OrderCondParams = ms.ordercondparams();
    ListTime = ms.listtime();
    DeListTime = ms.delisttime();

    return true;
}

void MallShopRes::OnLoad()
{
    Products.Add(ItemId, ItemQty);
    ListCond = ParseIntArrayText(ListCondParams);
    OrderCond = ParseIntArrayText(OrderCondParams);
    ListTimeSeconds = DateToSeconds(ListTime);
    DeListTimeSeconds = DateToSeconds(DeListTime);
}

bool MallGemRes::LoadFromPb(std::string data)
{
    MallGem mg;
    if (!mg.ParseFromString(data)) {
        return false;
    }

    Id = mg.id();
    BaseItemId = mg.baseitemid();
    BaseItemQty = mg.baseitemqty();
    ExperiencedBonusItemId = mg.experiencedbonusitemid();
    ExperiencedBonusItemQty = mg.experiencedbonusitemqty();
    MaidenBonusItemID = mg.maidenbonusitemid();
    MaidenBonusItemQty = mg.maidenbonusitemqty();
    Price = mg.price();

    return true;
}

void MallGemRes::OnLoad()
{
}

bool ResidentShopRes::LoadFromPb(std::string data)
{
    ResidentShop rs;
    if (!rs.ParseFromString(data)) {
        return false;
    }
    Id = rs.id();
    RefreshTimeType = rs.refreshtimetype();
    RefreshInterval = rs.refreshinterval();
    OpenTime = rs.opentime();
    return true;
}

void ResidentShopRes::OnLoad()
{
    OpenTimeSeconds = DateToSeconds(OpenTime);
}

bool ResidentGoodsRes::LoadFromPb(std::string data)
{
    ResidentGoods rg;
    if (!rg.ParseFromString(data)) {
        return false;
    }

    Id = rg.id();
    ShopId = rg.shopid();
    MaximumLimit = rg.maximumlimit();
    ItemId = rg.itemid();
    ItemQuantity = rg.itemquantity();
    CurrencyItemId = rg.currencyitemid();
    Price = rg.price();
    AppearCondType = rg.appearcondtype();
    AppearCondParams = rg.appearcondparams();

    return true;
}

void ResidentGoodsRes::OnLoad()
{
    Products.Add(ItemId, ItemQuantity);
    AppearCond = ParseIntArrayText(AppearCondParams);
}

