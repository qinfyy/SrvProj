#pragma once

#include "../proto/ServerProto_cpp/PlayerData.pb.h"
#include "../proto/proto_cpp/public_star_tower.pb.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class StarTowerCaseBase;
class StarTowerStageRes;
namespace StarTowerRuntime { class Game; }

enum class StarTowerRoomType : uint32_t
{
    BattleRoom = 0,
    EliteBattleRoom = 1,
    BossRoom = 2,
    FinalBossRoom = 3,
    DangerRoom = 4,
    HorrorRoom = 5,
    ShopRoom = 6,
    EventRoom = 7,
    UnifyBattleRoom = 15,
};

class StarTowerRoom
{
public:
    StarTowerRoom(StarTowerRuntime::Game* game, uint32_t stageId, StarTowerRoomType roomType);
    virtual ~StarTowerRoom();

    StarTowerRuntime::Game* GetGame() const { return mGame; }
    StarTowerRoomType GetType() const { return mRoomType; }
    uint32_t GetStageId() const { return mStageId; }
    uint32_t GetMapId() const { return mMapId; }
    uint32_t GetMapTableId() const { return mMapTableId; }
    const std::string& GetMapParam() const { return mMapParam; }
    uint32_t GetParamId() const { return mParamId; }
    bool HasDoor() const { return mHasDoor; }

    void SetMapInfo(uint32_t mapId, uint32_t mapTableId, const std::string& mapParam, uint32_t paramId);

    uint32_t GetNextCaseId();
    StarTowerCaseBase* AddCase(std::unique_ptr<StarTowerCaseBase> towerCase);
    StarTowerCaseBase* AddLoadedCase(std::unique_ptr<StarTowerCaseBase> towerCase);
    StarTowerCaseBase* GetCaseById(uint32_t id) const;
    std::vector<std::unique_ptr<StarTowerCaseBase>>& Cases() { return mCases; }
    const std::vector<std::unique_ptr<StarTowerCaseBase>>& Cases() const { return mCases; }

    virtual void OnEnter();
    virtual proto::StarTowerRoom ToProto() const;
    virtual void SaveToBin(ServerProto::StarTowerRoomBin& bin) const;
    virtual std::unique_ptr<StarTowerCaseBase> CreateNpcEventCase() const;
    std::unique_ptr<StarTowerCaseBase> CreateDoorCase() const;

    static std::unique_ptr<StarTowerRoom> LoadFromBin(StarTowerRuntime::Game* game, const ServerProto::StarTowerRoomBin& bin);

protected:
    StarTowerRuntime::Game* mGame = nullptr;
    uint32_t mStageId = 0;
    StarTowerRoomType mRoomType = StarTowerRoomType::BattleRoom;
    uint32_t mMapId = 0;
    uint32_t mMapTableId = 0;
    std::string mMapParam;
    uint32_t mParamId = 0;
    uint32_t mLastCaseId = 0;
    bool mHasDoor = false;
    std::vector<std::unique_ptr<StarTowerCaseBase>> mCases;
};

class StarTowerBattleRoom : public StarTowerRoom
{
public:
    using StarTowerRoom::StarTowerRoom;
    void OnEnter() override;
};

class StarTowerEventRoom : public StarTowerRoom
{
public:
    using StarTowerRoom::StarTowerRoom;
    void OnEnter() override;
};

class StarTowerHawkerRoom : public StarTowerRoom
{
public:
    using StarTowerRoom::StarTowerRoom;
    void OnEnter() override;
};
