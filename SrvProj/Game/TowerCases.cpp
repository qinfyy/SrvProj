#include "TowerCases.h"

#include "ChangeInfoUtil.h"
#include "AchievementMgr.h"
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
#include <cstddef>
#include <random>

namespace
{
constexpr uint32_t kTowerEventWrongAnswerOptionsParamId = 100140101;
constexpr uint32_t kTowerEventSubNoteSkillBaseId = 90010;
constexpr int kTowerEventCoinSmallReward = 30;
constexpr int kTowerEventSubNoteSmallReward = 5;
constexpr int kTowerEventSubNoteLargeReward = 10;
constexpr uint32_t kTowerNpcEventAffinityIncrease = 100;
constexpr int kTowerSubNoteSkillItemSubType = 19;

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

void TowerCaseBase::RegisterLoaded(TowerRoom* room, uint32_t id)
{
    mRoom = room;
    mGame = room ? room->GetGame() : nullptr;
    mId = id;
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
            instance->RerollTimes = bin.hawkercase().rerolltimes();
            instance->RerollPrice = bin.hawkercase().rerollprice();
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

    const auto towerIt = GameData::StarTowerDataTable.find(GetGame()->TowerId);
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
            SubNoteDrops = RandomChance(GetGame()->GetBattleSubNoteDropChance() + 0.4) ? 1u : 0u;
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
    if (req.has_battleendreq() && req.battleendreq().has_victory())
    {
        auto* battleEnd = rsp.mutable_battleendresp();
        GetGame()->AddExp(ExpReward);
        int picks = GetGame()->LevelUp();
        if (picks > 0)
        {
            // 与 Nebula 一致：首层 / Boss / 最终 Boss 优先稀有，否则 1/8 概率稀有。
            if (GetGame()->FloorCount == 1)
            {
                GetGame()->AddRarePotentialSelectors(1);
                --picks;
            }
            else if (GetRoom()->GetType() == TowerRoomType::BossRoom || GetRoom()->GetType() == TowerRoomType::FinalBossRoom)
            {
                GetGame()->AddRarePotentialSelectors(1);
                --picks;
            }
            else if (RandomChance(0.125))
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
        GetGame()->TotalDamages.assign(
            req.battleendreq().victory().damages().begin(),
            req.battleendreq().victory().damages().end());

        const auto stageIt = GameData::StarTowerStageDataTable.find(GetRoom()->GetStageId());
        if (stageIt != GameData::StarTowerStageDataTable.end())
        {
            int coin = (std::max)(stageIt->second.InteriorCurrencyQuantity, 0);
            if (RandomChance(GetGame()->GetBonusCoinChance()))
            {
                coin += static_cast<int>(GetGame()->GetBonusCoinCount());
            }
            if (coin > 0)
            {
                GetGame()->AddRuntimeItem(GameConstants::TowerCoinItemId, coin, rsp.mutable_change());
            }
        }

        uint32_t subNoteDrops = SubNoteDrops;
        if (GetRoom()->GetType() == TowerRoomType::BossRoom && GetGame()->GetBonusBossSubNotes() > 0 && RandomChance(0.5))
        {
            subNoteDrops += GetGame()->GetBonusBossSubNotes();
        }
        if (RandomChance(GetGame()->GetBonusSubNoteChance()))
        {
            subNoteDrops += GetGame()->GetBonusSubNotes();
        }

        // 与 Nebula 一致：先挂 pending 潜能/门，再发副音符（都写入 change，顺序对齐）。
        GetGame()->HandlePendingPotentialSelectors(rsp);

        if (subNoteDrops > 0)
        {
            for (uint32_t i = 0; i < subNoteDrops; ++i)
            {
                const int itemId = GetGame()->GetRandomSubNoteId();
                if (itemId > 0)
                {
                    GetGame()->AddRuntimeItem(static_cast<uint32_t>(itemId), 3, rsp.mutable_change());
                }
            }
        }

        GetGame()->RefreshSecondarySkills(rsp.mutable_data());

        battleEnd->mutable_victory()->set_lv(GetGame()->TeamLevel);
        battleEnd->mutable_victory()->set_battletime(GetGame()->BattleTime);
        if (GetGame()->GetManager() && GetGame()->GetManager()->GetPlayer() && req.battleendreq().victory().has_events())
        {
            GetGame()->GetManager()->GetPlayer()->Achievements().HandleClientEvents(req.battleendreq().victory().events());
        }
    }
    else
    {
        // Nebula 失败时只返回结算结构，不额外填充 BattleEndResp。
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

    const auto towerIt = GameData::StarTowerDataTable.find(game->TowerId);
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
    if (req.has_selectreq() && req.selectreq().has_reroll())
    {
        if (Reroll == 0 || GetGame()->GetResCount(GameConstants::TowerCoinItemId) < static_cast<int>(RerollPrice))
        {
            return rsp;
        }

        std::unique_ptr<TowerCaseBase> rerollBase;
        if (Strengthen)
        {
            rerollBase = GetGame()->CreateStrengthenSelector();
        }
        else
        {
            rerollBase = GetGame()->CreatePotentialSelector(CharId, Rare);
        }

        auto* rerollCase = dynamic_cast<TowerPotentialCase*>(rerollBase.get());
        if (!rerollCase || rerollCase->Potentials.empty())
        {
            return rsp;
        }

        --Reroll;
        rerollCase->Reroll = Reroll;
        rerollCase->RerollPrice = RerollPrice;
        rerollCase->Strengthen = Strengthen;
        rerollCase->Rare = Rare;
        GetGame()->AddRuntimeItem(GameConstants::TowerCoinItemId, -static_cast<int>(RerollPrice), rsp.mutable_change());
        auto* added = GetRoom()->AddCase(std::move(rerollBase));
        if (added)
        {
            rsp.add_cases()->CopyFrom(added->ToProto());
        }
        if (GetGame()->GetManager() && GetGame()->GetManager()->GetPlayer())
        {
            GetGame()->GetManager()->GetPlayer()->Trigger(529, 1, 0, 0);
        }
    }
    else if (req.has_selectreq())
    {
        // 与 Nebula 一致：非法 index 直接返回，不推进 pending/door。
        const int index = static_cast<int>(req.selectreq().index());
        if (index < 0 || index >= static_cast<int>(Potentials.size()))
        {
            rsp.mutable_change();
            return rsp;
        }

        const auto selected = Potentials[static_cast<size_t>(index)];
        if (selected.Level > 1 && GetGame()->GetManager() && GetGame()->GetManager()->GetPlayer())
        {
            const uint32_t triggerId = GetGame()->GetPotentialLevel(selected.Id) > 0 ? 534u : 533u;
            GetGame()->GetManager()->GetPlayer()->Trigger(triggerId, 1, 0, 0);
        }
        // 与 Nebula 一致：无论 add 是否因满级等失败，都继续 handlePending，避免卡死。
        if (GetGame()->AddRuntimeItem(selected.Id, static_cast<int>(selected.Level), rsp.mutable_change()) && GetGame()->GetManager())
        {
            GetGame()->GetManager()->RecordPotentialCollection(selected.Id, GetGame()->GetPotentialLevel(selected.Id));
        }

        GetGame()->RefreshSecondarySkills(rsp.mutable_data());
        GetGame()->HandlePendingPotentialSelectors(rsp);
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

    auto* result = rsp.mutable_selectresp()->mutable_resp();
    bool completed = true;

    if (!Options.empty())
    {
        const uint32_t selectedIndex = req.has_selectreq() ? req.selectreq().index() : 0;
        const uint32_t optionId = selectedIndex < Options.size() ? Options[selectedIndex] : 0;

        switch (optionId)
        {
        // 消耗100星塔币，生成普通潜能选择。
        case 10101:
            if (GetGame()->GetResCount(GameConstants::TowerCoinItemId) >= 100)
            {
                GetGame()->AddRuntimeItem(GameConstants::TowerCoinItemId, -100, rsp.mutable_change());
                auto selector = GetGame()->CreatePotentialSelector(0, false);
                if (selector)
                {
                    auto* added = GetRoom()->AddCase(std::move(selector));
                    if (added)
                    {
                        rsp.add_cases()->CopyFrom(added->ToProto());
                    }
                }
            }
            else
            {
                completed = false;
            }
            break;
        // 消耗120星塔币，生成普通潜能选择。
        case 10102:
            if (GetGame()->GetResCount(GameConstants::TowerCoinItemId) >= 120)
            {
                GetGame()->AddRuntimeItem(GameConstants::TowerCoinItemId, -120, rsp.mutable_change());
                auto selector = GetGame()->CreatePotentialSelector(0, false);
                if (selector)
                {
                    auto* added = GetRoom()->AddCase(std::move(selector));
                    if (added)
                    {
                        rsp.add_cases()->CopyFrom(added->ToProto());
                    }
                }
            }
            else
            {
                completed = false;
            }
            break;
        // 获得30星塔币。
        case 10103:
        // 获得30星塔币。
        case 10204:
        // 获得30星塔币。
        case 10303:
        // 获得30星塔币。
        case 10403:
        // 获得30星塔币。
        case 10503:
        // 获得30星塔币。
        case 10603:
        // 获得30星塔币。
        case 10809:
        // 获得30星塔币。
        case 12802:
            GetGame()->AddRuntimeItem(GameConstants::TowerCoinItemId, kTowerEventCoinSmallReward, rsp.mutable_change());
            break;
        // 消耗120星塔币，为支援角色生成普通潜能选择。
        case 10201:
            if (GetGame()->GetResCount(GameConstants::TowerCoinItemId) >= 120)
            {
                GetGame()->AddRuntimeItem(GameConstants::TowerCoinItemId, -120, rsp.mutable_change());
                uint32_t supportCharId = 0;
                if (GetGame()->CharIds.size() <= 1)
                {
                    supportCharId = GetGame()->CharIds.empty() ? 0u : GetGame()->CharIds.front();
                }
                else
                {
                    const size_t index = static_cast<size_t>(RandomInt(1, static_cast<int>(GetGame()->CharIds.size() - 1)));
                    supportCharId = GetGame()->CharIds[index];
                }

                auto selector = GetGame()->CreatePotentialSelector(supportCharId, false);
                if (selector)
                {
                    auto* added = GetRoom()->AddCase(std::move(selector));
                    if (added)
                    {
                        rsp.add_cases()->CopyFrom(added->ToProto());
                    }
                }
            }
            else
            {
                completed = false;
            }
            break;
        // 消耗160星塔币，为队长生成普通潜能选择。
        case 10202:
            if (GetGame()->GetResCount(GameConstants::TowerCoinItemId) >= 160)
            {
                GetGame()->AddRuntimeItem(GameConstants::TowerCoinItemId, -160, rsp.mutable_change());
                auto selector = GetGame()->CreatePotentialSelector(GetGame()->CharIds.empty() ? 0u : GetGame()->CharIds.front(), false);
                if (selector)
                {
                    auto* added = GetRoom()->AddCase(std::move(selector));
                    if (added)
                    {
                        rsp.add_cases()->CopyFrom(added->ToProto());
                    }
                }
            }
            else
            {
                completed = false;
            }
            break;
        // 消耗200星塔币，生成稀有潜能选择。
        case 10203:
        // 1消耗200星塔币，生成稀有潜能选择。
        case 10402:
            if (GetGame()->GetResCount(GameConstants::TowerCoinItemId) >= 200)
            {
                GetGame()->AddRuntimeItem(GameConstants::TowerCoinItemId, -200, rsp.mutable_change());
                auto selector = GetGame()->CreatePotentialSelector(0, true);
                if (selector)
                {
                    auto* added = GetRoom()->AddCase(std::move(selector));
                    if (added)
                    {
                        rsp.add_cases()->CopyFrom(added->ToProto());
                    }
                }
            }
            else
            {
                completed = false;
            }
            break;
        // 暂未实现消耗副音符兑换星塔币，固定判定未满足。
        case 10302:
        // 暂未实现该事件条件，固定判定未满足。
        case 10401:
            completed = false;
            break;
        // 50%获得200星塔币，否则失去100星塔币。
        case 10501:
            GetGame()->AddRuntimeItem(GameConstants::TowerCoinItemId, RandomChance(0.5) ? 200 : -100, rsp.mutable_change());
            break;
        // 30%获得650星塔币，否则失去200星塔币。
        case 10502:
            GetGame()->AddRuntimeItem(GameConstants::TowerCoinItemId, RandomChance(0.3) ? 650 : -200, rsp.mutable_change());
            break;
        // 50%生成稀有潜能选择。
        case 10601:
            if (RandomChance(0.5))
            {
                auto selector = GetGame()->CreatePotentialSelector(0, true);
                if (selector)
                {
                    auto* added = GetRoom()->AddCase(std::move(selector));
                    if (added)
                    {
                        rsp.add_cases()->CopyFrom(added->ToProto());
                    }
                }
            }
            break;
        // 生成普通潜能选择。
        case 10602:
        {
            auto selector = GetGame()->CreatePotentialSelector(0, false);
            if (selector)
            {
                auto* added = GetRoom()->AddCase(std::move(selector));
                if (added)
                {
                    rsp.add_cases()->CopyFrom(added->ToProto());
                }
            }
            break;
        }
        // 获得固定副音符技能90011，数量5。
        case 10701:
        // 获得固定副音符技能90012，数量5。
        case 10702:
        // 获得固定副音符技能90013，数量5。
        case 10703:
        // 获得固定副音符技能90014，数量5。
        case 10704:
        // 获得固定副音符技能90015，数量5。
        case 10705:
        // 获得固定副音符技能90016，数量5。
        case 10706:
        // 获得固定副音符技能90017，数量5。
        case 10707:
            GetGame()->AddRuntimeItem((optionId % 100) + kTowerEventSubNoteSkillBaseId, kTowerEventSubNoteSmallReward, rsp.mutable_change());
            break;
        // 获得随机副音符技能，数量5。
        case 10708:
            GetGame()->AddRuntimeItem(static_cast<uint32_t>((std::max)(GetGame()->GetRandomSubNoteId(), 0)), kTowerEventSubNoteSmallReward, rsp.mutable_change());
            break;
        // 消耗140星塔币，获得固定副音符技能90011，数量10。
        case 10801:
        // 消耗140星塔币，获得固定副音符技能90012，数量10。
        case 10802:
        // 消耗140星塔币，获得固定副音符技能90013，数量10。
        case 10803:
        // 消耗140星塔币，获得固定副音符技能90014，数量10。
        case 10804:
        // 消耗140星塔币，获得固定副音符技能90015，数量10。
        case 10805:
        // 消耗140星塔币，获得固定副音符技能90016，数量10。
        case 10806:
        // 消耗140星塔币，获得固定副音符技能90017，数量10。
        case 10807:
            if (GetGame()->GetResCount(GameConstants::TowerCoinItemId) >= 140)
            {
                GetGame()->AddRuntimeItem(GameConstants::TowerCoinItemId, -140, rsp.mutable_change());
                GetGame()->AddRuntimeItem((optionId % 100) + kTowerEventSubNoteSkillBaseId, kTowerEventSubNoteLargeReward, rsp.mutable_change());
            }
            else
            {
                completed = false;
            }
            break;
        // 消耗90星塔币，获得随机副音符技能，数量10。
        case 10808:
            if (GetGame()->GetResCount(GameConstants::TowerCoinItemId) >= 90)
            {
                GetGame()->AddRuntimeItem(GameConstants::TowerCoinItemId, -90, rsp.mutable_change());
                GetGame()->AddRuntimeItem(static_cast<uint32_t>((std::max)(GetGame()->GetRandomSubNoteId(), 0)), kTowerEventSubNoteLargeReward, rsp.mutable_change());
            }
            else
            {
                completed = false;
            }
            break;
        // 答题错误，设置错误答案提示参数。
        case 11401:
        // 答题错误，设置错误答案提示参数。
        case 11402:
        // 答题正确，获得随机副音符技能，数量10。
        case 11403:
        // 答题错误，设置错误答案提示参数。
        case 11404:
        // 答题错误，设置错误答案提示参数。
        case 11405:
            if (optionId == 11403)
            {
                GetGame()->AddRuntimeItem(static_cast<uint32_t>((std::max)(GetGame()->GetRandomSubNoteId(), 0)), kTowerEventSubNoteLargeReward, rsp.mutable_change());
            }
            else
            {
                result->set_optionsparamid(kTowerEventWrongAnswerOptionsParamId);
            }
            break;
        // 答题错误，设置错误答案提示参数。
        case 11501:
        // 答题错误，设置错误答案提示参数。
        case 11502:
        // 答题正确，生成普通潜能选择。
        case 11503:
        // 答题错误，设置错误答案提示参数。
        case 11504:
        // 答题错误，设置错误答案提示参数。
        case 11505:
            if (optionId == 11503)
            {
                auto selector = GetGame()->CreatePotentialSelector(0, false);
                if (selector)
                {
                    auto* added = GetRoom()->AddCase(std::move(selector));
                    if (added)
                    {
                        rsp.add_cases()->CopyFrom(added->ToProto());
                    }
                }
            }
            else
            {
                result->set_optionsparamid(kTowerEventWrongAnswerOptionsParamId);
            }
            break;
        // 答题错误，设置错误答案提示参数。
        case 11601:
        // 答题错误，设置错误答案提示参数。
        case 11602:
        // 答题正确，生成稀有潜能选择。
        case 11603:
        // 答题错误，设置错误答案提示参数。
        case 11604:
        // 答题错误，设置错误答案提示参数。
        case 11605:
            if (optionId == 11603)
            {
                auto selector = GetGame()->CreatePotentialSelector(0, true);
                if (selector)
                {
                    auto* added = GetRoom()->AddCase(std::move(selector));
                    if (added)
                    {
                        rsp.add_cases()->CopyFrom(added->ToProto());
                    }
                }
            }
            else
            {
                result->set_optionsparamid(kTowerEventWrongAnswerOptionsParamId);
            }
            break;
        // 为支援角色生成普通潜能选择。
        case 12601:
        // 为支援角色生成普通潜能选择。
        case 12701:
        {
            uint32_t supportCharId = 0;
            if (GetGame()->CharIds.size() <= 1)
            {
                supportCharId = GetGame()->CharIds.empty() ? 0u : GetGame()->CharIds.front();
            }
            else
            {
                const size_t index = static_cast<size_t>(RandomInt(1, static_cast<int>(GetGame()->CharIds.size() - 1)));
                supportCharId = GetGame()->CharIds[index];
            }

            auto selector = GetGame()->CreatePotentialSelector(supportCharId, false);
            if (selector)
            {
                auto* added = GetRoom()->AddCase(std::move(selector));
                if (added)
                {
                    rsp.add_cases()->CopyFrom(added->ToProto());
                }
            }
            break;
        }
        // 获得随机副音符技能，数量5。
        case 12702:
            GetGame()->AddRuntimeItem(static_cast<uint32_t>((std::max)(GetGame()->GetRandomSubNoteId(), 0)), kTowerEventSubNoteSmallReward, rsp.mutable_change());
            break;
        // 为支援角色生成稀有潜能选择。
        case 12801:
        {
            uint32_t supportCharId = 0;
            if (GetGame()->CharIds.size() <= 1)
            {
                supportCharId = GetGame()->CharIds.empty() ? 0u : GetGame()->CharIds.front();
            }
            else
            {
                const size_t index = static_cast<size_t>(RandomInt(1, static_cast<int>(GetGame()->CharIds.size() - 1)));
                supportCharId = GetGame()->CharIds[index];
            }

            auto selector = GetGame()->CreatePotentialSelector(supportCharId, true);
            if (selector)
            {
                auto* added = GetRoom()->AddCase(std::move(selector));
                if (added)
                {
                    rsp.add_cases()->CopyFrom(added->ToProto());
                }
            }
            break;
        }
        default:
            break;
        }

    }

    Completed = completed;
    result->set_optionsresult(completed);
    if (completed && GetGame()->GetManager())
    {
        auto* manager = GetGame()->GetManager();
        manager->RecordEventCollection(EventId);
        manager->GetPlayer()->Trigger(511, 1, 0, 0);
        if (NpcId > 0)
        {
            auto* affinityChange = result->add_affinitychange();
            const uint32_t affinity = manager->AddNpcAffinity(NpcId, kTowerNpcEventAffinityIncrease, affinityChange);
            manager->PushNpcAffinityNotify(NpcId, affinity, kTowerNpcEventAffinityIncrease);
        }
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
    for (uint32_t option : Options)
    {
        data->add_options(option);
    }
    auto* info = data->add_infos();
    info->set_npcid(NpcId);
    if (GetGame() && GetGame()->GetManager())
    {
        info->set_affinity(GetGame()->GetManager()->GetNpcAffinityValue(NpcId));
    }
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
    if (Goods.empty())
    {
        InitGoods();
    }

    if (GetGame())
    {
        RerollTimes = GetGame()->ShopRerollTimes;
        RerollPrice = GetGame()->ShopRerollPrice;
    }
}

void TowerHawkerCase::InitGoods()
{
    Goods.clear();

    const uint32_t total = GetGame() ? GetGame()->GetShopGoodsCount() : 2u;
    const uint32_t minPotentials = (std::max)(total / 2, 2u);
    const uint32_t maxPotentials = (std::max)(total - 1u, minPotentials);
    const uint32_t potentialCount = static_cast<uint32_t>(RandomInt(static_cast<int>(minPotentials), static_cast<int>(maxPotentials)));
    const uint32_t subNoteCount = total > potentialCount ? total - potentialCount : 0;
    const bool hasCoins = GetGame() && GetGame()->GetResCount(GameConstants::TowerCoinItemId) >= 500;

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
        if (hasCoins && RandomChance(0.25))
        {
            goods.Idx = 8;
            goods.Price = 400;
        }
        else
        {
            goods.Idx = 3;
            goods.Price = 90;
        }
        Goods.push_back(goods);
    }

    auto applyDiscount = [this](double chance, uint32_t times, double percentage) {
        if (!RandomChance(chance))
        {
            return;
        }

        std::vector<size_t> candidates;
        candidates.reserve(Goods.size());
        for (size_t i = 0; i < Goods.size(); ++i)
        {
            if (!Goods[i].HasDiscount())
            {
                candidates.push_back(i);
            }
        }

        for (uint32_t i = 0; i < times && !candidates.empty(); ++i)
        {
            const size_t candidateIndex = static_cast<size_t>(RandomInt(0, static_cast<int>(candidates.size() - 1)));
            Goods[candidates[candidateIndex]].ApplyDiscount(percentage);
            candidates.erase(candidates.begin() + static_cast<std::ptrdiff_t>(candidateIndex));
        }
    };

    if (GetGame() && GetGame()->GetManager())
    {
        const uint32_t difficulty = GetGame()->GetDifficulty();
        if (difficulty >= 3 && GetGame()->GetManager()->HasGrowthNode(20202))
        {
            applyDiscount(1.0, 2, 0.8);
        }
        if (difficulty >= 4 && GetGame()->GetManager()->HasGrowthNode(20502))
        {
            applyDiscount(0.3, 1, 0.5);
        }
        if (difficulty >= 5 && GetGame()->GetManager()->HasGrowthNode(20802))
        {
            applyDiscount(1.0, 1, 0.5);
        }
    }
}

proto::StarTowerInteractResp TowerHawkerCase::Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp)
{
    // 与 Nebula 一致：默认置 NilResp；reroll/购买成功时 oneof 会被 SelectResp 或后续字段覆盖。
    // 注意：购买潜能时只往 Cases 加 selector，oneof 仍可能是 NilResp（官方/Nebula 同）。
    rsp.mutable_nilresp();

    if (!req.has_hawkerreq() || !GetGame())
    {
        return rsp;
    }

    if (req.hawkerreq().has_sid())
    {
        const uint32_t sid = req.hawkerreq().sid();
        for (auto& goods : Goods)
        {
            if (goods.Sid != sid)
            {
                continue;
            }

            // 先验证商品可交付，避免无候选潜能或无效副音符仍扣费并售罄。
            if (goods.Sold || GetGame()->GetResCount(GameConstants::TowerCoinItemId) < goods.GetPrice())
            {
                break;
            }

            if (goods.Type == 1)
            {
                auto casePtr = GetGame()->CreatePotentialSelector(goods.GetCharId(*GetGame()), false);
                if (!casePtr)
                {
                    break;
                }

                auto* added = GetRoom()->AddCase(std::move(casePtr));
                if (!added)
                {
                    break;
                }

                goods.Sold = true;
                rsp.add_cases()->CopyFrom(added->ToProto());
            }
            else if (goods.Type == 2)
            {
                const auto itemIt = GameData::ItemDataTable.find(goods.GoodsId);
                if (itemIt == GameData::ItemDataTable.end() || itemIt->second.Stype != kTowerSubNoteSkillItemSubType || goods.GetCount() <= 0)
                {
                    break;
                }

                if (!GetGame()->AddRuntimeItem(goods.GoodsId, goods.GetCount(), rsp.mutable_change()))
                {
                    break;
                }

                goods.Sold = true;
            }
            else
            {
                break;
            }

            GetGame()->AddRuntimeItem(GameConstants::TowerCoinItemId, -goods.GetPrice(), rsp.mutable_change());
            if (GetGame()->GetManager() && GetGame()->GetManager()->GetPlayer())
            {
                GetGame()->GetManager()->GetPlayer()->Trigger(514, 1, 0, 0);
                if (goods.HasDiscount())
                {
                    GetGame()->GetManager()->GetPlayer()->Trigger(535, 1, 0, 0);
                }
            }
            break;
        }
    }
    else if (req.hawkerreq().has_reroll())
    {
        // 与 Nebula 一致：先检查次数与币，再刷货，再扣币与消耗 reroll。
        RerollTimes = GetGame()->ShopRerollTimes;
        RerollPrice = GetGame()->ShopRerollPrice;
        if (RerollTimes == 0 || GetGame()->GetResCount(GameConstants::TowerCoinItemId) < static_cast<int>(RerollPrice))
        {
            return rsp;
        }

        InitGoods();
        GetGame()->ConsumeShopReroll();
        RerollTimes = GetGame()->ShopRerollTimes;
        RerollPrice = GetGame()->ShopRerollPrice;
        GetGame()->AddRuntimeItem(GameConstants::TowerCoinItemId, -static_cast<int>(RerollPrice), rsp.mutable_change());
        if (GetGame()->GetManager() && GetGame()->GetManager()->GetPlayer())
        {
            GetGame()->GetManager()->GetPlayer()->Trigger(530, 1, 0, 0);
        }
        rsp.mutable_selectresp()->mutable_hawkercase()->CopyFrom(ToProto().hawkercase());
    }

    return rsp;
}

proto::StarTowerRoomCase TowerHawkerCase::ToProto() const
{
    proto::StarTowerRoomCase out;
    out.set_id(GetId());
    auto* data = out.mutable_hawkercase();
    if (RerollTimes > 0)
    {
        data->set_canreroll(true);
        data->set_rerolltimes(RerollTimes);
        data->set_rerollprice(RerollPrice);
    }
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
    data->set_rerolltimes(RerollTimes);
    data->set_rerollprice(RerollPrice);
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

void TowerStrengthenMachineCase::OnRegister()
{
    if (!GetGame() || !GetGame()->GetManager())
    {
        return;
    }

    Free = GetGame()->FreeStrengthenAvailable;
    Discount = static_cast<int32_t>(GetGame()->GetStrengthenDiscount());
}

proto::StarTowerInteractResp TowerStrengthenMachineCase::Interact(const proto::StarTowerInteractReq& req, proto::StarTowerInteractResp& rsp)
{
    const int price = GetPrice();
    if (GetGame()->GetResCount(GameConstants::TowerCoinItemId) < price)
    {
        rsp.mutable_strengthenmachineresp()->set_buysucceed(false);
        return rsp;
    }

    auto casePtr = GetGame()->CreateStrengthenSelector();
    if (!casePtr)
    {
        rsp.mutable_strengthenmachineresp()->set_buysucceed(false);
        return rsp;
    }

    GetGame()->AddRuntimeItem(GameConstants::TowerCoinItemId, -price, rsp.mutable_change());
    if (Free)
    {
        Free = false;
        GetGame()->ConsumeFreeStrengthen();
    }
    else
    {
        ++Times;
    }

    auto* added = GetRoom()->AddCase(std::move(casePtr));
    if (added)
    {
        rsp.add_cases()->CopyFrom(added->ToProto());
    }
    if (GetGame()->GetManager() && GetGame()->GetManager()->GetPlayer())
    {
        GetGame()->GetManager()->GetPlayer()->Trigger(522, 1, 0, 0);
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
    if (GetRoom())
    {
        auto* added = GetRoom()->AddCase(std::make_unique<TowerSyncHPCase>());
        if (added)
        {
            rsp.add_cases()->CopyFrom(added->ToProto());
        }
    }
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
