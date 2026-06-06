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

    return true;
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
    Items = mp.items();
    return true;
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

    return true;
}

bool MallGemRes::LoadFromPb(std::string data)
{
    MallGem mg;
    if (!mg.ParseFromString(data)) {
        return false;
    }

    Id = mg.id();
    //Stock = mg.stock();
    //ItemId = mg.itemid();
    //CurrencyItemId = mg.currencyitemid();
    //ItemQty = mg.itemqty();

    return true;
}

bool ResidentShopRes::LoadFromPb(std::string data)
{
    ResidentShop rs;
    if (!rs.ParseFromString(data)) {
        return false;
    }
    Id = rs.id();
    return true;
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

    return true;
}

