#include "InventoryMgr.h"

#include "ChangeInfoUtil.h"
#include "CharacterMgr.h"
#include "Player.h"
#include "../GameConstants.h"
#include "../Resources/BinClass/CharacterRes.h"
#include "../Resources/BinClass/DiscRes.h"
#include "../Resources/BinClass/ItemsRes.h"
#include "../Resources/BinClass/ShopsRes.h"
#include "../Resources/GameData.h"
#include "../proto/NetMsgId.pb.h"
#include "../proto/proto_cpp/notify.pb.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <string>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

void InventoryMgr::OnCreate()
{
    MutableBin()->clear_items();
    MutableBin()->clear_resources();
    MutableBin()->clear_skins();
    MutableBin()->clear_headicons();
    MutableBin()->clear_titles();
    MutableBin()->clear_honors();
    MutableBin()->clear_shopbuycount();
    MutableBin()->clear_mallbuycount();
    MutableBin()->clear_mallpackagebuycount();
    MutableBin()->clear_gemmaidenclaimed();
    MutableBin()->clear_monthlycardbuycount();

    EnsureDefaultResources();
}

void InventoryMgr::OnLoad()
{
    EnsureDefaultResources();
}

ServerProto::InventoryCompBin* InventoryMgr::MutableBin()
{
    return GetPlayer()->SaveData().mutable_inventorycomp();
}

const ServerProto::InventoryCompBin& InventoryMgr::Bin() const
{
    return GetPlayer()->SaveData().inventorycomp();
}

bool InventoryMgr::ContainsRepeated(const google::protobuf::RepeatedField<uint32_t>& values, uint32_t value) const
{
    return std::find(values.begin(), values.end(), value) != values.end();
}

void InventoryMgr::EnsureDefaultResources()
{
    auto* resources = MutableBin()->mutable_resources();

    auto ensure = [resources](uint32_t tid, int64_t qty) {
        auto it = resources->find(tid);
        if (it == resources->end() || it->second < qty)
        {
            (*resources)[tid] = qty;
        }
    };

    ensure(GameConstants::GoldItemId, 1000000);
    ensure(GameConstants::StellaniteDustItemId, 30000);
    ensure(GameConstants::FreeStellaniteLuminaItemId, 30000);
    ensure(GameConstants::JointDrillTicketId, 3);
    ensure(GameConstants::WeeklyEntryItemId, 3);

    auto* bin = MutableBin();
    if (!ContainsRepeated(bin->headicons(), 101))
    {
        bin->add_headicons(101);
    }
    if (!ContainsRepeated(bin->headicons(), 102))
    {
        bin->add_headicons(102);
    }
    if (!ContainsRepeated(bin->titles(), 1))
    {
        bin->add_titles(1);
    }
    if (!ContainsRepeated(bin->titles(), 2))
    {
        bin->add_titles(2);
    }
    if (!ContainsRepeated(bin->honors(), GameConstants::DefaultHonorId))
    {
        bin->add_honors(GameConstants::DefaultHonorId);
    }
}

