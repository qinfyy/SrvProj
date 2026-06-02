#pragma once

#include "ManagerBase.h"
#include "../proto/ServerProto_cpp/PlayerData.pb.h"
#include "../proto/proto_cpp/player_data.pb.h"

#include <string>

class QuestMgr : public ManagerBase
{
public:
    using ManagerBase::ManagerBase;

    void OnCreate() override;
    void OnLoad() override;
    void OnLogin() override;
    void EncodePlayerInfo(proto::PlayerInfo& out) const override;

private:
    ServerProto::QuestCompBin* MutableBin();
    const ServerProto::QuestCompBin& Bin() const;

    void InitializeDefaultQuests(bool markFirstLoginDone);
    void PushFirstLoginNotifications();
};
