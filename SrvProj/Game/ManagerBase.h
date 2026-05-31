#pragma once

namespace proto {
class PlayerInfo;
}

class Player;

class ManagerBase
{
protected:
    Player* mPlayerRef;

public:
    ManagerBase();
    explicit ManagerBase(Player* player);
    virtual ~ManagerBase() = default;

    Player* GetPlayer() const;
    void SetPlayer(Player* player);
    int GetPlayerUid() const;

    virtual void OnCreate();
    virtual void OnLoad();
    virtual void BeforeSave();
    virtual void EncodePlayerInfo(proto::PlayerInfo& out) const;
};