bool InventoryMgr::AddItem(uint32_t tid, int64_t qty, proto::ChangeInfo* change)
{
    if (tid == 0 || qty == 0)
    {
        return false;
    }

    if (qty < 0)
    {
        return ConsumeItem(tid, -qty, change);
    }

    const int type = ChangeInfoUtil::GetItemType(tid);
    if (type == 0)
    {
        return false;
    }

    if (type == ChangeInfoUtil::ItemType::Res)
    {
        auto* resources = MutableBin()->mutable_resources();
        (*resources)[tid] += qty;
        if (change)
        {
            ChangeInfoUtil::AddItemOrRes(*change, tid, qty);
        }
        GetPlayer()->Trigger(48, static_cast<uint32_t>((std::min)(qty, static_cast<int64_t>(std::numeric_limits<uint32_t>::max()))), tid, 0);
        return true;
    }

    if (type == ChangeInfoUtil::ItemType::Item)
    {
        const auto itemIt = GameData::ItemDataTable.find(std::to_string(tid));
        if (itemIt != GameData::ItemDataTable.end() &&
            itemIt->second.Stype == ChangeInfoUtil::ItemSubType::RandomPackage &&
            !itemIt->second.UseArgs.empty())
        {
            ItemParamMap packages = ItemParamMap::FromJsonString(itemIt->second.UseArgs);
            bool changed = false;
            for (const auto& [pkgId, pkgCount] : packages.Items)
            {
                const int total = pkgCount * static_cast<int>(qty);
                for (int i = 0; i < total; ++i)
                {
                    const auto dropIt = GameData::DropPkgDataTable.find(std::to_string(pkgId));
                    if (dropIt != GameData::DropPkgDataTable.end())
                    {
                        changed = AddItem(static_cast<uint32_t>(dropIt->second.ItemId), 1, change) || changed;
                    }
                }
            }
            return changed;
        }

        auto* items = MutableBin()->mutable_items();
        (*items)[tid] += qty;
        if (change)
        {
            ChangeInfoUtil::AddItemOrRes(*change, tid, qty);
        }
        GetPlayer()->Trigger(48, static_cast<uint32_t>((std::min)(qty, static_cast<int64_t>(std::numeric_limits<uint32_t>::max()))), tid, 0);
        return true;
    }

    if (type == ChangeInfoUtil::ItemType::Char)
    {
        bool changed = false;
        for (int64_t i = 0; i < qty; ++i)
        {
            if (GetPlayer()->Characters().HasCharacter(static_cast<int>(tid)))
            {
                const auto charIt = GameData::CharacterDataTable.find(std::to_string(tid));
                if (charIt != GameData::CharacterDataTable.end())
                {
                    changed = AddItem(static_cast<uint32_t>(charIt->second.FragmentsId), charIt->second.TransformQty, change) || changed;
                }
                continue;
            }

            auto* character = GetPlayer()->Characters().AddCharacterFromId(static_cast<int>(tid));
            if (!character)
            {
                continue;
            }

            if (change)
            {
                GetPlayer()->Characters().AddCharacterChange(*change, *character);
            }
            changed = true;
        }
        return changed;
    }

    if (type == ChangeInfoUtil::ItemType::Disc)
    {
        bool changed = false;
        for (int64_t i = 0; i < qty; ++i)
        {
            if (GetPlayer()->Characters().HasDisc(static_cast<int>(tid)))
            {
                const auto discIt = GameData::DiscDataTable.find(std::to_string(tid));
                if (discIt != GameData::DiscDataTable.end())
                {
                    changed = AddItem(static_cast<uint32_t>(discIt->second.TransformItemId), 1, change) || changed;
                }
                continue;
            }

            auto* disc = GetPlayer()->Characters().AddDiscFromId(static_cast<int>(tid));
            if (!disc)
            {
                continue;
            }

            if (change)
            {
                GetPlayer()->Characters().AddDiscChange(*change, *disc);
            }
            changed = true;
        }
        return changed;
    }

    if (type == ChangeInfoUtil::ItemType::Energy)
    {
        if (!GetPlayer()->AddEnergy(ChangeInfoUtil::ClampQty(qty)))
        {
            return false;
        }
        if (change)
        {
            ChangeInfoUtil::AddProp(*change, GetPlayer()->GetEnergyProto());
        }
        return true;
    }

    if (type == ChangeInfoUtil::ItemType::CharacterSkin)
    {
        return AddSkin(tid, change);
    }

    if (type == ChangeInfoUtil::ItemType::Title)
    {
        return AddTitle(tid);
    }

    if (type == ChangeInfoUtil::ItemType::Honor || type == ChangeInfoUtil::ItemType::LevelHonor)
    {
        return AddHonor(tid);
    }

    if (type == ChangeInfoUtil::ItemType::HeadItem)
    {
        return AddHeadIcon(tid);
    }

    auto* items = MutableBin()->mutable_items();
    (*items)[tid] += qty;
    if (change)
    {
        ChangeInfoUtil::AddItemOrRes(*change, tid, qty);
    }
    return true;
}

bool InventoryMgr::AddItems(const std::vector<std::pair<uint32_t, int64_t>>& items, proto::ChangeInfo* change)
{
    bool changed = false;
    for (const auto& [tid, qty] : items)
    {
        changed = AddItem(tid, qty, change) || changed;
    }

    return changed;
}

bool InventoryMgr::AddItems(const ItemParamMap& items, proto::ChangeInfo* change)
{
    bool changed = false;
    for (const auto& [tid, qty] : items.Items)
    {
        changed = AddItem(static_cast<uint32_t>(tid), qty, change) || changed;
    }

    return changed;
}

