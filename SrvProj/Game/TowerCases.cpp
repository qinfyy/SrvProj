#include "TowerCases.h"

#include "ChangeInfoUtil.h"
#include "InventoryMgr.h"
#include "Player.h"
#include "TowerMgr.h"
#include "TowerRooms.h"
#include "../GameConstants.h"
#include "../Resources/BinClass/ItemsRes.h"
#include "../Resources/BinClass/StarTowerRes.h"
#include "../Resources/GameData.h"
#include "../proto/proto_cpp/public.pb.h"

#include <algorithm>
#include <random>

namespace
{
constexpr uint32_t kTowerCoinItemId = 11;

double RandomDouble()
{
    static thread_local std::mt19937 rng{ std::random_device{}() };
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}

bool RandomChance(double chance)
{
    return chance > 0.0 && RandomDouble() < chance;
}

int RandomInt(int minValue, int maxValue)
{
    if (maxValue <= minValue)
    {
        return minValue;
    }

    static thread_local std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<int> dist(minValue, maxValue);
    return dist(rng);
}
}

void TowerCaseBase::Register(TowerRoom* room, uint32_t id)
{
    mRoom = room;
    mGame = room ? room->GetGame() : nullptr;
    mId = id;
    OnRegister();
}

std::unique_ptr<TowerCaseBase> TowerCaseBase::LoadFromBin(TowerRuntime::Game* game, const ServerProto::TowerCaseBin& bin)
{
    std::unique_ptr<TowerCaseBase> towerCase;
    switch (static_cast<TowerCaseType>(bin.type()))
    {
    case TowerCaseType::Battle:
    {
        auto instance = std::make_unique<TowerBattleCase>();
        if (bin.has_battlecase())
        {
            instance->SubNoteDrops = bin.battlecase().subnotedrops();
            instance->ExpReward = bin.battlecase().expreward();
        }
        towerCase = std::move(instance);
        break;
    }
    case TowerCaseType::Door:
    {
        auto instance = std::make_unique<TowerDoorCase>();
        if (bin.has_doorcase())
        {
            instance->FloorNum = bin.doorcase().floornum();
            instance->RoomType = bin.doorcase().roomtype();
        }
        towerCase = std::move(instance);
        break;
    }
    case TowerCaseType::Potential:
    case TowerCaseType::SelectSpecialPotential:
    {
        auto instance = std::make_unique<TowerPotentialCase>();
        instance->Rare = static_cast<TowerCaseType>(bin.type()) == TowerCaseType::SelectSpecialPotential;
        const auto& caseBin = bin.has_selectspecialpotentialcase() ? bin.selectspecialpotentialcase() : bin.potentialcase();
        instance->TeamLevel = caseBin.teamlevel();
        instance->CharId = caseBin.charid();
        instance->Reroll = caseBin.reroll();
        instance->RerollPrice = caseBin.rerollprice();
        instance->Strengthen = caseBin.strengthen();
        instance->Rare = instance->Rare || caseBin.rare();
        for (const auto& potential : caseBin.potentials())
        {
            instance->Potentials.push_back({ potential.id(), potential.level() });
        }
        towerCase = std::move(instance);
        break;
    }
    case TowerCaseType::NpcEvent:
    {
        auto instance = std::make_unique<TowerNpcEventCase>();
        if (bin.has_npceventcase())
        {
            instance->NpcId = bin.npceventcase().npcid();
            instance->EventId = bin.npceventcase().eventid();
            instance->Completed = bin.npceventcase().completed();
            instance->Options.assign(bin.npceventcase().options().begin(), bin.npceventcase().options().end());
        }
        towerCase = std::move(instance);
        break;
    }
    case TowerCaseType::Hawker:
    {
        auto instance = std::make_unique<TowerHawkerCase>();
        if (bin.has_hawkercase())
        {
            for (const auto& goods : bin.hawkercase().goods())
            {
                TowerRuntime::ShopGoods item;
                item.Sid = goods.sid();
                item.Type = goods.type();
                item.Idx = goods.idx();
                item.GoodsId = goods.goodsid();
                item.Price = goods.price();
                item.Discount = goods.discount();
                item.CharPos = goods.charpos();
                item.Sold = goods.sold();
                instance->Goods.push_back(std::move(item));
            }
        }
        towerCase = std::move(instance);
        break;
    }
    case TowerCaseType::StrengthenMachine:
    {
        auto instance = std::make_unique<TowerStrengthenMachineCase>();
        if (bin.has_strengthenmachinecase())
        {
            instance->Free = bin.strengthenmachinecase().free();
            instance->Discount = bin.strengthenmachinecase().discount();
            instance->Times = bin.strengthenmachinecase().times();
        }
        towerCase = std::move(instance);
        break;
    }
    case TowerCaseType::RecoveryHP:
    {
        auto instance = std::make_unique<TowerRecoveryHPCase>();
        if (bin.has_recoveryhpcase())
        {
            instance->EffectId = bin.recoveryhpcase().effectid();
        }
        towerCase = std::move(instance);
        break;
    }
    case TowerCaseType::NpcRecoveryHP:
    {
        auto instance = std::make_unique<TowerNpcRecoveryHPCase>();
        if (bin.has_npcrecoveryhpcase())
        {
            instance->EffectId = bin.npcrecoveryhpcase().effectid();
        }
        towerCase = std::move(instance);
        break;
    }
    case TowerCaseType::SyncHP:
    {
        towerCase = std::make_unique<TowerSyncHPCase>();
        break;
    }
    default:
        break;
    }

    if (towerCase)
    {
        towerCase->mGame = game;
        towerCase->mId = bin.id();
    }

    return towerCase;
}

