#include "TowerMgr.h"

#include "CharacterMgr.h"
#include "ChangeInfoUtil.h"
#include "FormationMgr.h"
#include "InventoryMgr.h"
#include "Player.h"
#include "TowerCases.h"
#include "TowerRooms.h"
#include "../Config.h"
#include "../GameConstants.h"
#include "../GameTime.h"
#include "../proto/NetMsgId.pb.h"
#include "../proto/proto_cpp/notify.pb.h"
#include "../Resources/BinClass/StarTowerRes.h"
#include "../Resources/GameData.h"
#include "../Util.h"

#include <algorithm>
#include <string>

namespace {
constexpr uint32_t kSweepGrowthNodeId = 10301;
constexpr uint32_t kWeeklyTicketNodeSmall = 10201;
constexpr uint32_t kWeeklyTicketNodeLarge = 10502;
constexpr uint32_t kQuestCondTowerEnterFloor = 538;

void AddRewardToChange(proto::ChangeInfo& change, uint32_t tid, int32_t qty)
{
    if (tid == 0 || qty == 0)
    {
        return;
    }

    ChangeInfoUtil::AddItemOrRes(change, tid, qty);
}
}

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

std::vector<uint32_t> TowerMgr::CollectValidCharacters(const Player& player, const google::protobuf::RepeatedField<uint32_t>& charIds)
{
    std::vector<uint32_t> out;
    out.reserve(static_cast<size_t>(charIds.size()));

    for (uint32_t charId : charIds)
    {
        if (charId == 0)
        {
            continue;
        }

        if (!player.Characters().HasCharacter(static_cast<int>(charId)))
        {
            continue;
        }

        out.push_back(charId);
    }

    return out;
}

std::vector<uint32_t> TowerMgr::CollectValidDiscs(const Player& player, const google::protobuf::RepeatedField<uint32_t>& discIds)
{
    std::vector<uint32_t> out;
    out.reserve(static_cast<size_t>(discIds.size()));

    for (uint32_t discId : discIds)
    {
        if (discId == 0)
        {
            continue;
        }

        if (!player.Characters().HasDisc(static_cast<int>(discId)))
        {
            continue;
        }

        out.push_back(discId);
    }

    return out;
}

TowerMgr::~TowerMgr() = default;

void TowerMgr::OnCreate()
{
    MutableBin()->Clear();
    MutableBin()->set_defaultunlockallstartower(Config::Get().unlockAllStarTower);
    InitializeDefaults();
}

void TowerMgr::OnLoad()
{
    InitializeDefaults();
    LoadCurrentGame();
}

void TowerMgr::OnLogin()
{
    LoadCurrentGame();
}

ServerProto::TowerCompBin* TowerMgr::MutableBin()
{
    return GetPlayer()->SaveData().mutable_towercomp();
}

const ServerProto::TowerCompBin& TowerMgr::Bin() const
{
    return GetPlayer()->SaveData().towercomp();
}

void TowerMgr::InitializeDefaults()
{
    mBuilds.clear();
    for (const auto& build : Bin().builds())
    {
        TowerRuntime::Build info;
        info.LoadFromBin(build);
        mBuilds.push_back(std::move(info));
    }

    mPresets.clear();
    for (const auto& preset : Bin().presets())
    {
        TowerRuntime::Preset info;
        info.LoadFromBin(preset);
        mPresets.push_back(std::move(info));
    }

    if (Bin().has_lastbuild() && Bin().lastbuild().uid() != 0)
    {
        mLastBuild = std::make_unique<TowerRuntime::Build>();
        mLastBuild->LoadFromBin(Bin().lastbuild());
    }
    else
    {
        mLastBuild.reset();
    }

    RebuildBookState();
    RebuildNpcAffinityBookState();
}

void TowerMgr::LoadCurrentGame()
{
    if (!Bin().has_currentgame() || Bin().currentgame().towerid() == 0)
    {
        mCurrentGame.reset();
        return;
    }

    mCurrentGame = std::make_unique<TowerRuntime::Game>();
    mCurrentGame->SetManager(this);
    mCurrentGame->LoadFromBin(Bin().currentgame());
    mCurrentGame->Room = TowerRoom::LoadFromBin(mCurrentGame.get(), Bin().currentgame().room());
}

void TowerMgr::SaveCurrentGame()
{
    if (!mCurrentGame)
    {
        MutableBin()->clear_currentgame();
        return;
    }

    mCurrentGame->SaveToBin(*MutableBin()->mutable_currentgame());
}

void TowerMgr::ClearCurrentGame()
{
    mCurrentGame.reset();
    MutableBin()->clear_currentgame();
}

proto::StarTowerState TowerMgr::BuildStateProto() const
{
    proto::StarTowerState out;
    if (!mCurrentGame)
    {
        return out;
    }

    out.set_id(mCurrentGame->TowerId);
    out.set_reconnection(1);
    out.set_buildid(mCurrentGame->BuildId);
    out.set_floor(mCurrentGame->FloorCount);
    out.set_sweep(mCurrentGame->Sweep);
    for (uint32_t charId : mCurrentGame->CharIds)
    {
        out.add_charids(charId);
    }
    return out;
}

proto::StarTowerBookState TowerMgr::BuildBookStateProto() const
{
    proto::StarTowerBookState out;
    for (uint32_t id : Bin().bookcharids())
    {
        out.add_charids(id);
    }
    for (uint32_t id : Bin().bookeventids())
    {
        out.add_eventids(id);
    }
    for (uint32_t id : Bin().bookbundles())
    {
        out.add_bundles(id);
    }
    return out;
}

