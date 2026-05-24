#pragma once
#include "Player.h"

class ManagerBase
{
protected:
    Player* mPayerRef;

public:
    ManagerBase();

    explicit ManagerBase(Player* player);

    virtual ~ManagerBase() = default;

public:
    Player* GetPlayer() const;

    void SetPlayer(Player* player);

    int GetPlayerUid() const;

    virtual void OnLoad();
};