bool InventoryMgr::ConsumeItem(uint32_t tid, int64_t qty, proto::ChangeInfo* change)
{
    if (tid == 0 || qty <= 0)
    {
        return false;
    }

    const int type = ChangeInfoUtil::GetItemType(tid);
    if (type == ChangeInfoUtil::ItemType::Res)
    {
        auto* resources = MutableBin()->mutable_resources();
        auto it = resources->find(tid);
        if (it == resources->end() || it->second < qty)
        {
            return false;
        }

        it->second -= qty;
        if (it->second <= 0)
        {
            resources->erase(tid);
        }
        if (change)
        {
            ChangeInfoUtil::AddItemOrRes(*change, tid, -qty);
        }
        GetPlayer()->Trigger(49, static_cast<uint32_t>((std::min)(qty, static_cast<int64_t>(std::numeric_limits<uint32_t>::max()))), tid, 0);
        return true;
    }

    if (type == ChangeInfoUtil::ItemType::Item || type == ChangeInfoUtil::ItemType::RogueItem ||
        type == ChangeInfoUtil::ItemType::Equipment || type == 0)
    {
        auto* items = MutableBin()->mutable_items();
        auto it = items->find(tid);
        if (it == items->end() || it->second < qty)
        {
            return false;
        }

        it->second -= qty;
        if (it->second <= 0)
        {
            items->erase(tid);
        }
        if (change)
        {
            ChangeInfoUtil::AddItemOrRes(*change, tid, -qty);
        }
        GetPlayer()->Trigger(49, static_cast<uint32_t>((std::min)(qty, static_cast<int64_t>(std::numeric_limits<uint32_t>::max()))), tid, 0);
        return true;
    }

    if (type == ChangeInfoUtil::ItemType::Energy)
    {
        if (!GetPlayer()->ConsumeEnergy(ChangeInfoUtil::ClampQty(qty)))
        {
            return false;
        }
        if (change)
        {
            ChangeInfoUtil::AddProp(*change, GetPlayer()->GetEnergyProto());
        }
        return true;
    }

    return false;
}

bool InventoryMgr::RemoveItem(uint32_t tid, int64_t qty, proto::ChangeInfo* change)
{
    return ConsumeItem(tid, qty, change);
}

bool InventoryMgr::RemoveItems(const ItemParamMap& items, proto::ChangeInfo* change)
{
    if (!HasItems(items))
    {
        return false;
    }

    bool changed = false;
    for (const auto& [tid, qty] : items.Items)
    {
        changed = RemoveItem(static_cast<uint32_t>(tid), qty, change) || changed;
    }

    return changed;
}

bool InventoryMgr::HasItem(uint32_t tid, int64_t qty) const
{
    if (qty == 0)
    {
        return true;
    }
    if (tid == 0 || qty < 0)
    {
        return false;
    }

    const int type = ChangeInfoUtil::GetItemType(tid);
    if (type == ChangeInfoUtil::ItemType::Res)
    {
        return GetResourceCount(tid) >= qty;
    }
    if (type == ChangeInfoUtil::ItemType::Item || type == ChangeInfoUtil::ItemType::RogueItem ||
        type == ChangeInfoUtil::ItemType::Equipment || type == 0)
    {
        const auto& items = Bin().items();
        const auto it = items.find(tid);
        return it != items.end() && it->second >= qty;
    }
    if (type == ChangeInfoUtil::ItemType::Char)
    {
        return qty <= 1 && GetPlayer()->Characters().HasCharacter(static_cast<int>(tid));
    }
    if (type == ChangeInfoUtil::ItemType::Disc)
    {
        return qty <= 1 && GetPlayer()->Characters().HasDisc(static_cast<int>(tid));
    }
    if (type == ChangeInfoUtil::ItemType::CharacterSkin)
    {
        return ContainsRepeated(Bin().skins(), tid);
    }
    if (type == ChangeInfoUtil::ItemType::Title)
    {
        return ContainsRepeated(Bin().titles(), tid);
    }
    if (type == ChangeInfoUtil::ItemType::Honor || type == ChangeInfoUtil::ItemType::LevelHonor)
    {
        return ContainsRepeated(Bin().honors(), tid);
    }
    if (type == ChangeInfoUtil::ItemType::HeadItem)
    {
        return ContainsRepeated(Bin().headicons(), tid);
    }
    if (type == ChangeInfoUtil::ItemType::Energy)
    {
        return GetPlayer()->GetEnergy() >= qty;
    }

    return false;
}

bool InventoryMgr::HasItems(const ItemParamMap& items) const
{
    for (const auto& [tid, qty] : items.Items)
    {
        if (!HasItem(static_cast<uint32_t>(tid), qty))
        {
            return false;
        }
    }

    return true;
}