void TowerMgr::EncodePlayerInfo(proto::PlayerInfo& out) const
{
    if (Bin().defaultunlockallstartower())
    {
        std::vector<uint32_t> towerIds;
        towerIds.reserve(GameData::StarTowerDataTable.size());
        for (const auto& [id, _] : GameData::StarTowerDataTable)
        {
            if (id > 0)
            {
                towerIds.push_back(static_cast<uint32_t>(id));
            }
        }
        std::sort(towerIds.begin(), towerIds.end());
        for (uint32_t towerId : towerIds)
        {
            out.add_rglpassedids(towerId);
        }
    }
    else
    {
        for (uint32_t towerId : Bin().startowerlog())
        {
            out.add_rglpassedids(towerId);
        }
    }

    out.set_towerticket(Bin().towertickets());
    out.mutable_state()->mutable_startower()->CopyFrom(BuildStateProto());
    out.mutable_state()->mutable_startowerbook()->CopyFrom(BuildBookStateProto());
}

bool TowerMgr::Apply(const proto::StarTowerApplyReq& req, proto::StarTowerApplyResp& rsp)
{
    const auto towerIt = GameData::StarTowerDataTable.find(req.id());
    if (towerIt == GameData::StarTowerDataTable.end())
    {
        return false;
    }

    const auto* formation = GetPlayer()->Formations().GetFormationById(req.formationid());
    if (!formation)
    {
        return false;
    }

    const auto validCharIds = CollectValidCharacters(*GetPlayer(), formation->charids());
    const auto validDiscIds = CollectValidDiscs(*GetPlayer(), formation->discids());
    if (validCharIds.size() != 3 || validDiscIds.size() < 3)
    {
        return false;
    }

    proto::ChangeInfo change;
    uint32_t sweepTicketId = 0;
    if (req.sweep())
    {
        if (!HasGrowthNode(kSweepGrowthNodeId))
        {
            return false;
        }

        if (!Bin().defaultunlockallstartower())
        {
            const auto it = std::find(Bin().startowerlog().begin(), Bin().startowerlog().end(), req.id());
            if (it == Bin().startowerlog().end())
            {
                return false;
            }
        }

        if (GetPlayer()->Inventory().HasItem(29, 1))
        {
            sweepTicketId = 29;
        }
        else if (GetPlayer()->Inventory().HasItem(30, 1))
        {
            sweepTicketId = 30;
        }
        else
        {
            return false;
        }
    }

    auto game = std::make_unique<TowerRuntime::Game>();
    game->SetManager(this);
    game->TowerId = req.id();
    game->FormationId = req.formationid();
    game->BuildId = TowerRuntime::GenerateUid();
    game->TeamLevel = 1;
    const auto teamExpIt = GameData::StarTowerTeamExpDataTable.find(2);
    game->NextLevelExp = teamExpIt == GameData::StarTowerTeamExpDataTable.end()
        ? 100
        : static_cast<uint32_t>((std::max)(teamExpIt->second.NeedExp, 0));
    // Nebula 创建星塔局时固定从未知血量开始，后续由 SyncHP / NpcRecoveryHP 交互同步。
    game->CharHp = -1;
    game->Sweep = req.sweep();
    game->FloorCount = 0;
    game->StageNum = 0;
    game->StageFloor = 0;
    game->CharIds = validCharIds;
    game->DiscIds = validDiscIds;
    game->InitModifierState();
    game->InitializeSubNotesFromDiscs();

    if (!game->EnterNextRoom() || !game->Room)
    {
        return false;
    }
    game->Room->SetMapInfo(req.mapid(), req.maptableid(), req.mapparam(), req.paramid());
    game->AddStartingItems();

    if (sweepTicketId != 0 && !GetPlayer()->Inventory().RemoveItem(sweepTicketId, 1, &change))
    {
        return false;
    }

    mCurrentGame = std::move(game);
    SaveCurrentGame();
    GetPlayer()->Trigger(kQuestCondTowerEnterFloor, 1, 0, 0);

    rsp.set_lastid(req.id());
    rsp.set_coinqty(static_cast<uint32_t>((std::max)(mCurrentGame->GetResCount(GameConstants::TowerCoinItemId), 0)));
    rsp.mutable_info()->CopyFrom(mCurrentGame->ToProto());
    rsp.mutable_change()->CopyFrom(change);
    return true;
}

bool TowerMgr::HandleInfo(proto::StarTowerInfo& out) const
{
    if (!mCurrentGame)
    {
        return false;
    }

    out.CopyFrom(mCurrentGame->ToProto());
    return true;
}

bool TowerMgr::HandleInteract(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp)
{
    if (!mCurrentGame || !mCurrentGame->Room)
    {
        return false;
    }

    TowerRoom* const originalRoom = mCurrentGame->Room.get();
    TowerCaseBase* towerCase = mCurrentGame->Room->GetCaseById(req.id());
    if (!towerCase)
    {
        rsp.set_id(req.id());
        rsp.mutable_nilresp();
        if (mCurrentGame)
        {
            mCurrentGame->FlushNewInfos(rsp.mutable_data());
        }
        rsp.mutable_change();
        return true;
    }

    rsp.set_id(req.id());
    const bool removeAfterInteract = towerCase->RemoveAfterInteract();
    const TowerCaseType caseType = towerCase->GetType();
    towerCase->Interact(req, rsp);
    if (mCurrentGame)
    {
        mCurrentGame->FlushNewInfos(rsp.mutable_data());
    }
    rsp.mutable_change();

    if (mCurrentGame && mCurrentGame->Completed)
    {
        auto build = std::make_unique<TowerRuntime::Build>(mCurrentGame->GetBuild());
        mLastBuild = std::move(build);
        MutableBin()->mutable_lastbuild()->Clear();
        mLastBuild->SaveToBin(*MutableBin()->mutable_lastbuild());
        ClearCurrentGame();
        return true;
    }

    // 过门失败时房间回滚且无 enter/settle：保留门 case，避免删门卡死（Nebula 此处抛异常不会走到 remove）。
    if (removeAfterInteract && mCurrentGame && mCurrentGame->Room && mCurrentGame->Room.get() == originalRoom)
    {
        const bool doorInteractFailed = caseType == TowerCaseType::Door &&
            !rsp.has_enterresp() &&
            !rsp.has_settle();
        if (!doorInteractFailed)
        {
            std::vector<std::unique_ptr<TowerCaseBase>>& roomCases = mCurrentGame->Room->Cases();
            roomCases.erase(
                std::remove_if(roomCases.begin(), roomCases.end(), [&req](const std::unique_ptr<TowerCaseBase>& entry) {
                    return entry && entry->GetId() == req.id();
                }),
                roomCases.end());
        }
    }

    SaveCurrentGame();
    return true;
}

