#include "TowerRooms.h"

#include "TowerCases.h"
#include "TowerMgr.h"
#include "../Resources/BinClass/StarTowerRes.h"
#include "../Resources/GameData.h"

#include <algorithm>

TowerRoom::TowerRoom(TowerRuntime::Game* game, uint32_t stageId, TowerRoomType roomType)
    : mGame(game)
    , mStageId(stageId)
    , mRoomType(roomType)
{
}

TowerRoom::~TowerRoom() = default;

void TowerRoom::SetMapInfo(uint32_t mapId, uint32_t mapTableId, const std::string& mapParam, uint32_t paramId)
{
    mMapId = mapId;
    mMapTableId = mapTableId;
    mMapParam = mapParam;
    mParamId = paramId;
}

uint32_t TowerRoom::GetNextCaseId()
{
    return ++mLastCaseId;
}

TowerCaseBase* TowerRoom::AddCase(std::unique_ptr<TowerCaseBase> towerCase)
{
    if (!towerCase)
    {
        return nullptr;
    }

    if (towerCase->GetId() == 0)
    {
        towerCase->Register(this, GetNextCaseId());
    }
    else
    {
        mLastCaseId = (std::max)(mLastCaseId, towerCase->GetId());
        towerCase->Register(this, towerCase->GetId());
    }

    if (towerCase->GetType() == TowerCaseType::Door)
    {
        mHasDoor = true;
    }

    TowerCaseBase* result = towerCase.get();
    mCases.push_back(std::move(towerCase));
    return result;
}

TowerCaseBase* TowerRoom::GetCaseById(uint32_t id) const
{
    for (const auto& towerCase : mCases)
    {
        if (towerCase && towerCase->GetId() == id)
        {
            return towerCase.get();
        }
    }

    return nullptr;
}

void TowerRoom::OnEnter()
{
    AddCase(std::make_unique<TowerSyncHPCase>());
    AddCase(CreateDoorCase());
}

proto::StarTowerRoom TowerRoom::ToProto() const
{
    proto::StarTowerRoom out;
    auto* data = out.mutable_data();
    data->set_floor(0);
    data->set_mapid(mMapId);
    data->set_maptableid(mMapTableId);
    data->set_mapparam(mMapParam);
    data->set_paramid(mParamId);
    data->set_roomtype(static_cast<uint32_t>(mRoomType));

    for (const auto& towerCase : mCases)
    {
        if (!towerCase)
        {
            continue;
        }
        out.add_cases()->CopyFrom(towerCase->ToProto());
    }

    return out;
}

std::unique_ptr<TowerCaseBase> TowerRoom::CreateNpcEventCase() const
{
    for (const auto& [_, data] : GameData::StarTowerEventDataTable)
    {
        if (!data.RelatedNPCs.empty())
        {
            auto instance = std::make_unique<TowerNpcEventCase>();
            instance->NpcId = static_cast<uint32_t>(data.RelatedNPCs.front());
            instance->EventId = static_cast<uint32_t>(data.Id);
            return instance;
        }
    }

    return nullptr;
}

std::unique_ptr<TowerCaseBase> TowerRoom::CreateDoorCase() const
{
    auto instance = std::make_unique<TowerDoorCase>();
    instance->FloorNum = mGame ? mGame->FloorCount + 1 : 0;
    instance->RoomType = static_cast<uint32_t>(mRoomType);
    return instance;
}

void TowerRoom::SaveToBin(ServerProto::TowerRoomBin& bin) const
{
    bin.Clear();
    bin.set_stageid(mStageId);
    bin.set_roomtype(static_cast<uint32_t>(mRoomType));
    bin.set_mapid(mMapId);
    bin.set_maptableid(mMapTableId);
    bin.set_mapparam(mMapParam);
    bin.set_paramid(mParamId);
    bin.set_lastcaseid(mLastCaseId);
    bin.set_hasdoor(mHasDoor);

    for (const auto& towerCase : mCases)
    {
        if (!towerCase)
        {
            continue;
        }
        towerCase->SaveToBin(*bin.add_cases());
    }
}

std::unique_ptr<TowerRoom> TowerRoom::LoadFromBin(TowerRuntime::Game* game, const ServerProto::TowerRoomBin& bin)
{
    std::unique_ptr<TowerRoom> room;
    const TowerRoomType type = static_cast<TowerRoomType>(bin.roomtype());
    if (type == TowerRoomType::EventRoom)
    {
        room = std::make_unique<TowerEventRoom>(game, bin.stageid(), type);
    }
    else if (type == TowerRoomType::ShopRoom)
    {
        room = std::make_unique<TowerHawkerRoom>(game, bin.stageid(), type);
    }
    else if (type == TowerRoomType::BattleRoom || type == TowerRoomType::EliteBattleRoom ||
        type == TowerRoomType::BossRoom || type == TowerRoomType::FinalBossRoom ||
        type == TowerRoomType::DangerRoom || type == TowerRoomType::HorrorRoom ||
        type == TowerRoomType::UnifyBattleRoom)
    {
        room = std::make_unique<TowerBattleRoom>(game, bin.stageid(), type);
    }
    else
    {
        room = std::make_unique<TowerRoom>(game, bin.stageid(), type);
    }

    room->mMapId = bin.mapid();
    room->mMapTableId = bin.maptableid();
    room->mMapParam = bin.mapparam();
    room->mParamId = bin.paramid();
    room->mLastCaseId = bin.lastcaseid();
    room->mHasDoor = bin.hasdoor();

    for (const auto& caseBin : bin.cases())
    {
        auto towerCase = TowerCaseBase::LoadFromBin(game, caseBin);
        if (towerCase)
        {
            room->AddCase(std::move(towerCase));
        }
    }

    return room;
}

void TowerBattleRoom::OnEnter()
{
    AddCase(std::make_unique<TowerBattleCase>());
    AddCase(std::make_unique<TowerSyncHPCase>());
}

void TowerEventRoom::OnEnter()
{
    AddCase(CreateNpcEventCase());
    AddCase(std::make_unique<TowerSyncHPCase>());
    AddCase(CreateDoorCase());
}

void TowerHawkerRoom::OnEnter()
{
    AddCase(std::make_unique<TowerHawkerCase>());
    AddCase(std::make_unique<TowerStrengthenMachineCase>());
    AddCase(std::make_unique<TowerSyncHPCase>());
    AddCase(CreateDoorCase());
}
