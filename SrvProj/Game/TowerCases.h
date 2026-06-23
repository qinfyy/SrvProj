#pragma once

#include "TowerRuntime.h"
#include "../proto/ServerProto_cpp/PlayerData.pb.h"
#include "../proto/proto_cpp/star_tower_interact.pb.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class TowerRoom;

enum class TowerCaseType : uint32_t
{
    Battle = 1,
    Door = 2,
    Potential = 3,
    FateCard = 4,
    Note = 5,
    NpcEvent = 6,
    SelectSpecialPotential = 7,
    RecoveryHP = 8,
    NpcRecoveryHP = 9,
    Hawker = 10,
    StrengthenMachine = 11,
    DoorDanger = 12,
    SyncHP = 13,
};

class TowerCaseBase
{
public:
    virtual ~TowerCaseBase() = default;

    TowerRuntime::Game* GetGame() const { return mGame; }
    TowerRoom* GetRoom() const { return mRoom; }
    uint32_t GetId() const { return mId; }

    void Register(TowerRoom* room, uint32_t id);
    virtual void OnRegister() {}
    virtual TowerCaseType GetType() const = 0;
    virtual bool RemoveAfterInteract() const { return true; }
    virtual proto::StarTowerInteractResp Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp) = 0;
    virtual proto::StarTowerRoomCase ToProto() const = 0;
    virtual void SaveToBin(ServerProto::TowerCaseBin& bin) const = 0;

    static std::unique_ptr<TowerCaseBase> LoadFromBin(TowerRuntime::Game* game, const ServerProto::TowerCaseBin& bin);

protected:
    TowerRuntime::Game* mGame = nullptr;
    TowerRoom* mRoom = nullptr;
    uint32_t mId = 0;
};

class TowerBattleCase : public TowerCaseBase
{
public:
    uint32_t SubNoteDrops = 0;
    uint32_t ExpReward = 0;

    TowerCaseType GetType() const override { return TowerCaseType::Battle; }
    void OnRegister() override;
    proto::StarTowerInteractResp Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp) override;
    proto::StarTowerRoomCase ToProto() const override;
    void SaveToBin(ServerProto::TowerCaseBin& bin) const override;
};

class TowerDoorCase : public TowerCaseBase
{
public:
    uint32_t FloorNum = 0;
    uint32_t RoomType = 0;

    TowerCaseType GetType() const override { return TowerCaseType::Door; }
    proto::StarTowerInteractResp Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp) override;
    proto::StarTowerRoomCase ToProto() const override;
    void SaveToBin(ServerProto::TowerCaseBin& bin) const override;
};

class TowerPotentialCase : public TowerCaseBase
{
public:
    uint32_t TeamLevel = 0;
    uint32_t CharId = 0;
    uint32_t Reroll = 0;
    uint32_t RerollPrice = 0;
    bool Strengthen = false;
    bool Rare = false;
    std::vector<TowerRuntime::PotentialInfo> Potentials;
    uint32_t SourceType = 0;

    TowerCaseType GetType() const override { return Rare ? TowerCaseType::SelectSpecialPotential : TowerCaseType::Potential; }
    proto::StarTowerInteractResp Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp) override;
    proto::StarTowerRoomCase ToProto() const override;
    void SaveToBin(ServerProto::TowerCaseBin& bin) const override;
};

class TowerNpcEventCase : public TowerCaseBase
{
public:
    uint32_t NpcId = 0;
    uint32_t EventId = 0;
    std::vector<uint32_t> Options;
    bool Completed = false;

    TowerCaseType GetType() const override { return TowerCaseType::NpcEvent; }
    proto::StarTowerInteractResp Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp) override;
    proto::StarTowerRoomCase ToProto() const override;
    void SaveToBin(ServerProto::TowerCaseBin& bin) const override;
};

class TowerHawkerCase : public TowerCaseBase
{
public:
    std::vector<TowerRuntime::ShopGoods> Goods;

    TowerCaseType GetType() const override { return TowerCaseType::Hawker; }
    bool RemoveAfterInteract() const override { return false; }
    void OnRegister() override;
    proto::StarTowerInteractResp Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp) override;
    proto::StarTowerRoomCase ToProto() const override;
    void SaveToBin(ServerProto::TowerCaseBin& bin) const override;
};

class TowerStrengthenMachineCase : public TowerCaseBase
{
public:
    bool Free = false;
    int32_t Discount = 0;
    uint32_t Times = 0;
    int32_t GetPrice() const;

    TowerCaseType GetType() const override { return TowerCaseType::StrengthenMachine; }
    bool RemoveAfterInteract() const override { return false; }
    proto::StarTowerInteractResp Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp) override;
    proto::StarTowerRoomCase ToProto() const override;
    void SaveToBin(ServerProto::TowerCaseBin& bin) const override;
};

class TowerRecoveryHPCase : public TowerCaseBase
{
public:
    uint32_t EffectId = 0;

    TowerCaseType GetType() const override { return TowerCaseType::RecoveryHP; }
    proto::StarTowerInteractResp Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp) override;
    proto::StarTowerRoomCase ToProto() const override;
    void SaveToBin(ServerProto::TowerCaseBin& bin) const override;
};

class TowerNpcRecoveryHPCase : public TowerCaseBase
{
public:
    uint32_t EffectId = 989970;

    TowerCaseType GetType() const override { return TowerCaseType::NpcRecoveryHP; }
    proto::StarTowerInteractResp Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp) override;
    proto::StarTowerRoomCase ToProto() const override;
    void SaveToBin(ServerProto::TowerCaseBin& bin) const override;
};

class TowerSyncHPCase : public TowerCaseBase
{
public:
    TowerCaseType GetType() const override { return TowerCaseType::SyncHP; }
    proto::StarTowerInteractResp Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp) override;
    proto::StarTowerRoomCase ToProto() const override;
    void SaveToBin(ServerProto::TowerCaseBin& bin) const override;
};