bool TowerMgr::GiveUp(proto::StarTowerGiveUpResp& rsp)
{
    if (!mCurrentGame)
    {
        return false;
    }

    mLastBuild = std::make_unique<TowerRuntime::Build>(mCurrentGame->GetBuild());
    MutableBin()->mutable_lastbuild()->Clear();
    mLastBuild->SaveToBin(*MutableBin()->mutable_lastbuild());

    rsp.mutable_build()->CopyFrom(mLastBuild->ToProto());
    rsp.set_potentialcnt(mCurrentGame->GetTotalPotentialCount());
    rsp.set_floor(mCurrentGame->FloorCount);
    rsp.mutable_change();

    ClearCurrentGame();
    return true;
}

bool TowerMgr::BuildBriefList(proto::StarTowerBuildBriefListGetResp& rsp) const
{
    for (const auto& build : mBuilds)
    {
        rsp.add_briefs()->CopyFrom(build.ToBriefProto());
    }
    return true;
}

bool TowerMgr::BuildDetail(uint64_t buildId, proto::StarTowerBuildDetailGetResp& rsp) const
{
    const auto* build = FindBuild(buildId);
    if (!build)
    {
        return false;
    }

    rsp.mutable_detail()->CopyFrom(build->ToDetailProto());
    return true;
}

bool TowerMgr::DeleteBuilds(const google::protobuf::RepeatedField<uint64_t>& buildIds, proto::StarTowerBuildDeleteResp& rsp)
{
    proto::ChangeInfo change;
    for (uint64_t buildId : buildIds)
    {
        auto it = std::find_if(mBuilds.begin(), mBuilds.end(), [buildId](const TowerRuntime::Build& build) {
            return build.Uid == buildId;
        });
        if (it == mBuilds.end())
        {
            continue;
        }

        uint32_t tickets = static_cast<uint32_t>(it->Score / 100);
        tickets = (std::min)(tickets, GetMaxEarnableWeeklyTowerTickets());
        if (tickets > 0)
        {
            GetPlayer()->Inventory().AddItem(12, tickets, &change);
            AddWeeklyTowerTickets(tickets);
        }
        mBuilds.erase(it);
    }

    MutableBin()->clear_builds();
    for (const auto& build : mBuilds)
    {
        build.SaveToBin(*MutableBin()->add_builds());
    }

    rsp.set_ticket(GetTowerTickets());
    rsp.mutable_change()->CopyFrom(change);
    return true;
}

bool TowerMgr::SaveLastBuild(bool removeBuild, const std::string& name, bool lock, proto::StarTowerBuildWhetherSaveResp& rsp)
{
    (void)name;
    (void)lock;

    if (!mLastBuild)
    {
        return false;
    }

    std::unique_ptr<TowerRuntime::Build> lastBuild = std::move(mLastBuild);
    MutableBin()->clear_lastbuild();

    proto::ChangeInfo change;
    if (removeBuild)
    {
        uint32_t tickets = static_cast<uint32_t>(lastBuild->Score / 100);
        tickets = (std::min)(tickets, GetMaxEarnableWeeklyTowerTickets());
        if (tickets > 0)
        {
            GetPlayer()->Inventory().AddItem(12, tickets, &change);
            AddWeeklyTowerTickets(tickets);
        }
    }
    else
    {
        if (mBuilds.size() >= GameConstants::MaxBuilds)
        {
            return false;
        }

        auto saved = *lastBuild;
        int32_t rankRarity = 0;
        int32_t rankMinGrade = -1;
        for (const auto& [_, rank] : GameData::StarTowerBuildRankDataTable)
        {
            if (rank.MinGrade >= rankMinGrade && saved.Score >= static_cast<uint32_t>((std::max)(rank.MinGrade, 0)))
            {
                rankMinGrade = rank.MinGrade;
                rankRarity = rank.Rarity;
            }
        }
        if (rankRarity > 0)
        {
            GetPlayer()->Trigger(504, 1, static_cast<uint32_t>(rankRarity), 0);
        }
        mBuilds.push_back(std::move(saved));
    }

    MutableBin()->clear_builds();
    for (const auto& build : mBuilds)
    {
        build.SaveToBin(*MutableBin()->add_builds());
    }

    rsp.set_ticket(GetTowerTickets());
    rsp.mutable_change()->CopyFrom(change);
    return true;
}

bool TowerMgr::SetBuildLock(uint64_t buildId, bool lock)
{
    auto* build = FindBuild(buildId);
    if (!build)
    {
        return false;
    }
    build->Lock = lock;

    MutableBin()->clear_builds();
    for (const auto& item : mBuilds)
    {
        item.SaveToBin(*MutableBin()->add_builds());
    }
    return true;
}

bool TowerMgr::SetBuildName(uint64_t buildId, const std::string& name)
{
    auto* build = FindBuild(buildId);
    if (!build || name.empty())
    {
        return false;
    }

    build->Name = name;
    TowerRuntime::ClampNameLength(build->Name);
    MutableBin()->clear_builds();
    for (const auto& item : mBuilds)
    {
        item.SaveToBin(*MutableBin()->add_builds());
    }
    return true;
}

bool TowerMgr::SetBuildPreference(const google::protobuf::RepeatedField<uint64_t>& checkInIds, const google::protobuf::RepeatedField<uint64_t>& checkOutIds)
{
    for (uint64_t id : checkInIds)
    {
        auto* build = FindBuild(id);
        if (build)
        {
            build->Preference = true;
        }
    }
    for (uint64_t id : checkOutIds)
    {
        auto* build = FindBuild(id);
        if (build)
        {
            build->Preference = false;
        }
    }

    MutableBin()->clear_builds();
    for (const auto& item : mBuilds)
    {
        item.SaveToBin(*MutableBin()->add_builds());
    }
    return true;
}