void TowerBattleCase::OnRegister()
{
    if (!GetGame())
    {
        return;
    }

    const auto towerIt = GameData::StarTowerDataTable.find(std::to_string(GetGame()->TowerId));
    if (towerIt == GameData::StarTowerDataTable.end())
    {
        SubNoteDrops = 1;
        ExpReward = 100;
        return;
    }

    for (const auto& [_, floorExp] : GameData::StarTowerFloorExpDataTable)
    {
        if (floorExp.StarTowerId != static_cast<int>(GetGame()->TowerId))
        {
            continue;
        }

        switch (GetRoom()->GetType())
        {
        case TowerRoomType::BattleRoom:
            SubNoteDrops = RandomChance(1.4) ? 1u : 0u;
            ExpReward = static_cast<uint32_t>((std::max)(floorExp.NormalExp, 0));
            break;
        case TowerRoomType::EliteBattleRoom:
            SubNoteDrops = 1;
            ExpReward = static_cast<uint32_t>((std::max)(floorExp.EliteExp, 0));
            break;
        case TowerRoomType::BossRoom:
            SubNoteDrops = 2;
            ExpReward = static_cast<uint32_t>((std::max)(floorExp.BossExp, 0));
            break;
        case TowerRoomType::FinalBossRoom:
            SubNoteDrops = 2;
            ExpReward = static_cast<uint32_t>((std::max)(floorExp.FinalBossExp, 0));
            break;
        default:
            SubNoteDrops = 1;
            ExpReward = static_cast<uint32_t>((std::max)(floorExp.NormalExp, 0));
            break;
        }
        return;
    }

    SubNoteDrops = 1;
    ExpReward = 100;
}