int64_t InventoryMgr::GetItemCount(uint32_t tid) const
{
    const auto& items = Bin().items();
    if (const auto it = items.find(tid); it != items.end())
    {
        return it->second;
    }

    const auto& resources = Bin().resources();
    if (const auto it = resources.find(tid); it != resources.end())
    {
        return it->second;
    }

    return 0;
}

int64_t InventoryMgr::GetResourceCount(uint32_t tid) const
{
    const auto& resources = Bin().resources();
    if (const auto it = resources.find(tid); it != resources.end())
    {
        return it->second;
    }

    return 0;
}

bool InventoryMgr::AddSkin(uint32_t id, proto::ChangeInfo* change)
{
    if (id == 0 || ContainsRepeated(Bin().skins(), id))
    {
        return false;
    }

    MutableBin()->add_skins(id);
    if (change)
    {
        proto::Skin skin;
        skin.mutable_new_()->set_value(id);
        ChangeInfoUtil::AddProp(*change, skin);
    }

    proto::Skin notify;
    notify.mutable_new_()->set_value(id);
    GetPlayer()->PushNextPackage(character_skin_gain_notify, notify);
    GetPlayer()->Trigger(61, 1, id, 0);
    return true;
}

bool InventoryMgr::AddHeadIcon(uint32_t id)
{
    if (id == 0 || ContainsRepeated(Bin().headicons(), id))
    {
        return false;
    }

    MutableBin()->add_headicons(id);
    return true;
}

bool InventoryMgr::AddTitle(uint32_t id)
{
    if (id == 0 || ContainsRepeated(Bin().titles(), id))
    {
        return false;
    }

    MutableBin()->add_titles(id);
    return true;
}

bool InventoryMgr::AddHonor(uint32_t id)
{
    if (id == 0 || ContainsRepeated(Bin().honors(), id))
    {
        return false;
    }

    MutableBin()->add_honors(id);
    return true;
}

bool InventoryMgr::BuyItem(uint32_t currencyId, int64_t currencyCount, const ItemParamMap& products, uint32_t buyCount, proto::ChangeInfo& change)
{
    if (buyCount == 0)
    {
        return false;
    }

    const int64_t cost = currencyCount * static_cast<int64_t>(buyCount);
    if (currencyId > 0 && cost > 0 && !HasItem(currencyId, cost))
    {
        return false;
    }

    if (currencyId > 0 && cost > 0 && !RemoveItem(currencyId, cost, &change))
    {
        return false;
    }

    ItemParamMap multiplied;
    for (const auto& [tid, qty] : products.Items)
    {
        multiplied.Add(tid, qty * static_cast<int>(buyCount));
    }

    AddItems(multiplied, &change);
    return true;
}

bool InventoryMgr::ConvertStellaniteLuminaToDust(uint32_t qty, proto::ChangeInfo& change)
{
    if (qty == 0)
    {
        return false;
    }

    if (!HasMallPackageCurrency(GameConstants::PaidStellaniteLuminaItemId, qty))
    {
        return false;
    }

    if (!ConsumeMallPackageCurrency(GameConstants::PaidStellaniteLuminaItemId, qty, change))
    {
        return false;
    }

    return AddItem(GameConstants::StellaniteDustItemId, qty, &change);
}

bool InventoryMgr::UseItem(uint32_t id, uint32_t count, uint32_t selectId, proto::ChangeInfo& change)
{
    if (id == 0)
    {
        return false;
    }

    const uint32_t safeCount = (std::max)(count, 1u);
    const auto itemIt = GameData::ItemDataTable.find(std::to_string(id));
    if (itemIt == GameData::ItemDataTable.end() || itemIt->second.UseArgs.empty())
    {
        return false;
    }
    if (!HasItem(id, safeCount))
    {
        return false;
    }

    ItemParamMap params = ItemParamMap::FromJsonString(itemIt->second.UseArgs);
    ItemParamMap rewards;
    if (itemIt->second.UseAction == 2)
    {
        for (const auto& [tid, qty] : params.Items)
        {
            rewards.Add(tid, qty * static_cast<int>(safeCount));
        }
    }
    else if (itemIt->second.UseAction == 3)
    {
        const auto it = params.Items.find(static_cast<int>(selectId));
        if (it == params.Items.end() || it->second <= 0)
        {
            return false;
        }
        rewards.Add(static_cast<int>(selectId), it->second * static_cast<int>(safeCount));
    }
    else
    {
        return false;
    }

    if (!RemoveItem(id, safeCount, &change))
    {
        return false;
    }
    AddItems(rewards, &change);
    return true;
}

