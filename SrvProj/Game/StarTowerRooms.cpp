#include "StarTowerRooms.h"

#include "StarTowerCases.h"
#include "StarTowerMgr.h"
#include "../GameConstants.h"
#include "../Resources/BinClass/StarTowerRes.h"
#include "../Resources/GameData.h"

#include <algorithm>
#include <random>

namespace
{
std::vector<uint32_t> BuildEventOptions(const StarTowerEventRes& data)
{
    std::vector<uint32_t> options;
    options.reserve((std::min)(static_cast<size_t>(4), data.OptionIds.size()));

    std::vector<int> candidates = data.OptionIds;
    std::shuffle(candidates.begin(), candidates.end(), std::mt19937{ std::random_device{}() });

    const size_t maxOptions = (std::min)(static_cast<size_t>(4), candidates.size());
    for (size_t i = 0; i < maxOptions; ++i)
    {
        if (candidates[i] > 0)
        {
            options.push_back(static_cast<uint32_t>(candidates[i]));
        }
    }

    if (data.Id >= 114 && data.Id <= 116)
    {
        const uint32_t answerId = static_cast<uint32_t>((data.Id * 100) + 3);
        if (std::find(options.begin(), options.end(), answerId) == options.end())
        {
            if (options.empty())
            {
                options.push_back(answerId);
            }
            else
            {
                options.front() = answerId;
            }
        }
    }

    std::shuffle(options.begin(), options.end(), std::mt19937{ std::random_device{}() });
    return options;
}

bool IsBattleRoomType(StarTowerRoomType type)
{
    return type == StarTowerRoomType::BattleRoom ||
        type == StarTowerRoomType::EliteBattleRoom ||
        type == StarTowerRoomType::BossRoom ||
        type == StarTowerRoomType::FinalBossRoom;
}
}

StarTowerRoom::StarTowerRoom(StarTowerRuntime::Game* game, uint32_t stageId, StarTowerRoomType roomType)
    : mGame(game)
    , mStageId(stageId)
    , mRoomType(roomType)
{
}

StarTowerRoom::~StarTowerRoom() = default;

void StarTowerRoom::SetMapInfo(uint32_t mapId, uint32_t mapTableId, const std::string& mapParam, uint32_t paramId)
{
    mMapId = mapId;
    mMapTableId = mapTableId;
    mMapParam = mapParam;
    mParamId = paramId;
}

uint32_t StarTowerRoom::GetNextCaseId()
{
    return ++mLastCaseId;
}

StarTowerCaseBase* StarTowerRoom::AddCase(std::unique_ptr<StarTowerCaseBase> towerCase)
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

    if (towerCase->GetType() == StarTowerCaseType::Door)
    {
        mHasDoor = true;
    }

    StarTowerCaseBase* result = towerCase.get();
    mCases.push_back(std::move(towerCase));
    return result;
}

StarTowerCaseBase* StarTowerRoom::AddLoadedCase(std::unique_ptr<StarTowerCaseBase> towerCase)
{
    if (!towerCase)
    {
        return nullptr;
    }

    if (towerCase->GetId() == 0)
    {
        towerCase->RegisterLoaded(this, GetNextCaseId());
    }
    else
    {
        mLastCaseId = (std::max)(mLastCaseId, towerCase->GetId());
        towerCase->RegisterLoaded(this, towerCase->GetId());
    }

    if (towerCase->GetType() == StarTowerCaseType::Door)
    {
        mHasDoor = true;
    }

    StarTowerCaseBase* result = towerCase.get();
    mCases.push_back(std::move(towerCase));
    return result;
}

StarTowerCaseBase* StarTowerRoom::GetCaseById(uint32_t id) const
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

void StarTowerRoom::OnEnter()
{
    AddCase(std::make_unique<StarTowerSyncHPCase>());
    AddCase(CreateDoorCase());
}