proto::StarTowerInteractResp TowerBattleCase::Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp)
{
    auto* battleEnd = rsp.mutable_battleendresp();
    if (req.has_battleendreq() && req.battleendreq().has_victory())
    {
        GetGame()->AddExp(ExpReward);
        int picks = GetGame()->LevelUp();
        if (picks > 0)
        {
            if (GetGame()->FloorCount == 1 || GetRoom()->GetType() == TowerRoomType::BossRoom || GetRoom()->GetType() == TowerRoomType::FinalBossRoom)
            {
                GetGame()->AddRarePotentialSelectors(1);
                --picks;
            }
        }
        if (picks > 0)
        {
            GetGame()->AddPotentialSelectors(static_cast<uint32_t>(picks));
        }
        GetGame()->AddBattleTime(req.battleendreq().victory().time());

        const auto stageIt = GameData::StarTowerStageDataTable.find(std::to_string(GetRoom()->GetStageId()));
        if (stageIt != GameData::StarTowerStageDataTable.end())
        {
            const int coin = (std::max)(stageIt->second.InteriorCurrencyQuantity, 0);
            if (coin > 0)
            {
                GetGame()->AddRuntimeItem(kTowerCoinItemId, coin, rsp.mutable_change());
            }
        }

        if (SubNoteDrops > 0)
        {
            for (uint32_t i = 0; i < SubNoteDrops; ++i)
            {
                const int itemId = GetGame()->GetRandomSubNoteId();
                if (itemId > 0)
                {
                    GetGame()->AddRuntimeItem(static_cast<uint32_t>(itemId), 3, rsp.mutable_change());
                }
            }
        }

        GetGame()->RefreshSecondarySkills(rsp.mutable_data());

        if (GetGame()->PendingRarePotentialCases > 0 || GetGame()->PendingPotentialCases > 0)
        {
            auto casePtr = std::make_unique<TowerPotentialCase>();
            casePtr->TeamLevel = GetGame()->TeamLevel;
            casePtr->Rare = GetGame()->PendingRarePotentialCases > 0;
            if (casePtr->Rare)
            {
                --GetGame()->PendingRarePotentialCases;
            }
            else
            {
                --GetGame()->PendingPotentialCases;
            }

            if (!GetGame()->CharIds.empty())
            {
                const size_t index = static_cast<size_t>(RandomInt(0, static_cast<int>(GetGame()->CharIds.size() - 1)));
                casePtr->CharId = GetGame()->CharIds[index];
            }
            casePtr->RerollPrice = 100;
            casePtr->Reroll = GetGame()->GetManager() && GetGame()->GetManager()->HasGrowthNode(20901) ? 1u : 0u;

            const auto charIt = GameData::CharPotentialDataTable.find(std::to_string(casePtr->CharId));
            if (charIt != GameData::CharPotentialDataTable.end())
            {
                auto candidates = charIt->second.GetPotentialList(!GetGame()->CharIds.empty() && GetGame()->CharIds.front() == casePtr->CharId, casePtr->Rare);
                std::shuffle(candidates.begin(), candidates.end(), std::mt19937{ std::random_device{}() });
                for (int potentialId : candidates)
                {
                    const auto potentialIt = GameData::PotentialDataTable.find(std::to_string(potentialId));
                    if (potentialIt == GameData::PotentialDataTable.end())
                    {
                        continue;
                    }
                    if (GetGame()->GetPotentialLevel(static_cast<uint32_t>(potentialId)) >= potentialIt->second.GetMaxLevel(GetGame()->GetExtraPotentialMaxLevel()))
                    {
                        continue;
                    }
                    casePtr->Potentials.push_back({ static_cast<uint32_t>(potentialId), 1u });
                    if (casePtr->Potentials.size() >= 3)
                    {
                        break;
                    }
                }
            }

            if (!casePtr->Potentials.empty())
            {
                auto* added = GetRoom()->AddCase(std::move(casePtr));
                if (added)
                {
                    rsp.add_cases()->CopyFrom(added->ToProto());
                }
            }
        }
        else if (!GetRoom()->HasDoor())
        {
            auto* added = GetRoom()->AddCase(GetRoom()->CreateDoorCase());
            if (added)
            {
                rsp.add_cases()->CopyFrom(added->ToProto());
            }
        }

        battleEnd->mutable_victory()->set_lv(GetGame()->TeamLevel);
        battleEnd->mutable_victory()->set_battletime(GetGame()->BattleTime);
    }
    else
    {
        battleEnd->mutable_defeat()->set_lv(GetGame()->TeamLevel);
        GetGame()->Settle(false, rsp);
    }
    rsp.mutable_change();
    return rsp;
}