bool InventoryMgr::Produce(uint32_t id, uint32_t count, proto::ChangeInfo& change)
{
    if (id == 0 || count == 0)
    {
        return false;
    }

    const auto dataIt = GameData::ProductionDataTable.find(std::to_string(id));
    if (dataIt == GameData::ProductionDataTable.end())
    {
        return false;
    }

    ItemParamMap cost;
    cost.Add(dataIt->second.RawMaterialId1, dataIt->second.RawMaterialCount1 * static_cast<int>(count));
    if (!RemoveItems(cost, &change))
    {
        return false;
    }

    AddItem(static_cast<uint32_t>(dataIt->second.ProductionId), static_cast<int64_t>(dataIt->second.ProductionPerBatch) * count, &change);
    GetPlayer()->Trigger(73, count, 0, 0);
    return true;
}

bool InventoryMgr::BuyMallPackage(const MallPackageRes& data, proto::ChangeInfo& change)
{
    if (data.CurrencyType == GameConstants::CurrencyTypeItem &&
        data.CurrencyItemId > 0 &&
        data.CurrencyItemQty > 0 &&
        !ConsumeMallPackageCurrency(static_cast<uint32_t>(data.CurrencyItemId), data.CurrencyItemQty, change))
    {
        return false;
    }

    if (!AddItems(data.Products, &change))
    {
        return false;
    }

    AddMallPackagePurchaseCount(data.Id, 1);
    return true;
}

bool InventoryMgr::BuyMallGem(const MallGemRes& data, proto::ChangeInfo& change)
{
    ItemParamMap products;
    products.Add(data.BaseItemId, data.BaseItemQty);
    if (HasMallGemMaidenBonus(data.Id))
    {
        products.Add(data.MaidenBonusItemID, data.MaidenBonusItemQty);
    }
    else
    {
        products.Add(data.ExperiencedBonusItemId, data.ExperiencedBonusItemQty);
    }

    if (!AddItems(products, &change))
    {
        return false;
    }

    SetMallGemMaidenClaimed(data.Id, true);
    return true;
}

bool InventoryMgr::BuyMallMonthlyCard(const MallMonthlyCardRes& data, proto::ChangeInfo& change)
{
    if (!AddItems(data.Products, &change))
    {
        return false;
    }

    AddMallMonthlyCardPurchaseCount(data.Id, 1);
    return true;
}

bool InventoryMgr::BuyMallShopItem(const MallShopRes& data, uint32_t buyCount, proto::ChangeInfo& change)
{
    if (buyCount == 0)
    {
        return false;
    }

    ItemParamMap normalizedProducts;
    if (!data.Products.Items.empty())
    {
        for (const auto& [tid, qty] : data.Products.Items)
        {
            if (tid <= 0 || qty <= 0)
            {
                continue;
            }

            int perUnitQty = qty;
            if (data.ItemId > 0 && tid == data.ItemId && data.ItemQty > 0 && qty >= data.ItemQty && qty % data.ItemQty == 0)
            {
                perUnitQty = qty / data.ItemQty;
            }
            normalizedProducts.Add(tid, perUnitQty);
        }
    }
    else if (data.ItemId > 0 && data.ItemQty > 0)
    {
        normalizedProducts.Add(data.ItemId, 1);
    }

    if (normalizedProducts.Items.empty())
    {
        return false;
    }

    if (!BuyItem(static_cast<uint32_t>(data.ExchangeItemId), data.ExchangeItemQty, normalizedProducts, buyCount, change))
    {
        return false;
    }

    AddMallShopPurchaseCount(data.Id, buyCount);
    return true;
}

uint32_t InventoryMgr::GetShopPurchaseCount(uint32_t id) const
{
    const auto& counters = Bin().shopbuycount();
    const auto it = counters.find(id);
    return it == counters.end() ? 0 : it->second;
}

void InventoryMgr::AddShopPurchaseCount(uint32_t id, uint32_t count)
{
    if (id == 0 || count == 0)
    {
        return;
    }

    (*MutableBin()->mutable_shopbuycount())[id] += count;
}

uint32_t InventoryMgr::GetStringCounter(const google::protobuf::Map<std::string, uint32_t>& counters, const std::string& key) const
{
    if (key.empty())
    {
        return 0;
    }

    const auto it = counters.find(key);
    return it == counters.end() ? 0 : it->second;
}

void InventoryMgr::AddStringCounter(google::protobuf::Map<std::string, uint32_t>* counters, const std::string& key, uint32_t count)
{
    if (!counters || key.empty() || count == 0)
    {
        return;
    }

    (*counters)[key] += count;
}