bool TowerMgr::BuildPresetList(proto::PotentialPreselectionList& rsp) const
{
    for (const auto& preset : mPresets)
    {
        rsp.add_list()->CopyFrom(preset.ToProto());
    }
    return true;
}

bool TowerMgr::ImportPreset(const std::string& name, bool preference, const google::protobuf::RepeatedPtrField<proto::StarTowerBookCharPotential>& chars, proto::PotentialPreselection& rsp)
{
    if (mPresets.size() >= GameConstants::MaxPresets)
    {
        return false;
    }

    TowerRuntime::Preset preset;
    preset.Uid = TowerRuntime::GenerateUid();
    preset.Name = name;
    TowerRuntime::ClampNameLength(preset.Name);
    preset.Preference = preference;
    preset.Timestamp = GameTime::NowSeconds();
    if (!UpdatePresetCharacters(preset, chars))
    {
        return false;
    }

    mPresets.push_back(std::move(preset));
    MutableBin()->clear_presets();
    for (const auto& item : mPresets)
    {
        item.SaveToBin(*MutableBin()->add_presets());
    }

    rsp.CopyFrom(mPresets.back().ToProto());
    return true;
}

bool TowerMgr::UpdatePreset(uint64_t presetId, const google::protobuf::RepeatedPtrField<proto::StarTowerBookCharPotential>& chars, proto::PotentialPreselection& rsp)
{
    auto* preset = FindPreset(presetId);
    if (!preset)
    {
        return false;
    }

    if (!UpdatePresetCharacters(*preset, chars))
    {
        return false;
    }

    MutableBin()->clear_presets();
    for (const auto& item : mPresets)
    {
        item.SaveToBin(*MutableBin()->add_presets());
    }

    rsp.CopyFrom(preset->ToProto());
    return true;
}

bool TowerMgr::SetPresetName(uint64_t presetId, const std::string& name)
{
    auto* preset = FindPreset(presetId);
    if (!preset)
    {
        return false;
    }

    preset->Name = name;
    TowerRuntime::ClampNameLength(preset->Name);
    MutableBin()->clear_presets();
    for (const auto& item : mPresets)
    {
        item.SaveToBin(*MutableBin()->add_presets());
    }
    return true;
}

bool TowerMgr::SetPresetPreference(const google::protobuf::RepeatedField<uint64_t>& checkInIds, const google::protobuf::RepeatedField<uint64_t>& checkOutIds)
{
    for (uint64_t id : checkInIds)
    {
        if (!FindPreset(id))
        {
            return false;
        }
    }
    for (uint64_t id : checkOutIds)
    {
        if (!FindPreset(id))
        {
            return false;
        }
    }

    for (uint64_t id : checkInIds)
    {
        FindPreset(id)->Preference = true;
    }
    for (uint64_t id : checkOutIds)
    {
        FindPreset(id)->Preference = false;
    }

    MutableBin()->clear_presets();
    for (const auto& item : mPresets)
    {
        item.SaveToBin(*MutableBin()->add_presets());
    }
    return true;
}

bool TowerMgr::DeletePresets(const google::protobuf::RepeatedField<uint64_t>& ids)
{
    for (uint64_t id : ids)
    {
        if (!FindPreset(id))
        {
            return false;
        }
    }

    for (uint64_t id : ids)
    {
        auto it = std::remove_if(mPresets.begin(), mPresets.end(), [id](const TowerRuntime::Preset& preset) {
            return preset.Uid == id;
        });
        mPresets.erase(it, mPresets.end());
    }

    MutableBin()->clear_presets();
    for (const auto& item : mPresets)
    {
        item.SaveToBin(*MutableBin()->add_presets());
    }
    return true;
}

bool TowerMgr::BuildGrowthDetail(proto::TowerGrowthDetailResp& rsp) const
{
    for (uint32_t detail : Bin().startowergrowth())
    {
        rsp.add_detail(detail);
    }
    while (rsp.detail_size() < 3)
    {
        rsp.add_detail(0);
    }
    return true;
}

bool TowerMgr::UnlockGrowthNode(uint32_t nodeId, proto::ChangeInfo& change)
{
    const auto it = GameData::StarTowerGrowthNodeDataTable.find(nodeId);
    if (it == GameData::StarTowerGrowthNodeDataTable.end())
    {
        return false;
    }

    if (HasGrowthNode(nodeId))
    {
        return false;
    }

    if (!GetPlayer()->Inventory().HasItem(static_cast<uint32_t>(it->second.ItemId1), it->second.ItemQty1))
    {
        return false;
    }

    if (!GetPlayer()->Inventory().RemoveItem(static_cast<uint32_t>(it->second.ItemId1), it->second.ItemQty1, &change))
    {
        return false;
    }

    auto* growth = MutableBin()->mutable_startowergrowth();
    const int groupIndex = (std::max)(static_cast<int>(it->second.Group) - 1, 0);
    while (growth->size() <= groupIndex)
    {
        growth->Add(0);
    }

    const uint32_t oldValue = growth->Get(groupIndex);
    growth->Set(groupIndex, oldValue | (1u << ((std::max)(it->second.NodeId - 1, 0))));
    return true;
}

bool TowerMgr::UnlockGrowthGroup(uint32_t groupId, proto::TowerGrowthGroupNodeUnlockResp& rsp)
{
    proto::ChangeInfo change;
    bool unlocked = false;
    for (const auto& [_, data] : GameData::StarTowerGrowthNodeDataTable)
    {
        if (static_cast<uint32_t>(data.Group) != groupId)
        {
            continue;
        }

        const uint32_t nodeId = static_cast<uint32_t>(data.Id);
        const auto before = change.props_size();
        if (!UnlockGrowthNode(nodeId, change))
        {
            continue;
        }

        rsp.add_nodes(nodeId);
        unlocked = true;
        (void)before;
    }

    if (!unlocked)
    {
        return false;
    }

    rsp.mutable_changeinfo()->CopyFrom(change);
    return true;
}