proto::StarTowerRoom StarTowerRoom::ToProto() const
{
    proto::StarTowerRoom out;
    auto* data = out.mutable_data();
    data->set_floor(mGame ? mGame->FloorCount : 0);
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

std::unique_ptr<StarTowerCaseBase> StarTowerRoom::CreateNpcEventCase() const
{
    std::vector<const StarTowerEventRes*> candidates;
    candidates.reserve(sizeof(GameConstants::StarTowerEventIds) / sizeof(GameConstants::StarTowerEventIds[0]));

    for (uint32_t eventId : GameConstants::StarTowerEventIds)
    {
        const auto it = GameData::StarTowerEventDataTable.find(eventId);
        if (it == GameData::StarTowerEventDataTable.end())
        {
            continue;
        }

        if (!it->second.RelatedNPCs.empty() && !it->second.OptionIds.empty())
        {
            candidates.push_back(&it->second);
        }
    }

    if (candidates.empty())
    {
        return nullptr;
    }

    std::shuffle(candidates.begin(), candidates.end(), std::mt19937{ std::random_device{}() });
    const StarTowerEventRes& data = *candidates.front();

    std::vector<int> npcs = data.RelatedNPCs;
    std::shuffle(npcs.begin(), npcs.end(), std::mt19937{ std::random_device{}() });
    if (npcs.empty() || npcs.front() <= 0)
    {
        return nullptr;
    }

    auto instance = std::make_unique<StarTowerNpcEventCase>();
    instance->NpcId = static_cast<uint32_t>(npcs.front());
    instance->EventId = static_cast<uint32_t>(data.Id);
    instance->Options = BuildEventOptions(data);
    return instance;
}

std::unique_ptr<StarTowerCaseBase> StarTowerRoom::CreateDoorCase() const
{
    auto instance = std::make_unique<StarTowerDoorCase>();
    instance->FloorNum = mGame ? mGame->FloorCount + 1 : 0;
    if (mGame)
    {
        const auto towerIt = GameData::StarTowerDataTable.find(mGame->StarTowerId);
        if (towerIt != GameData::StarTowerDataTable.end())
        {
            const uint32_t nextStageId = mGame->GetNextStageId(towerIt->second);
            const auto stageIt = GameData::StarTowerStageDataTable.find(nextStageId);
            if (stageIt != GameData::StarTowerStageDataTable.end())
            {
                instance->RoomType = static_cast<uint32_t>((std::max)(stageIt->second.RoomType, 0));
            }
        }
    }
    return instance;
}

void StarTowerRoom::SaveToBin(ServerProto::StarTowerRoomBin& bin) const
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

std::unique_ptr<StarTowerRoom> StarTowerRoom::LoadFromBin(StarTowerRuntime::Game* game, const ServerProto::StarTowerRoomBin& bin)
{
    std::unique_ptr<StarTowerRoom> room;
    const StarTowerRoomType type = static_cast<StarTowerRoomType>(bin.roomtype());
    if (type == StarTowerRoomType::EventRoom)
    {
        room = std::make_unique<StarTowerEventRoom>(game, bin.stageid(), type);
    }
    else if (type == StarTowerRoomType::ShopRoom)
    {
        room = std::make_unique<StarTowerHawkerRoom>(game, bin.stageid(), type);
    }
    else if (IsBattleRoomType(type))
    {
        room = std::make_unique<StarTowerBattleRoom>(game, bin.stageid(), type);
    }
    else
    {
        room = std::make_unique<StarTowerRoom>(game, bin.stageid(), type);
    }

    room->mMapId = bin.mapid();
    room->mMapTableId = bin.maptableid();
    room->mMapParam = bin.mapparam();
    room->mParamId = bin.paramid();
    room->mLastCaseId = bin.lastcaseid();
    room->mHasDoor = bin.hasdoor();

    for (const auto& caseBin : bin.cases())
    {
        auto towerCase = StarTowerCaseBase::LoadFromBin(game, caseBin);
        if (towerCase)
        {
            room->AddLoadedCase(std::move(towerCase));
        }
    }

    return room;
}

void StarTowerBattleRoom::OnEnter()
{
    AddCase(std::make_unique<StarTowerBattleCase>());
    AddCase(std::make_unique<StarTowerSyncHPCase>());
}

void StarTowerEventRoom::OnEnter()
{
    AddCase(CreateNpcEventCase());
    AddCase(std::make_unique<StarTowerSyncHPCase>());
    AddCase(CreateDoorCase());
}

void StarTowerHawkerRoom::OnEnter()
{
    AddCase(std::make_unique<StarTowerHawkerCase>());
    if (GetGame() && GetGame()->GetDifficulty() >= 4 && GetGame()->GetManager() && GetGame()->GetManager()->HasGrowthNode(20301))
    {
        AddCase(std::make_unique<StarTowerStrengthenMachineCase>());
    }
    AddCase(std::make_unique<StarTowerSyncHPCase>());
    AddCase(CreateDoorCase());
}