uint32_t InventoryMgr::GetMallShopPurchaseCount(const std::string& id) const
{
    return GetStringCounter(Bin().mallbuycount(), id);
}

void InventoryMgr::AddMallShopPurchaseCount(const std::string& id, uint32_t count)
{
    AddStringCounter(MutableBin()->mutable_mallbuycount(), id, count);
}

uint32_t InventoryMgr::GetMallPackagePurchaseCount(const std::string& id) const
{
    return GetStringCounter(Bin().mallpackagebuycount(), id);
}

void InventoryMgr::AddMallPackagePurchaseCount(const std::string& id, uint32_t count)
{
    AddStringCounter(MutableBin()->mutable_mallpackagebuycount(), id, count);
}

uint32_t InventoryMgr::GetMallMonthlyCardPurchaseCount(const std::string& id) const
{
    return GetStringCounter(Bin().monthlycardbuycount(), id);
}

void InventoryMgr::AddMallMonthlyCardPurchaseCount(const std::string& id, uint32_t count)
{
    AddStringCounter(MutableBin()->mutable_monthlycardbuycount(), id, count);
}

bool InventoryMgr::HasMallGemMaidenBonus(const std::string& id) const
{
    return GetStringCounter(Bin().gemmaidenclaimed(), id) == 0;
}

void InventoryMgr::SetMallGemMaidenClaimed(const std::string& id, bool claimed)
{
    if (id.empty())
    {
        return;
    }

    auto* counters = MutableBin()->mutable_gemmaidenclaimed();
    if (claimed)
    {
        (*counters)[id] = 1;
    }
    else
    {
        counters->erase(id);
    }
}

bool InventoryMgr::HasMallPackageCurrency(uint32_t currencyId, int64_t qty) const
{
    if (qty <= 0)
    {
        return true;
    }

    if (currencyId == GameConstants::PaidStellaniteLuminaItemId || currencyId == GameConstants::FreeStellaniteLuminaItemId)
    {
        return GetResourceCount(GameConstants::PaidStellaniteLuminaItemId) + GetResourceCount(GameConstants::FreeStellaniteLuminaItemId) >= qty;
    }

    return HasItem(currencyId, qty);
}

bool InventoryMgr::ConsumeMallPackageCurrency(uint32_t currencyId, int64_t qty, proto::ChangeInfo& change)
{
    if (qty <= 0)
    {
        return true;
    }

    if (!HasMallPackageCurrency(currencyId, qty))
    {
        return false;
    }

    if (currencyId == GameConstants::PaidStellaniteLuminaItemId || currencyId == GameConstants::FreeStellaniteLuminaItemId)
    {
        const int64_t freeOwned = GetResourceCount(GameConstants::FreeStellaniteLuminaItemId);
        const int64_t useFree = (std::min)(freeOwned, qty);
        if (useFree > 0 && !RemoveItem(GameConstants::FreeStellaniteLuminaItemId, useFree, &change))
        {
            return false;
        }

        const int64_t remain = qty - useFree;
        if (remain > 0 && !RemoveItem(GameConstants::PaidStellaniteLuminaItemId, remain, &change))
        {
            return false;
        }

        return true;
    }

    return RemoveItem(currencyId, qty, &change);
}

void InventoryMgr::PushItemsChange(const proto::ChangeInfo& change)
{
    if (!GetPlayer() || !GetPlayer()->GetSessionRef())
    {
        return;
    }

    if (change.props_size() <= 0)
    {
        return;
    }

    GetPlayer()->PushNextPackage(items_change_notify, change);
}

void InventoryMgr::EncodePlayerInfo(proto::PlayerInfo& out) const
{
    for (const auto& [tid, qty] : Bin().resources())
    {
        if (qty <= 0)
        {
            continue;
        }

        auto* res = out.add_res();
        res->set_tid(tid);
        res->set_qty(ChangeInfoUtil::ClampQty(qty));
    }

    for (const auto& [tid, qty] : Bin().items())
    {
        if (qty <= 0)
        {
            continue;
        }

        auto* item = out.add_items();
        item->set_tid(tid);
        item->set_qty(ChangeInfoUtil::ClampQty(qty));
    }

    for (uint32_t titleId : Bin().titles())
    {
        out.add_titles()->set_titleid(titleId);
    }

    for (uint32_t honorId : Bin().honors())
    {
        out.add_honorlist(honorId);
    }
}