bool TowerMgr::BuildFateCardDetail(proto::TowerBookFateCardDetailResp& rsp) const
{
    for (uint32_t cardId : Bin().fatecards())
    {
        rsp.add_cards(cardId);
    }

    for (const auto& [key, data] : GameData::StarTowerBookFateCardQuestDataTable)
    {
        (void)key;
        rsp.add_quests(static_cast<uint32_t>(data.Id));
    }
    return true;
}

void TowerMgr::RecordPotentialCollection(uint32_t potentialId, uint32_t level)
{
    if (potentialId == 0 || level == 0)
    {
        return;
    }

    const auto potentialIt = GameData::PotentialDataTable.find(potentialId);
    if (potentialIt == GameData::PotentialDataTable.end())
    {
        return;
    }

    auto* levels = MutableBin()->mutable_bookpotentiallevels();
    auto& stored = (*levels)[potentialId];
    if (level > stored)
    {
        stored = level;
    }

    const uint32_t charId = static_cast<uint32_t>(potentialIt->second.CharId);
    if (std::find(Bin().bookcharids().begin(), Bin().bookcharids().end(), charId) == Bin().bookcharids().end())
    {
        MutableBin()->add_bookcharids(charId);
    }

    RebuildBookState();
}

void TowerMgr::RecordEventCollection(uint32_t eventId)
{
    if (eventId == 0)
    {
        return;
    }

    if (std::find(Bin().bookeventids().begin(), Bin().bookeventids().end(), eventId) == Bin().bookeventids().end())
    {
        MutableBin()->add_bookeventids(eventId);
        RebuildBookState();
    }
}

void TowerMgr::RecordFateCardCollection(uint32_t cardId)
{
    if (cardId == 0)
    {
        return;
    }

    if (std::find(Bin().fatecards().begin(), Bin().fatecards().end(), cardId) == Bin().fatecards().end())
    {
        MutableBin()->add_fatecards(cardId);
    }

    const auto fateIt = GameData::FateCardDataTable.find(cardId);
    if (fateIt != GameData::FateCardDataTable.end() && fateIt->second.BundleId > 0)
    {
        const uint32_t bundleId = static_cast<uint32_t>(fateIt->second.BundleId);
        if (std::find(Bin().bookbundles().begin(), Bin().bookbundles().end(), bundleId) == Bin().bookbundles().end())
        {
            MutableBin()->add_bookbundles(bundleId);
        }
    }

    RebuildBookState();
    PushFateCardCollectNotify(cardId);
}

int TowerMgr::FindNpcAffinityIndex(uint32_t npcId) const
{
    const auto* npcIds = &Bin().npcaffinitybooknpcids();
    for (int i = 0; i < npcIds->size(); ++i)
    {
        if (npcIds->Get(i) == npcId)
        {
            return i;
        }
    }
    return -1;
}

uint32_t TowerMgr::AddNpcAffinity(uint32_t npcId, uint32_t increase, proto::NPCAffinityChange* outChange)
{
    if (npcId == 0 || increase == 0)
    {
        return GetNpcAffinityValue(npcId);
    }

    int index = FindNpcAffinityIndex(npcId);
    if (index < 0)
    {
        MutableBin()->add_npcaffinitybooknpcids(npcId);
        MutableBin()->add_npcaffinitybooklevels(0);
        MutableBin()->add_npcaffinitybookvalues(0);
        index = Bin().npcaffinitybooknpcids_size() - 1;
    }

    auto* values = MutableBin()->mutable_npcaffinitybookvalues();
    auto* levels = MutableBin()->mutable_npcaffinitybooklevels();
    uint32_t nextValue = values->Get(index) + increase;
    values->Set(index, nextValue);

    uint32_t nextLevel = levels->Get(index);
    for (const auto& [_, group] : GameData::NPCAffinityGroupDataTable)
    {
        if (group.AffinityGroupId <= 0)
        {
            continue;
        }

        if (nextValue >= static_cast<uint32_t>((std::max)(group.AffinityValue, 0)))
        {
            nextLevel = (std::max)(nextLevel, static_cast<uint32_t>((std::max)(group.Level, 0)));
        }
    }
    levels->Set(index, nextLevel);

    if (outChange)
    {
        outChange->set_npcid(npcId);
        outChange->set_affinity(nextValue);
        outChange->set_increase(increase);
    }

    return nextValue;
}

bool TowerMgr::BuildNpcAffinityBook(proto::NPCAffinityBookGetResp& rsp) const
{
    const auto* npcIds = &Bin().npcaffinitybooknpcids();
    const auto* levels = &Bin().npcaffinitybooklevels();
    const auto* values = &Bin().npcaffinitybookvalues();

    const int count = (std::min)((std::min)(npcIds->size(), levels->size()), values->size());
    rsp.set_number(static_cast<uint32_t>(count));

    for (int i = 0; i < count; ++i)
    {
        auto* info = rsp.add_infos();
        const uint32_t npcId = npcIds->Get(i);
        info->set_npcid(npcId);
        info->set_affinity(values->Get(i));

        const auto plotIds = GetNpcAffinityPlotIds(npcId);
        for (uint32_t plotId : plotIds)
        {
            const auto plotIt = GameData::NPCAffinityPlotDataTable.find(plotId);
            if (plotIt == GameData::NPCAffinityPlotDataTable.end())
            {
                continue;
            }

            if (static_cast<uint32_t>((std::max)(plotIt->second.AffinityLevel, 0)) <= levels->Get(i))
            {
                info->add_plotids(plotId);
            }
        }
    }

    return true;
}