proto::StarTowerRoomCase TowerBattleCase::ToProto() const
{
    proto::StarTowerRoomCase out;
    out.set_id(GetId());
    out.mutable_battlecase()->set_subnoteskillnum(SubNoteDrops);
    return out;
}

void TowerBattleCase::SaveToBin(ServerProto::TowerCaseBin& bin) const
{
    bin.Clear();
    bin.set_id(GetId());
    bin.set_type(static_cast<uint32_t>(GetType()));
    auto* data = bin.mutable_battlecase();
    data->set_subnotedrops(SubNoteDrops);
    data->set_expreward(ExpReward);
}

proto::StarTowerInteractResp TowerDoorCase::Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp)
{
    TowerRuntime::Game* game = GetGame();
    if (!game)
    {
        return rsp;
    }

    const auto towerIt = GameData::StarTowerDataTable.find(std::to_string(game->TowerId));
    if (towerIt == GameData::StarTowerDataTable.end())
    {
        return rsp;
    }

    if (game->IsOnFinalFloor(towerIt->second))
    {
        game->Settle(true, rsp);
        return rsp;
    }

    std::unique_ptr<TowerRoom> keepAlive = std::move(game->Room);
    if (!game->EnterNextRoom() || !game->Room)
    {
        game->Room = std::move(keepAlive);
        return rsp;
    }

    if (req.has_enterreq())
    {
        game->Room->SetMapInfo(req.enterreq().mapid(), req.enterreq().maptableid(), req.enterreq().mapparam(), req.enterreq().paramid());
    }
    rsp.mutable_enterresp()->mutable_room()->CopyFrom(game->Room->ToProto());
    return rsp;
}

proto::StarTowerRoomCase TowerDoorCase::ToProto() const
{
    proto::StarTowerRoomCase out;
    out.set_id(GetId());
    auto* data = out.mutable_doorcase();
    data->set_floor(FloorNum);
    data->set_type(RoomType);
    return out;
}

void TowerDoorCase::SaveToBin(ServerProto::TowerCaseBin& bin) const
{
    bin.Clear();
    bin.set_id(GetId());
    bin.set_type(static_cast<uint32_t>(GetType()));
    auto* data = bin.mutable_doorcase();
    data->set_floornum(FloorNum);
    data->set_roomtype(RoomType);
}

