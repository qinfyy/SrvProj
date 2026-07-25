#pragma once

#include "StarTowerRuntime.h"
#include "../proto/ServerProto_cpp/PlayerData.pb.h"
#include "../proto/proto_cpp/star_tower_interact.pb.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class StarTowerRoom;

enum class StarTowerCaseType : uint32_t
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

class StarTowerCaseBase
{
public:
    virtual ~StarTowerCaseBase() = default;

    StarTowerRuntime::Game* GetGame() const { return mGame; }
    StarTowerRoom* GetRoom() const { return mRoom; }
    uint32_t GetId() const { return mId; }

    void Register(StarTowerRoom* room, uint32_t id);
    void RegisterLoaded(StarTowerRoom* room, uint32_t id);
    virtual void OnRegister() {}
    virtual StarTowerCaseType GetType() const = 0;
    virtual bool RemoveAfterInteract() const { return true; }
    virtual proto::StarTowerInteractResp Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp) = 0;
    virtual proto::StarTowerRoomCase ToProto() const = 0;
    virtual void SaveToBin(ServerProto::StarTowerCaseBin& bin) const = 0;

    static std::unique_ptr<StarTowerCaseBase> LoadFromBin(StarTowerRuntime::Game* game, const ServerProto::StarTowerCaseBin& bin);

protected:
    StarTowerRuntime::Game* mGame = nullptr;
    StarTowerRoom* mRoom = nullptr;
    uint32_t mId = 0;
};

class StarTowerBattleCase : public StarTowerCaseBase
{
public:
    uint32_t SubNoteDrops = 0;
    uint32_t ExpReward = 0;

    StarTowerCaseType GetType() const override { return StarTowerCaseType::Battle; }
    void OnRegister() override;
    proto::StarTowerInteractResp Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp) override;
    proto::StarTowerRoomCase ToProto() const override;
    void SaveToBin(ServerProto::StarTowerCaseBin& bin) const override;
};

class StarTowerDoorCase : public StarTowerCaseBase
{
public:
    uint32_t FloorNum = 0;
    uint32_t RoomType = 0;

    StarTowerCaseType GetType() const override { return StarTowerCaseType::Door; }
    proto::StarTowerInteractResp Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp) override;
    proto::StarTowerRoomCase ToProto() const override;
    void SaveToBin(ServerProto::StarTowerCaseBin& bin) const override;
};

class StarTowerPotentialCase : public StarTowerCaseBase
{
public:
    uint32_t TeamLevel = 0;
    uint32_t CharId = 0;
    uint32_t Reroll = 0;
    uint32_t RerollPrice = 0;
    bool Strengthen = false;
    bool Rare = false;
    std::vector<StarTowerRuntime::PotentialInfo> Potentials;
    uint32_t SourceType = 0;

    StarTowerCaseType GetType() const override { return Rare ? StarTowerCaseType::SelectSpecialPotential : StarTowerCaseType::Potential; }
    proto::StarTowerInteractResp Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp) override;
    proto::StarTowerRoomCase ToProto() const override;
    void SaveToBin(ServerProto::StarTowerCaseBin& bin) const override;
};

class StarTowerNpcEventCase : public StarTowerCaseBase
{
public:
    uint32_t NpcId = 0;
    uint32_t EventId = 0;
    std::vector<uint32_t> Options;
    bool Completed = false;

    StarTowerCaseType GetType() const override { return StarTowerCaseType::NpcEvent; }
    proto::StarTowerInteractResp Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp) override;
    proto::StarTowerRoomCase ToProto() const override;
    void SaveToBin(ServerProto::StarTowerCaseBin& bin) const override;
};

class StarTowerHawkerCase : public StarTowerCaseBase
{
public:
    std::vector<StarTowerRuntime::ShopGoods> Goods;
    uint32_t RerollTimes = 0;
    uint32_t RerollPrice = 0;

    StarTowerCaseType GetType() const override { return StarTowerCaseType::Hawker; }
    bool RemoveAfterInteract() const override { return false; }
    void OnRegister() override;
    void InitGoods();
    proto::StarTowerInteractResp Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp) override;
    proto::StarTowerRoomCase ToProto() const override;
    void SaveToBin(ServerProto::StarTowerCaseBin& bin) const override;
};

class StarTowerStrengthenMachineCase : public StarTowerCaseBase
{
public:
    bool Free = false;
    int32_t Discount = 0;
    uint32_t Times = 0;
    int32_t GetPrice() const;

    StarTowerCaseType GetType() const override { return StarTowerCaseType::StrengthenMachine; }
    bool RemoveAfterInteract() const override { return false; }
    void OnRegister() override;
    proto::StarTowerInteractResp Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp) override;
    proto::StarTowerRoomCase ToProto() const override;
    void SaveToBin(ServerProto::StarTowerCaseBin& bin) const override;
};

class StarTowerRecoveryHPCase : public StarTowerCaseBase
{
public:
    uint32_t EffectId = 0;

    StarTowerCaseType GetType() const override { return StarTowerCaseType::RecoveryHP; }
    proto::StarTowerInteractResp Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp) override;
    proto::StarTowerRoomCase ToProto() const override;
    void SaveToBin(ServerProto::StarTowerCaseBin& bin) const override;
};

class StarTowerNpcRecoveryHPCase : public StarTowerCaseBase
{
public:
    uint32_t EffectId = 989970;

    StarTowerCaseType GetType() const override { return StarTowerCaseType::NpcRecoveryHP; }
    proto::StarTowerInteractResp Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp) override;
    proto::StarTowerRoomCase ToProto() const override;
    void SaveToBin(ServerProto::StarTowerCaseBin& bin) const override;
};

class StarTowerSyncHPCase : public StarTowerCaseBase
{
public:
    StarTowerCaseType GetType() const override { return StarTowerCaseType::SyncHP; }
    proto::StarTowerInteractResp Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp) override;
    proto::StarTowerRoomCase ToProto() const override;
    void SaveToBin(ServerProto::StarTowerCaseBin& bin) const override;
};