bool TowerMgr::ReceiveNpcAffinityPlotReward(uint32_t plotId, proto::NPCAffinityPlotRewardReceiveResp& rsp)
{
    if (HasReceivedNpcPlot(plotId))
    {
        return false;
    }

    const auto plotIt = GameData::NPCAffinityPlotDataTable.find(plotId);
    if (plotIt == GameData::NPCAffinityPlotDataTable.end())
    {
        return false;
    }

    const uint32_t npcLevel = GetNpcAffinityLevel(static_cast<uint32_t>(plotIt->second.NPCId));
    if (npcLevel < static_cast<uint32_t>((std::max)(plotIt->second.AffinityLevel, 0)))
    {
        return false;
    }

    proto::ChangeInfo change;
    if (plotIt->second.ItemId > 0 && plotIt->second.ItemQty > 0)
    {
        if (!GetPlayer()->Inventory().AddItem(static_cast<uint32_t>(plotIt->second.ItemId), plotIt->second.ItemQty, &change))
        {
            return false;
        }

        auto* show = rsp.add_show();
        show->set_tid(static_cast<uint32_t>(plotIt->second.ItemId));
        show->set_qty(plotIt->second.ItemQty);
    }

    rsp.mutable_change()->CopyFrom(change);
    SetReceivedNpcPlot(plotId);
    PushNpcAffinityNotify(static_cast<uint32_t>(plotIt->second.NPCId), GetNpcAffinityValue(static_cast<uint32_t>(plotIt->second.NPCId)), 0);
    return true;
}

bool TowerMgr::BuildPotentialBriefList(proto::StarTowerBookPotentialBriefListResp& rsp) const
{
    for (const auto& [_, data] : GameData::CharPotentialDataTable)
    {
        auto* info = rsp.add_infos();
        info->set_charid(static_cast<uint32_t>(data.Id));

        uint32_t count = 0;
        for (const auto& [potentialId, potential] : GameData::PotentialDataTable)
        {
            if (potential.CharId != data.Id)
            {
                continue;
            }

            const auto it = Bin().bookpotentiallevels().find(static_cast<uint32_t>(potentialId));
            if (it != Bin().bookpotentiallevels().end() && it->second > 0)
            {
                ++count;
            }
        }
        info->set_count(count);
    }

    return true;
}

bool TowerMgr::BuildCharPotential(uint32_t charId, proto::StarTowerBookPotentialGetResp& rsp) const
{
    const auto charIt = GameData::CharPotentialDataTable.find(charId);
    if (charIt == GameData::CharPotentialDataTable.end())
    {
        return false;
    }

    for (const auto& [_, potential] : GameData::PotentialDataTable)
    {
        if (potential.CharId != static_cast<int>(charId))
        {
            continue;
        }

        auto* info = rsp.add_potentials();
        info->set_id(static_cast<uint32_t>(potential.Id));
        const auto it = Bin().bookpotentiallevels().find(static_cast<uint32_t>(potential.Id));
        info->set_level(it == Bin().bookpotentiallevels().end() ? 0u : it->second);
    }

    for (uint32_t id : Bin().bookpotentialreceivedids())
    {
        rsp.add_receivedids(id);
    }
    return true;
}

bool TowerMgr::ReceivePotentialBookReward(uint32_t potentialId, proto::StarTowerBookPotentialRewardReceiveResp& rsp)
{
    if (HasReceivedPotentialBookReward(potentialId))
    {
        return false;
    }

    const auto it = GameData::PotentialDataTable.find(potentialId);
    const auto ownedIt = Bin().bookpotentiallevels().find(potentialId);
    if (it == GameData::PotentialDataTable.end() || ownedIt == Bin().bookpotentiallevels().end() || ownedIt->second == 0)
    {
        return false;
    }

    proto::ChangeInfo change;
    AddRewardToChange(change, static_cast<uint32_t>(GameConstants::TowerCoinItemId), 10);
    GetPlayer()->Inventory().AddItem(GameConstants::TowerCoinItemId, 10, rsp.mutable_change());
    rsp.mutable_change()->MergeFrom(change);

    SetReceivedPotentialBookReward(potentialId);
    for (uint32_t id : Bin().bookpotentialreceivedids())
    {
        rsp.add_receivedids(id);
    }
    PushBookPotentialNotify(potentialId);
    return true;
}

bool TowerMgr::ReceiveEventBookReward(uint32_t eventId, proto::StarTowerBookEventRewardReceiveResp& rsp)
{
    if (HasReceivedEventBookReward(eventId))
    {
        return false;
    }

    if (GameData::StarTowerEventDataTable.find(eventId) == GameData::StarTowerEventDataTable.end())
    {
        return false;
    }
    if (std::find(Bin().bookeventids().begin(), Bin().bookeventids().end(), eventId) == Bin().bookeventids().end())
    {
        return false;
    }

    GetPlayer()->Inventory().AddItem(GameConstants::TowerCoinItemId, 10, rsp.mutable_change());
    SetReceivedEventBookReward(eventId);
    for (uint32_t id : Bin().bookeventreceivedids())
    {
        rsp.add_receivedids(id);
    }
    PushBookEventNotify(eventId);
    return true;
}

bool TowerMgr::ReceiveFateCardReward(uint32_t bundleId, uint32_t questId, proto::ChangeInfo& outChange)
{
    if (bundleId == 0 && questId == 0)
    {
        return false;
    }

    const uint32_t receivedKey = bundleId > 0 ? bundleId : questId;
    if (HasReceivedFateCardReward(receivedKey))
    {
        return false;
    }

    if (bundleId > 0)
    {
        if (GameData::StarTowerBookFateCardBundleDataTable.find(bundleId) == GameData::StarTowerBookFateCardBundleDataTable.end())
        {
            return false;
        }
        if (std::find(Bin().bookbundles().begin(), Bin().bookbundles().end(), bundleId) == Bin().bookbundles().end())
        {
            return false;
        }
    }
    if (questId > 0)
    {
        if (GameData::StarTowerBookFateCardQuestDataTable.find(questId) == GameData::StarTowerBookFateCardQuestDataTable.end())
        {
            return false;
        }
    }

    GetPlayer()->Inventory().AddItem(GameConstants::TowerCoinItemId, 10, &outChange);
    SetReceivedFateCardReward(receivedKey);
    PushFateCardRewardNotify(receivedKey, questId > 0);
    return true;
}