proto::StarTowerInteractResp TowerPotentialCase::Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp)
{
    auto* select = rsp.mutable_selectresp();
    if (req.has_selectreq() && req.selectreq().has_reroll())
    {
        if (Reroll == 0 || GetGame()->GetResCount(kTowerCoinItemId) < static_cast<int>(RerollPrice))
        {
            return rsp;
        }

        --Reroll;
        GetGame()->AddRuntimeItem(kTowerCoinItemId, -static_cast<int>(RerollPrice), rsp.mutable_change());
    }
    else if (req.has_selectreq())
    {
        const int index = static_cast<int>(req.selectreq().index());
        if (index >= 0 && index < static_cast<int>(Potentials.size()))
        {
            const auto selected = Potentials[static_cast<size_t>(index)];
            GetGame()->AddRuntimeItem(selected.Id, static_cast<int>(selected.Level), rsp.mutable_change());
            if (GetGame()->GetManager())
            {
                GetGame()->GetManager()->RecordPotentialCollection(selected.Id, selected.Level);
            }
        }

        GetGame()->RefreshSecondarySkills(rsp.mutable_data());

        if (GetGame()->PendingRarePotentialCases > 0 || GetGame()->PendingPotentialCases > 0)
        {
            auto nextCase = std::make_unique<TowerPotentialCase>();
            nextCase->TeamLevel = GetGame()->TeamLevel;
            nextCase->Rare = GetGame()->PendingRarePotentialCases > 0;
            if (nextCase->Rare)
            {
                --GetGame()->PendingRarePotentialCases;
            }
            else
            {
                --GetGame()->PendingPotentialCases;
            }

            if (!GetGame()->CharIds.empty())
            {
                const size_t charIndex = static_cast<size_t>(RandomInt(0, static_cast<int>(GetGame()->CharIds.size() - 1)));
                nextCase->CharId = GetGame()->CharIds[charIndex];
            }
            nextCase->RerollPrice = 100;
            nextCase->Reroll = GetGame()->GetManager() && GetGame()->GetManager()->HasGrowthNode(20901) ? 1u : 0u;

            const auto charIt = GameData::CharPotentialDataTable.find(std::to_string(nextCase->CharId));
            if (charIt != GameData::CharPotentialDataTable.end())
            {
                auto candidates = charIt->second.GetPotentialList(!GetGame()->CharIds.empty() && GetGame()->CharIds.front() == nextCase->CharId, nextCase->Rare);
                std::shuffle(candidates.begin(), candidates.end(), std::mt19937{ std::random_device{}() });
                for (int potentialId : candidates)
                {
                    const auto potentialIt = GameData::PotentialDataTable.find(std::to_string(potentialId));
                    if (potentialIt == GameData::PotentialDataTable.end())
                    {
                        continue;
                    }
                    if (GetGame()->GetPotentialLevel(static_cast<uint32_t>(potentialId)) >= potentialIt->second.GetMaxLevel(GetGame()->GetExtraPotentialMaxLevel()))
                    {
                        continue;
                    }
                    nextCase->Potentials.push_back({ static_cast<uint32_t>(potentialId), 1u });
                    if (nextCase->Potentials.size() >= 3)
                    {
                        break;
                    }
                }
            }

            if (!nextCase->Potentials.empty())
            {
                auto* added = GetRoom()->AddCase(std::move(nextCase));
                if (added)
                {
                    rsp.add_cases()->CopyFrom(added->ToProto());
                }
            }
        }
        else if (!GetRoom()->HasDoor())
        {
            auto* added = GetRoom()->AddCase(GetRoom()->CreateDoorCase());
            if (added)
            {
                rsp.add_cases()->CopyFrom(added->ToProto());
            }
        }
    }

    if (Rare)
    {
        auto* data = select->mutable_selectspecialpotentialcase();
        data->set_teamlevel(TeamLevel);
        for (const auto& potential : Potentials)
        {
            data->add_ids(potential.Id);
        }
    }
    else
    {
        auto* data = select->mutable_selectpotentialcase();
        data->set_teamlevel(TeamLevel);
        for (const auto& potential : Potentials)
        {
            auto* info = data->add_infos();
            info->set_tid(potential.Id);
            info->set_level(static_cast<int32_t>(potential.Level));
        }
    }
    rsp.mutable_change();
    return rsp;
}

proto::StarTowerRoomCase TowerPotentialCase::ToProto() const
{
    proto::StarTowerRoomCase out;
    out.set_id(GetId());
    if (Rare)
    {
        auto* data = out.mutable_selectspecialpotentialcase();
        data->set_teamlevel(TeamLevel);
        for (const auto& potential : Potentials)
        {
            data->add_ids(potential.Id);
        }
        if (Reroll > 0)
        {
            data->set_canreroll(true);
            data->set_rerollprice(RerollPrice);
        }
    }
    else
    {
        auto* data = out.mutable_selectpotentialcase();
        data->set_teamlevel(TeamLevel);
        for (const auto& potential : Potentials)
        {
            auto* info = data->add_infos();
            info->set_tid(potential.Id);
            info->set_level(static_cast<int32_t>(potential.Level));
            if (potential.Level > 1)
            {
                data->add_luckyids(potential.Id);
            }
        }
        if (Reroll > 0)
        {
            data->set_canreroll(true);
            data->set_rerollprice(RerollPrice);
        }
    }
    return out;
}

