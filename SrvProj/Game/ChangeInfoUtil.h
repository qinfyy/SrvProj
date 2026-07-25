#pragma once

#include "../proto/proto_cpp/public.pb.h"

#include <cstdint>

namespace ChangeInfoUtil
{
    enum ItemType : int
    {
        Res = 1,
        Item = 2,
        Char = 3,
        Energy = 4,
        WorldRankExp = 5,
        RogueItem = 6,
        Disc = 7,
        Equipment = 9,
        CharacterSkin = 10,
        MonthlyCard = 11,
        Title = 12,
        Honor = 13,
        HeadItem = 14,
        LevelHonor = 15,
    };

    enum ItemSubType : int
    {
        RandomPackage = 34,
    };

    int GetItemType(uint32_t tid);
    int GetItemSubType(uint32_t tid);
    bool IsKnownItem(uint32_t tid);
    bool IsResource(uint32_t tid);
    int32_t ClampQty(int64_t qty);

    void AddItemTpl(proto::ItemTpl* out, uint32_t tid, int64_t qty);
    void AddProp(proto::ChangeInfo& change, const google::protobuf::Message& message);
    void AddItemOrRes(proto::ChangeInfo& change, uint32_t tid, int64_t qty);
    void AddTitle(proto::ChangeInfo& change, uint32_t titleId);
    void AddHonor(proto::ChangeInfo& change, uint32_t honorId, uint32_t level = 0);
    void AddHeadIcon(proto::ChangeInfo& change, uint32_t tid);
    void AddItemChange(proto::ChangeInfo& change, uint32_t tid, int32_t qty);
}