bool TowerMgr::HasGrowthNode(uint32_t nodeId) const
{
    const auto it = GameData::StarTowerGrowthNodeDataTable.find(nodeId);
    if (it == GameData::StarTowerGrowthNodeDataTable.end())
    {
        return false;
    }

    const int groupIndex = (std::max)(static_cast<int>(it->second.Group) - 1, 0);
    if (groupIndex >= Bin().startowergrowth_size())
    {
        return false;
    }

    const uint32_t mask = 1u << ((std::max)(static_cast<int>(it->second.NodeId) - 1, 0));
    return (Bin().startowergrowth(groupIndex) & mask) != 0;
}

bool TowerMgr::IsValidPresetForCharacters(uint64_t presetId, const google::protobuf::RepeatedField<uint32_t>& charIds) const
{
    std::vector<uint32_t> ids(charIds.begin(), charIds.end());
    return IsValidPresetForCharacters(presetId, ids);
}

bool TowerMgr::IsValidPresetForCharacters(uint64_t presetId, const std::vector<uint32_t>& charIds) const
{
    const auto* preset = FindPreset(presetId);
    if (!preset)
    {
        return false;
    }

    for (uint32_t charId : charIds)
    {
        bool found = false;
        for (const auto& [presetCharId, _] : preset->CharPotentials)
        {
            if (presetCharId == charId)
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            return false;
        }
    }
    return true;
}

uint32_t TowerMgr::GetTowerTickets() const
{
    return Bin().towertickets();
}

void TowerMgr::ResetWeeklyTickets()
{
    MutableBin()->set_towertickets(0);
}

TowerRuntime::Build* TowerMgr::FindBuild(uint64_t buildId)
{
    for (auto& build : mBuilds)
    {
        if (build.Uid == buildId)
        {
            return &build;
        }
    }
    return nullptr;
}

const TowerRuntime::Build* TowerMgr::FindBuild(uint64_t buildId) const
{
    for (const auto& build : mBuilds)
    {
        if (build.Uid == buildId)
        {
            return &build;
        }
    }
    return nullptr;
}

TowerRuntime::Preset* TowerMgr::FindPreset(uint64_t presetId)
{
    for (auto& preset : mPresets)
    {
        if (preset.Uid == presetId)
        {
            return &preset;
        }
    }
    return nullptr;
}

const TowerRuntime::Preset* TowerMgr::FindPreset(uint64_t presetId) const
{
    for (const auto& preset : mPresets)
    {
        if (preset.Uid == presetId)
        {
            return &preset;
        }
    }
    return nullptr;
}

bool TowerMgr::UpdatePresetCharacters(TowerRuntime::Preset& preset, const google::protobuf::RepeatedPtrField<proto::StarTowerBookCharPotential>& chars)
{
    preset.CharPotentials.clear();
    for (const auto& ch : chars)
    {
        std::vector<TowerRuntime::PotentialInfo> list;
        for (const auto& potential : ch.potentials())
        {
            const auto it = GameData::PotentialDataTable.find(potential.id());
            if (it == GameData::PotentialDataTable.end())
            {
                continue;
            }

            TowerRuntime::PotentialInfo* existingPotential = nullptr;
            for (auto& entry : list)
            {
                if (entry.Id == potential.id())
                {
                    existingPotential = &entry;
                    break;
                }
            }
            if (!existingPotential)
            {
                list.push_back({ potential.id(), potential.level() });
            }
            else
            {
                existingPotential->Level = potential.level();
            }
        }

        std::vector<TowerRuntime::PotentialInfo>* existingCharPotentials = nullptr;
        for (auto& entry : preset.CharPotentials)
        {
            if (entry.first == ch.charid())
            {
                existingCharPotentials = &entry.second;
                break;
            }
        }
        if (!existingCharPotentials)
        {
            preset.CharPotentials.emplace_back(ch.charid(), std::move(list));
        }
        else
        {
            *existingCharPotentials = std::move(list);
        }
    }

    return true;
}

uint32_t TowerMgr::GetWeeklyTowerTicketLimit() const
{
    if (HasGrowthNode(kWeeklyTicketNodeLarge))
    {
        return 3000;
    }
    if (HasGrowthNode(kWeeklyTicketNodeSmall))
    {
        return 2500;
    }
    return 2000;
}

uint32_t TowerMgr::GetMaxEarnableWeeklyTowerTickets() const
{
    const uint32_t current = GetTowerTickets();
    const uint32_t limit = GetWeeklyTowerTicketLimit();
    return current >= limit ? 0 : (limit - current);
}

void TowerMgr::AddWeeklyTowerTickets(uint32_t count)
{
    if (count == 0)
    {
        return;
    }

    MutableBin()->set_towertickets(Bin().towertickets() + count);
}

void TowerMgr::RebuildBookState()
{
    auto* bookChars = MutableBin()->mutable_bookcharids();
    auto* bookEvents = MutableBin()->mutable_bookeventids();
    auto* bundles = MutableBin()->mutable_bookbundles();

    std::vector<uint32_t> sortedChars(bookChars->begin(), bookChars->end());
    std::sort(sortedChars.begin(), sortedChars.end());
    bookChars->Clear();
    for (uint32_t id : sortedChars)
    {
        bookChars->Add(id);
    }

    std::vector<uint32_t> sortedEvents(bookEvents->begin(), bookEvents->end());
    std::sort(sortedEvents.begin(), sortedEvents.end());
    bookEvents->Clear();
    for (uint32_t id : sortedEvents)
    {
        bookEvents->Add(id);
    }

    std::vector<uint32_t> sortedBundles(bundles->begin(), bundles->end());
    std::sort(sortedBundles.begin(), sortedBundles.end());
    bundles->Clear();
    for (uint32_t id : sortedBundles)
    {
        bundles->Add(id);
    }
}

std::vector<uint32_t> TowerMgr::GetNpcAffinityPlotIds(uint32_t npcId) const
{
    std::vector<uint32_t> ids;
    for (const auto& [_, plot] : GameData::NPCAffinityPlotDataTable)
    {
        if (static_cast<uint32_t>(plot.NPCId) == npcId)
        {
            ids.push_back(static_cast<uint32_t>(plot.Id));
        }
    }

    std::sort(ids.begin(), ids.end());
    return ids;
}

