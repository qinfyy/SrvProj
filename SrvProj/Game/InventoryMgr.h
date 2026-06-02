#pragma once

#include "ManagerBase.h"
#include "../proto/proto_cpp/player_data.pb.h"

class InventoryMgr : public ManagerBase
{
public:
    using ManagerBase::ManagerBase;

    void EncodePlayerInfo(proto::PlayerInfo& out) const override
    {
        auto* gold = out.add_res();
        gold->set_tid(1);
        gold->set_qty(1000000);

        auto* gems = out.add_res();
        gems->set_tid(2);
        gems->set_qty(30000);

        auto* jointDrillTicket = out.add_res();
        jointDrillTicket->set_tid(36);
        jointDrillTicket->set_qty(3);

        auto* weeklyEntry = out.add_res();
        weeklyEntry->set_tid(28);
        weeklyEntry->set_qty(3);
    }
};
