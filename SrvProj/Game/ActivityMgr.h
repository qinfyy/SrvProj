#pragma once

#include "ManagerBase.h"
#include "../proto/proto_cpp/player_data.pb.h"

class ActivityMgr : public ManagerBase
{
public:
    using ManagerBase::ManagerBase;

    void EncodePlayerInfo(proto::PlayerInfo& out) const override;
};