uint32_t TowerMgr::GetNpcAffinityValue(uint32_t npcId) const
{
    const auto* ids = &Bin().npcaffinitybooknpcids();
    const auto* values = &Bin().npcaffinitybookvalues();
    const int count = (std::min)(ids->size(), values->size());
    for (int i = 0; i < count; ++i)
    {
        if (ids->Get(i) == npcId)
        {
            return values->Get(i);
        }
    }
    return 0;
}

uint32_t TowerMgr::GetNpcAffinityLevel(uint32_t npcId) const
{
    const auto* ids = &Bin().npcaffinitybooknpcids();
    const auto* levels = &Bin().npcaffinitybooklevels();
    const int count = (std::min)(ids->size(), levels->size());
    for (int i = 0; i < count; ++i)
    {
        if (ids->Get(i) == npcId)
        {
            return levels->Get(i);
        }
    }
    return 0;
}

bool TowerMgr::HasReceivedNpcPlot(uint32_t plotId) const
{
    return std::find(Bin().npcaffinityrewardreceivedplotids().begin(), Bin().npcaffinityrewardreceivedplotids().end(), plotId) != Bin().npcaffinityrewardreceivedplotids().end();
}

void TowerMgr::SetReceivedNpcPlot(uint32_t plotId)
{
    if (!HasReceivedNpcPlot(plotId))
    {
        MutableBin()->add_npcaffinityrewardreceivedplotids(plotId);
    }
}

bool TowerMgr::HasReceivedPotentialBookReward(uint32_t id) const
{
    return std::find(Bin().bookpotentialreceivedids().begin(), Bin().bookpotentialreceivedids().end(), id) != Bin().bookpotentialreceivedids().end();
}

void TowerMgr::SetReceivedPotentialBookReward(uint32_t id)
{
    if (!HasReceivedPotentialBookReward(id))
    {
        MutableBin()->add_bookpotentialreceivedids(id);
    }
}

bool TowerMgr::HasReceivedEventBookReward(uint32_t id) const
{
    return std::find(Bin().bookeventreceivedids().begin(), Bin().bookeventreceivedids().end(), id) != Bin().bookeventreceivedids().end();
}

void TowerMgr::SetReceivedEventBookReward(uint32_t id)
{
    if (!HasReceivedEventBookReward(id))
    {
        MutableBin()->add_bookeventreceivedids(id);
    }
}

bool TowerMgr::HasReceivedFateCardReward(uint32_t id) const
{
    return std::find(Bin().fatecardrewardreceivedids().begin(), Bin().fatecardrewardreceivedids().end(), id) != Bin().fatecardrewardreceivedids().end();
}

void TowerMgr::SetReceivedFateCardReward(uint32_t id)
{
    if (!HasReceivedFateCardReward(id))
    {
        MutableBin()->add_fatecardrewardreceivedids(id);
    }
}

void TowerMgr::PushBookPotentialNotify(uint32_t id) const
{
    if (!GetPlayer())
    {
        return;
    }

    proto::StarTowerBookPotentialChange notify;
    for (const auto& [_, potential] : GameData::PotentialDataTable)
    {
        if (static_cast<uint32_t>(potential.Id) != id)
        {
            continue;
        }

        auto* charInfo = notify.add_charpotentials();
        charInfo->set_charid(static_cast<uint32_t>(potential.CharId));
        auto* book = charInfo->add_potentials();
        book->set_id(static_cast<uint32_t>(potential.Id));
        book->set_level(1);
        notify.add_charids(static_cast<uint32_t>(potential.CharId));
        break;
    }

    GetPlayer()->PushNextPackage(star_tower_book_potential_notify, notify);
}

void TowerMgr::PushBookEventNotify(uint32_t id) const
{
    if (!GetPlayer())
    {
        return;
    }

    proto::StarTowerBookEventChange notify;
    notify.add_eventids(id);
    GetPlayer()->PushNextPackage(star_tower_book_event_notify, notify);
}

void TowerMgr::PushNpcAffinityNotify(uint32_t npcId, uint32_t affinity, uint32_t increase) const
{
    if (!GetPlayer())
    {
        return;
    }

    proto::NPCAffinityChange notify;
    notify.set_npcid(npcId);
    notify.set_affinity(affinity);
    notify.set_increase(increase);
    GetPlayer()->PushNextPackage(change_npc_affinity_notify, notify);
}

void TowerMgr::PushFateCardCollectNotify(uint32_t cardId) const
{
    if (!GetPlayer())
    {
        return;
    }

    proto::TowerBookFateCardCollectNotify notify;
    notify.add_cards(cardId);
    GetPlayer()->PushNextPackage(tower_book_fate_card_collect_notify, notify);
}

void TowerMgr::PushFateCardRewardNotify(uint32_t id, bool questReward) const
{
    if (!GetPlayer())
    {
        return;
    }

    proto::TowerBookFateCardRewardChangeNotify notify;
    notify.add_list(id);
    notify.set_option(questReward);
    GetPlayer()->PushNextPackage(tower_book_fate_card_reward_notify, notify);
}

void TowerMgr::RebuildNpcAffinityBookState()
{
    auto* npcIds = MutableBin()->mutable_npcaffinitybooknpcids();
    auto* levels = MutableBin()->mutable_npcaffinitybooklevels();
    auto* values = MutableBin()->mutable_npcaffinitybookvalues();

    if (npcIds->empty())
    {
        std::vector<uint32_t> ids;
        for (const auto& [_, plot] : GameData::NPCAffinityPlotDataTable)
        {
            const uint32_t npcId = static_cast<uint32_t>(plot.NPCId);
            if (std::find(ids.begin(), ids.end(), npcId) == ids.end())
            {
                ids.push_back(npcId);
            }
        }

        std::sort(ids.begin(), ids.end());
        for (uint32_t id : ids)
        {
            npcIds->Add(id);
            levels->Add(0);
            values->Add(0);
        }
    }
}
