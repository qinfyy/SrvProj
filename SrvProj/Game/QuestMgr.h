#pragma once

#include "ManagerBase.h"
#include "../proto/proto_cpp/player_data.pb.h"

class QuestMgr : public ManagerBase
{
public:
    using ManagerBase::ManagerBase;

    void EncodePlayerInfo(proto::PlayerInfo& out) const override
    {
        out.mutable_quests();
        out.set_tourguidequestgroup(9);
    }
};
