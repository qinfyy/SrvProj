#include "InventoryMgr.h"

#include "Player.h"
#include "../GameConstants.h"
#include "../Logger.h"
#include "../Resources/BinClass/ItemsRes.h"
#include "../Resources/GameData.h"
#include "../proto/NetMsgId.pb.h"

#include <algorithm>
#include <cstdint>
#include <limits>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace {
int32_t ClampChangeQty(int64_t qty)
{
    if (qty > std::numeric_limits<int32_t>::max())
    {
        return std::numeric_limits<int32_t>::max();
    }

    if (qty < std::numeric_limits<int32_t>::min())
    {
        return std::numeric_limits<int32_t>::min();
    }

    return static_cast<int32_t>(qty);
}
}

void InventoryMgr::OnCreate()
{
    MutableBin()->clear_items();
    MutableBin()->clear_resources();
    MutableBin()->clear_skins();
    MutableBin()->clear_headicons();
    MutableBin()->clear_titles();
    MutableBin()->clear_honors();

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

bool InventoryMgr::IsResourceItem(uint32_t tid) const
{
    if (tid == GameConstants::GoldItemId ||
        tid == GameConstants::GemItemId ||
        tid == GameConstants::WeeklyEntryItemId ||
        tid == GameConstants::JointDrillTicketId)
    {
        return true;
    }

    const auto it = GameData::ItemDataTable.find(std::to_string(tid));
    if (it == GameData::ItemDataTable.end())
    {
        return false;
    }

    return it->second.Type == 1 || it->second.Stype == 1;
}

void InventoryMgr::AddItemChange(proto::ChangeInfo* change, uint32_t tid, int64_t qty) const
{
    if (!change || tid == 0 || qty == 0)
    {
        return;
    }

    proto::Item item;
    item.set_tid(tid);
    item.set_qty(ClampChangeQty(qty));
    change->add_props()->PackFrom(item);
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
    ensure(GameConstants::GemItemId, 30000);
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

    if (IsResourceItem(tid))
    {
        auto* resources = MutableBin()->mutable_resources();
        (*resources)[tid] += qty;
    }
    else
    {
        auto* items = MutableBin()->mutable_items();
        (*items)[tid] += qty;
    }

    AddItemChange(change, tid, qty);
    GetPlayer()->Trigger(48, static_cast<uint32_t>(std::min<int64_t>(qty, std::numeric_limits<uint32_t>::max())), tid, 0);
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

bool InventoryMgr::ConsumeItem(uint32_t tid, int64_t qty, proto::ChangeInfo* change)
{
    if (tid == 0 || qty <= 0)
    {
        return false;
    }

    auto* map = IsResourceItem(tid)
        ? MutableBin()->mutable_resources()
        : MutableBin()->mutable_items();

    auto it = map->find(tid);
    if (it == map->end() || it->second < qty)
    {
        return false;
    }

    it->second -= qty;
    if (it->second <= 0)
    {
        map->erase(tid);
    }

    AddItemChange(change, tid, -qty);
    GetPlayer()->Trigger(49, static_cast<uint32_t>(std::min<int64_t>(qty, std::numeric_limits<uint32_t>::max())), tid, 0);
    return true;
}

int64_t InventoryMgr::GetItemCount(uint32_t tid) const
{
    const auto& resources = Bin().resources();
    if (auto it = resources.find(tid); it != resources.end())
    {
        return it->second;
    }

    const auto& items = Bin().items();
    if (auto it = items.find(tid); it != items.end())
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
    AddItemChange(change, id, 1);
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
        res->set_qty(ClampChangeQty(qty));
    }

    for (const auto& [tid, qty] : Bin().items())
    {
        if (qty <= 0)
        {
            continue;
        }

        auto* item = out.add_items();
        item->set_tid(tid);
        item->set_qty(ClampChangeQty(qty));
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
