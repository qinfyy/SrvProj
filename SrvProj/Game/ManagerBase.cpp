#include "ManagerBase.h"
#include "Player.h"

ManagerBase::ManagerBase()
    : mPlayerRef(nullptr)
{
}

ManagerBase::ManagerBase(Player* player)
    : mPlayerRef(player)
{
}

Player* ManagerBase::GetPlayer() const
{
    return mPlayerRef;
}

void ManagerBase::SetPlayer(Player* player)
{
    mPlayerRef = player;
}

int ManagerBase::GetPlayerUid() const
{
    return GetPlayer() ? static_cast<int>(GetPlayer()->GetUid()) : 0;
}

void ManagerBase::OnCreate()
{
}

void ManagerBase::OnLoad()
{
}

void ManagerBase::BeforeSave()
{
}

void ManagerBase::EncodePlayerInfo(proto::PlayerInfo& out) const
{
}
