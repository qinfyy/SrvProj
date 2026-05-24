#include "ManagerBase.h"

ManagerBase::ManagerBase()
    : mPayerRef(nullptr)
{
}

ManagerBase::ManagerBase(Player* player)
    : mPayerRef(player)
{
}

Player* ManagerBase::GetPlayer() const
{
    return this->mPayerRef;
}

void ManagerBase::SetPlayer(Player* player)
{
    if (this->mPayerRef == nullptr)
    {
        this->mPayerRef = player;
    }
}

int ManagerBase::GetPlayerUid() const
{
    return this->GetPlayer()->GetUid();
}

void ManagerBase::OnLoad()
{
}
