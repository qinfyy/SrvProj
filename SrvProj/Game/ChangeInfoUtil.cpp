#include "ChangeInfoUtil.h"

#include "../Resources/BinClass/ItemsRes.h"
#include "../Resources/GameData.h"

#include <limits>
#include <string>

namespace ChangeInfoUtil
{
int GetItemType(uint32_t tid)
{
    const auto it = GameData::ItemDataTable.find(std::to_string(tid));
    if (it == GameData::ItemDataTable.end())
    {
        return 0;
    }

    return it->second.Type;
}

int GetItemSubType(uint32_t tid)
{
    const auto it = GameData::ItemDataTable.find(std::to_string(tid));
    if (it == GameData::ItemDataTable.end())
    {
        return 0;
    }

    return it->second.Stype;
}

bool IsKnownItem(uint32_t tid)
{
    return GameData::ItemDataTable.find(std::to_string(tid)) != GameData::ItemDataTable.end();
}

bool IsResource(uint32_t tid)
{
    return GetItemType(tid) == ItemType::Res;
}

int32_t ClampQty(int64_t qty)
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

void AddItemTpl(proto::ItemTpl* out, uint32_t tid, int64_t qty)
{
    if (!out)
    {
        return;
    }

    out->set_tid(tid);
    out->set_qty(ClampQty(qty));
}

void AddProp(proto::ChangeInfo& change, const google::protobuf::Message& message)
{
    auto* any = change.add_props();
    std::string typeUrl = "type.googleapis.com/";
    const auto fullName = message.GetDescriptor()->full_name();
    typeUrl.append(fullName.data(), fullName.size());
    any->set_type_url(typeUrl);
    any->set_value(message.SerializeAsString());
}

void AddItemOrRes(proto::ChangeInfo& change, uint32_t tid, int64_t qty)
{
    if (tid == 0 || qty == 0)
    {
        return;
    }

    if (IsResource(tid))
    {
        proto::Res res;
        res.set_tid(tid);
        res.set_qty(ClampQty(qty));
        AddProp(change, res);
        return;
    }

    proto::Item item;
    item.set_tid(tid);
    item.set_qty(ClampQty(qty));
    AddProp(change, item);
}

void AddTitle(proto::ChangeInfo& change, uint32_t titleId)
{
    if (titleId == 0)
    {
        return;
    }

    proto::Title title;
    title.set_titleid(titleId);
    AddProp(change, title);
}

void AddHonor(proto::ChangeInfo& change, uint32_t honorId, uint32_t level)
{
    if (honorId == 0)
    {
        return;
    }

    proto::Honor honor;
    honor.set_newid(honorId);
    honor.set_level(level);
    AddProp(change, honor);
}

void AddHeadIcon(proto::ChangeInfo& change, uint32_t tid)
{
    if (tid == 0)
    {
        return;
    }

    proto::HeadIcon headIcon;
    headIcon.set_tid(tid);
    AddProp(change, headIcon);
}
}