void TowerPotentialCase::SaveToBin(ServerProto::TowerCaseBin& bin) const
{
    bin.Clear();
    bin.set_id(GetId());
    bin.set_type(static_cast<uint32_t>(GetType()));
    auto* data = Rare ? bin.mutable_selectspecialpotentialcase() : bin.mutable_potentialcase();
    data->set_teamlevel(TeamLevel);
    data->set_charid(CharId);
    data->set_reroll(Reroll);
    data->set_rerollprice(RerollPrice);
    data->set_strengthen(Strengthen);
    data->set_rare(Rare);
    for (const auto& potential : Potentials)
    {
        auto* info = data->add_potentials();
        info->set_id(potential.Id);
        info->set_level(potential.Level);
    }
}

proto::StarTowerInteractResp TowerNpcEventCase::Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp)
{
    if (Completed)
    {
        return rsp;
    }

    Completed = true;
    if (GetGame()->GetManager())
    {
        GetGame()->GetManager()->RecordEventCollection(EventId);
    }
    auto* result = rsp.mutable_selectresp()->mutable_resp();
    result->set_optionsresult(true);

    if (!Options.empty())
    {
        const uint32_t selectedIndex = req.has_selectreq() ? req.selectreq().index() : 0;
        const uint32_t optionId = selectedIndex < Options.size() ? Options[selectedIndex] : Options.front();

        if (optionId >= 10101 && optionId <= 10809)
        {
            GetGame()->AddRuntimeItem(11, 30, rsp.mutable_change());
        }

        const uint32_t affinity = 10;
        (void)affinity;
    }

    if (GetGame())
    {
        const uint32_t currentAffinity = 10;
        (void)currentAffinity;
    }
    return rsp;
}

proto::StarTowerRoomCase TowerNpcEventCase::ToProto() const
{
    proto::StarTowerRoomCase out;
    out.set_id(GetId());
    auto* data = out.mutable_selectoptionseventcase();
    data->set_evtid(EventId);
    data->set_npcid(NpcId);
    data->set_done(Completed);
    for (uint32_t option : Options)
    {
        data->add_options(option);
    }
    auto* info = data->add_infos();
    info->set_npcid(NpcId);
    info->set_affinity(0);
    return out;
}

void TowerNpcEventCase::SaveToBin(ServerProto::TowerCaseBin& bin) const
{
    bin.Clear();
    bin.set_id(GetId());
    bin.set_type(static_cast<uint32_t>(GetType()));
    auto* data = bin.mutable_npceventcase();
    data->set_npcid(NpcId);
    data->set_eventid(EventId);
    data->set_completed(Completed);
    for (uint32_t option : Options)
    {
        data->add_options(option);
    }
}

void TowerHawkerCase::OnRegister()
{
    if (!Goods.empty())
    {
        return;
    }

    uint32_t total = 2;
    if (GetGame()->GetManager())
    {
        if (GetGame()->GetManager()->HasGrowthNode(20702))
        {
            total = 8;
        }
        else if (GetGame()->GetManager()->HasGrowthNode(20402))
        {
            total = 6;
        }
        else if (GetGame()->GetManager()->HasGrowthNode(10402))
        {
            total = 4;
        }
    }

    const uint32_t potentialCount = (std::max)(total / 2, 2u);
    const uint32_t subNoteCount = total > potentialCount ? total - potentialCount : 0;

    for (uint32_t i = 0; i < potentialCount; ++i)
    {
        TowerRuntime::ShopGoods goods;
        goods.Sid = static_cast<uint32_t>(Goods.size() + 1);
        goods.Type = 1;
        goods.Idx = 1;
        goods.GoodsId = 102;
        goods.Price = 200;
        if (RandomChance(0.2))
        {
            goods.CharPos = 1;
        }
        Goods.push_back(goods);
    }

    for (uint32_t i = 0; i < subNoteCount; ++i)
    {
        TowerRuntime::ShopGoods goods;
        goods.Sid = static_cast<uint32_t>(Goods.size() + 1);
        goods.Type = 2;
        goods.GoodsId = static_cast<uint32_t>((std::max)(GetGame()->GetRandomSubNoteId(), 0));
        goods.Idx = 3;
        goods.Price = 90;
        Goods.push_back(goods);
    }
}

