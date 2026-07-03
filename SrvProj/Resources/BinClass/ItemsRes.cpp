#include "ItemsRes.h"
#include "../ResourceJsonUtil.h"
#include "../../proto/table_cpp/client_table.pb.h"

using namespace nova::client;

bool ItemRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "Title", Title);
    ReadResourceJsonField(data, "Type", Type);
    ReadResourceJsonField(data, "Stype", Stype);
    ReadResourceJsonField(data, "Rarity", Rarity);
    ReadResourceJsonField(data, "Stack", Stack);
    ReadResourceJsonField(data, "UseMode", UseMode);
    ReadResourceJsonField(data, "UseAction", UseAction);
    ReadResourceJsonField(data, "UseArgs", UseArgs);
    return true;
}

bool ItemRes::LoadFromPb(std::string data)
{
    Item i;
    if (!i.ParseFromString(data)) {
        return false;
    }

    Id = i.id();
    Title = i.title();
    Type = i.type();
    Stype = i.stype();
    Rarity = i.rarity();
    Stack = i.stack();
    UseMode = i.usemode();
    UseAction = i.useaction();
    UseArgs = i.useargs();

    return true;
}

bool ProductionRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "UnlockWorldLevel", UnlockWorldLevel);
    ReadResourceJsonField(data, "ProductionId", ProductionId);
    ReadResourceJsonField(data, "ProductionPerBatch", ProductionPerBatch);
    ReadResourceJsonField(data, "RawMaterialId1", RawMaterialId1);
    ReadResourceJsonField(data, "RawMaterialCount1", RawMaterialCount1);
    return true;
}

bool ProductionRes::LoadFromPb(std::string data)
{
    Production p;
    if (!p.ParseFromString(data)) {
        return false;
    }

    Id = p.id();
    UnlockWorldLevel = p.unlockworldlevel();
    ProductionId = p.productionid();
    ProductionPerBatch = p.productionperbatch();
    RawMaterialId1 = p.rawmaterialid1();
    RawMaterialCount1 = p.rawmaterialcount1();


    return true;
}

bool PlayerHeadRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "HeadType", HeadType);
    ReadResourceJsonField(data, "UnlockChar", UnlockChar);
    ReadResourceJsonField(data, "UnlockSkin", UnlockSkin);
    return true;
}

bool PlayerHeadRes::LoadFromPb(std::string data)
{
    PlayerHead ph;
    if (!ph.ParseFromString(data)) {
        return false;
    }

    Id = ph.id();
    HeadType = ph.headtype();
    UnlockChar = ph.unlockchar();
    UnlockSkin = ph.unlockskin();

    return true;
}

bool TitleRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "ItemId", ItemId);
    ReadResourceJsonField(data, "TitleType", TitleType);
    return true;
}

bool TitleRes::LoadFromPb(std::string data)
{
    Title t;
    if (!t.ParseFromString(data)) {
        return false;
    }

    Id = t.id();
    ItemId = t.itemid();
    TitleType = t.titletype();

    return true;
}

bool HonorRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "Id", Id);
    ReadResourceJsonField(data, "Type", Type);
    ReadResourceJsonField(data, "Params", Params);
    return true;
}

bool HonorRes::LoadFromPb(std::string data)
{
    Honor h;
    if (!h.ParseFromString(data)) {
        return false;
    }

    Id = h.id();
    Type = h.type();
    Params.clear();
    for (const auto& param : h.params()) {
        Params.push_back(param);
    }


    return true;
}

bool DropPkgRes::LoadFromJson(const nlohmann::json& data)
{
    ReadResourceJsonField(data, "PkgId", PkgId);
    ReadResourceJsonField(data, "ItemId", ItemId);
    return true;
}

bool DropPkgRes::LoadFromPb(std::string data)
{
    DropPkg dp;
    if (!dp.ParseFromString(data)) {
        return false;
    }

    PkgId = dp.pkgid();
    ItemId = dp.itemid();

    return true;
}

