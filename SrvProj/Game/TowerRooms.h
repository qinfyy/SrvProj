#pragma once

#include "../proto/ServerProto_cpp/PlayerData.pb.h"
#include "../proto/proto_cpp/public_star_tower.pb.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

class TowerCaseBase;
class StarTowerStageRes;
namespace TowerRuntime { class Game; }

enum class TowerRoomType : uint32_t
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

class TowerRoom
{
public:
    TowerRoom(TowerRuntime::Game* game, uint32_t stageId, TowerRoomType roomType);
    virtual ~TowerRoom();

    TowerRuntime::Game* GetGame() const { return mGame; }
    TowerRoomType GetType() const { return mRoomType; }
    uint32_t GetStageId() const { return mStageId; }
    uint32_t GetMapId() const { return mMapId; }
    uint32_t GetMapTableId() const { return mMapTableId; }
    const std::string& GetMapParam() const { return mMapParam; }
    uint32_t GetParamId() const { return mParamId; }
    bool HasDoor() const { return mHasDoor; }

    void SetMapInfo(uint32_t mapId, uint32_t mapTableId, const std::string& mapParam, uint32_t paramId);

    uint32_t GetNextCaseId();
    TowerCaseBase* AddCase(std::unique_ptr<TowerCaseBase> towerCase);
    TowerCaseBase* AddLoadedCase(std::unique_ptr<TowerCaseBase> towerCase);
    TowerCaseBase* GetCaseById(uint32_t id) const;
    std::vector<std::unique_ptr<TowerCaseBase>>& Cases() { return mCases; }
    const std::vector<std::unique_ptr<TowerCaseBase>>& Cases() const { return mCases; }

    virtual void OnEnter();
    virtual proto::StarTowerRoom ToProto() const;
    virtual void SaveToBin(ServerProto::TowerRoomBin& bin) const;
    virtual std::unique_ptr<TowerCaseBase> CreateNpcEventCase() const;
    std::unique_ptr<TowerCaseBase> CreateDoorCase() const;

    static std::unique_ptr<TowerRoom> LoadFromBin(TowerRuntime::Game* game, const ServerProto::TowerRoomBin& bin);

protected:
    TowerRuntime::Game* mGame = nullptr;
    uint32_t mStageId = 0;
    TowerRoomType mRoomType = TowerRoomType::BattleRoom;
    uint32_t mMapId = 0;
    uint32_t mMapTableId = 0;
    std::string mMapParam;
    uint32_t mParamId = 0;
    uint32_t mLastCaseId = 0;
    bool mHasDoor = false;
    std::vector<std::unique_ptr<TowerCaseBase>> mCases;
};

class TowerBattleRoom : public TowerRoom
{
public:
    using TowerRoom::TowerRoom;
    void OnEnter() override;
};

class TowerEventRoom : public TowerRoom
{
public:
    using TowerRoom::TowerRoom;
    void OnEnter() override;
};

class TowerHawkerRoom : public TowerRoom
{
public:
    using TowerRoom::TowerRoom;
    void OnEnter() override;
};