proto::StarTowerInteractResp TowerHawkerCase::Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp)
{
    if (req.has_hawkerreq())
    {
        if (req.hawkerreq().has_sid())
        {
            const uint32_t sid = req.hawkerreq().sid();
            for (auto& goods : Goods)
            {
                if (goods.Sid != sid || goods.Sold)
                {
                    continue;
                }

                if (GetGame()->GetResCount(kTowerCoinItemId) < goods.GetPrice())
                {
                    break;
                }

                goods.Sold = true;
                GetGame()->AddRuntimeItem(kTowerCoinItemId, -goods.GetPrice(), rsp.mutable_change());
                if (goods.Type == 1)
                {
                    auto casePtr = std::make_unique<TowerPotentialCase>();
                    casePtr->TeamLevel = GetGame()->TeamLevel;
                    casePtr->CharId = goods.GetCharId(*GetGame());
                    casePtr->Potentials.push_back({ goods.GoodsId, 1 });
                    auto* added = GetRoom()->AddCase(std::move(casePtr));
                    if (added)
                    {
                        rsp.add_cases()->CopyFrom(added->ToProto());
                    }
                }
                else
                {
                    GetGame()->AddRuntimeItem(goods.GoodsId, goods.GetCount(), rsp.mutable_change());
                }
                break;
            }
        }
        else if (req.hawkerreq().has_reroll())
        {
            Goods.clear();
            OnRegister();
            GetGame()->AddRuntimeItem(kTowerCoinItemId, -100, rsp.mutable_change());
        }
    }

    rsp.mutable_selectresp()->mutable_hawkercase()->CopyFrom(ToProto().hawkercase());
    return rsp;
}

proto::StarTowerRoomCase TowerHawkerCase::ToProto() const
{
    proto::StarTowerRoomCase out;
    out.set_id(GetId());
    auto* data = out.mutable_hawkercase();
    for (const auto& goods : Goods)
    {
        auto* info = data->add_list();
        info->set_sid(goods.Sid);
        info->set_type(goods.Type);
        info->set_idx(goods.Idx);
        info->set_goodsid(goods.GoodsId);
        info->set_price(goods.GetDisplayPrice());
        if (goods.Discount > 0)
        {
            info->set_discount(goods.GetPrice());
        }
        if (goods.CharPos > 0)
        {
            info->set_charpos(goods.CharPos);
        }
        info->set_tag(1);
        if (goods.Sold)
        {
            data->add_purchase(goods.Sid);
        }
    }
    return out;
}

void TowerHawkerCase::SaveToBin(ServerProto::TowerCaseBin& bin) const
{
    bin.Clear();
    bin.set_id(GetId());
    bin.set_type(static_cast<uint32_t>(GetType()));
    auto* data = bin.mutable_hawkercase();
    for (const auto& goods : Goods)
    {
        auto* info = data->add_goods();
        info->set_sid(goods.Sid);
        info->set_type(goods.Type);
        info->set_idx(goods.Idx);
        info->set_goodsid(goods.GoodsId);
        info->set_price(goods.Price);
        info->set_discount(goods.Discount);
        info->set_charpos(goods.CharPos);
        info->set_sold(goods.Sold);
    }
}

proto::StarTowerInteractResp TowerStrengthenMachineCase::Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp)
{
    const int price = GetPrice();
    if (GetGame()->GetResCount(kTowerCoinItemId) < price)
    {
        rsp.mutable_strengthenmachineresp()->set_buysucceed(false);
        return rsp;
    }

    GetGame()->AddRuntimeItem(kTowerCoinItemId, -price, rsp.mutable_change());
    ++Times;
    Free = false;

    auto casePtr = std::make_unique<TowerPotentialCase>();
    casePtr->TeamLevel = GetGame()->TeamLevel;
    casePtr->Strengthen = true;
    for (const auto& [potentialId, level] : GetGame()->Potentials)
    {
        if (level <= 0)
        {
            continue;
        }
        casePtr->Potentials.push_back({ potentialId, 1u });
        if (casePtr->Potentials.size() >= 3)
        {
            break;
        }
    }

    if (casePtr->Potentials.empty())
    {
        rsp.mutable_strengthenmachineresp()->set_buysucceed(false);
        return rsp;
    }

    auto* added = GetRoom()->AddCase(std::move(casePtr));
    if (added)
    {
        rsp.add_cases()->CopyFrom(added->ToProto());
    }

    rsp.mutable_strengthenmachineresp()->set_buysucceed(true);
    return rsp;
}

int TowerStrengthenMachineCase::GetPrice() const
{
    if (Free)
    {
        return 0;
    }

    const int price = 120 + static_cast<int>(Times) * 60 - Discount;
    return (std::max)(price, 0);
}

proto::StarTowerRoomCase TowerStrengthenMachineCase::ToProto() const
{
    proto::StarTowerRoomCase out;
    out.set_id(GetId());
    auto* data = out.mutable_strengthenmachinecase();
    data->set_firstfree(Free);
    data->set_discount(Discount);
    data->set_times(Times);
    return out;
}

void TowerStrengthenMachineCase::SaveToBin(ServerProto::TowerCaseBin& bin) const
{
    bin.Clear();
    bin.set_id(GetId());
    bin.set_type(static_cast<uint32_t>(GetType()));
    auto* data = bin.mutable_strengthenmachinecase();
    data->set_free(Free);
    data->set_discount(Discount);
    data->set_times(Times);
}

proto::StarTowerInteractResp TowerRecoveryHPCase::Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp)
{
    rsp.mutable_nilresp();
    return rsp;
}

proto::StarTowerRoomCase TowerRecoveryHPCase::ToProto() const
{
    proto::StarTowerRoomCase out;
    out.set_id(GetId());
    out.mutable_recoveryhpcase()->set_effectid(EffectId);
    return out;
}

void TowerRecoveryHPCase::SaveToBin(ServerProto::TowerCaseBin& bin) const
{
    bin.Clear();
    bin.set_id(GetId());
    bin.set_type(static_cast<uint32_t>(GetType()));
    bin.mutable_recoveryhpcase()->set_effectid(EffectId);
}

proto::StarTowerInteractResp TowerNpcRecoveryHPCase::Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp)
{
    if (req.has_recoveryhpreq())
    {
        GetGame()->SetHp(static_cast<int>(req.recoveryhpreq().hp()));
    }
    return rsp;
}

proto::StarTowerRoomCase TowerNpcRecoveryHPCase::ToProto() const
{
    proto::StarTowerRoomCase out;
    out.set_id(GetId());
    out.mutable_npcrecoveryhpcase()->set_effectid(EffectId);
    return out;
}

void TowerNpcRecoveryHPCase::SaveToBin(ServerProto::TowerCaseBin& bin) const
{
    bin.Clear();
    bin.set_id(GetId());
    bin.set_type(static_cast<uint32_t>(GetType()));
    bin.mutable_npcrecoveryhpcase()->set_effectid(EffectId);
}

proto::StarTowerInteractResp TowerSyncHPCase::Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp)
{
    if (req.has_recoveryhpreq())
    {
        GetGame()->SetHp(static_cast<int>(req.recoveryhpreq().hp()));
    }
    return rsp;
}

proto::StarTowerRoomCase TowerSyncHPCase::ToProto() const
{
    proto::StarTowerRoomCase out;
    out.set_id(GetId());
    out.mutable_synchpcase();
    return out;
}

void TowerSyncHPCase::SaveToBin(ServerProto::TowerCaseBin& bin) const
{
    bin.Clear();
    bin.set_id(GetId());
    bin.set_type(static_cast<uint32_t>(GetType()));
    bin.mutable_synchpcase()->set_placeholder(true);
}
