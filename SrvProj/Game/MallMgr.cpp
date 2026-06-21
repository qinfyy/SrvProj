#include "MallMgr.h"

#include "ChangeInfoUtil.h"
#include "InventoryMgr.h"
#include "Player.h"
#include "../GameConstants.h"
#include "../GameTime.h"
#include "../GameSession.h"
#include "../Resources/BinClass/ShopsRes.h"
#include "../Resources/GameData.h"
#include "../proto/NetMsgId.pb.h"
#include "../proto/proto_cpp/notify.pb.h"

#include <algorithm>
#include <cstdint>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace {
uint32_t PackageStock(const Player& player, const MallPackageRes& data)
{
    if (data.Stock <= 0)
    {
        return GameConstants::UnlimitedStock;
    }

    const uint32_t bought = player.Inventory().GetMallPackagePurchaseCount(data.Id);
    return bought >= static_cast<uint32_t>(data.Stock) ? 0 : static_cast<uint32_t>(data.Stock) - bought;
}

int32_t ShopStock(const Player& player, const MallShopRes& data)
{
    if (data.Stock <= 0)
    {
        return GameConstants::UnlimitedStock;
    }

    const uint32_t bought = player.Inventory().GetMallShopPurchaseCount(data.Id);
    return bought >= static_cast<uint32_t>(data.Stock) ? 0 : static_cast<int32_t>(static_cast<uint32_t>(data.Stock) - bought);
}
}

MallMgr& MallMgr::Instance()
{
    static MallMgr instance;
    return instance;
}

bool MallMgr::MatchCondition(const Player& player, int condType, const std::vector<int>& params) const
{
    if (condType == 0)
    {
        return true;
    }

    if (condType == 71)
    {
        const int requiredLevel = params.empty() ? 0 : params.front();
        return player.GetPlayerData().level() >= requiredLevel;
    }

    return false;
}

bool MallMgr::IsPackageVisible(const Player& player, const MallPackageRes& data) const
{
    const int64_t now = GameTime::ServerNowSeconds();
    if (data.ListTimeSeconds > 0 && now < data.ListTimeSeconds)
    {
        return false;
    }
    if (data.DeListTimeSeconds > 0 && now >= data.DeListTimeSeconds)
    {
        return false;
    }

    return MatchCondition(player, data.ListCondType, data.ListCond);
}

bool MallMgr::CanPurchasePackage(const Player& player, const MallPackageRes& data) const
{
    auto ipv = IsPackageVisible(player, data);
    auto ps = PackageStock(player, data);
    if (!IsPackageVisible(player, data) || PackageStock(player, data) == 0)
    {
        return false;
    }

    return MatchCondition(player, data.OrderCondType, data.OrderCond);
}

bool MallMgr::IsShopVisible(const MallShopRes& data) const
{
    const int64_t now = GameTime::ServerNowSeconds();
    return (data.ListTimeSeconds <= 0 || now >= data.ListTimeSeconds) &&
        (data.DeListTimeSeconds <= 0 || now < data.DeListTimeSeconds);
}

bool MallMgr::CanPurchaseShopItem(const Player& player, const MallShopRes& data, uint32_t count) const
{
    return count > 0 && IsShopVisible(data) && ShopStock(player, data) >= static_cast<int32_t>(count) &&
        MatchCondition(player, data.OrderCondType, data.OrderCond);
}

bool MallMgr::CanPurchaseMonthlyCard(const Player& player, const MallMonthlyCardRes& data) const
{
    return player.GetMonthlyCardRemainingDays(data.Id) <= static_cast<uint32_t>((std::max)(data.MaxDays, 0));
}

int64_t MallMgr::GetNextRefreshTime(int refreshType) const
{
    switch (refreshType)
    {
    case GameConstants::RefreshTypeDaily:
        return GameTime::NextDailyReset();
    case GameConstants::RefreshTypeWeekly:
        return GameTime::NextWeeklyReset();
    case GameConstants::RefreshTypeMonthly:
        return GameTime::NextMonthlyReset();
    default:
        return 0;
    }
}

proto::MallGemList MallMgr::BuildGemList(Player& player) const
{
    proto::MallGemList out;
    for (const auto& [_, data] : GameData::MallGemDataTable)
    {
        auto* info = out.add_list();
        info->set_id(data.Id);
        info->set_maiden(player.Inventory().HasMallGemMaidenBonus(data.Id));
    }

    return out;
}

proto::MallMonthlyCardList MallMgr::BuildMonthlyCardList(Player& player) const
{
    proto::MallMonthlyCardList out;
    for (const auto& [_, data] : GameData::MallMonthlyCardDataTable)
    {
        auto* info = out.add_list();
        info->set_id(data.Id);
        info->set_remaining(player.GetMonthlyCardRemainingDays(data.Id));
        info->set_received(player.ReceivedMonthlyCardRewardToday(data.Id));
    }

    return out;
}

proto::MallPackageList MallMgr::BuildPackageList(Player& player) const
{
    proto::MallPackageList out;
    player.QueueMallPackageStateNotify();
    for (const auto& [_, data] : GameData::MallPackageDataTable)
    {
        if (!IsPackageVisible(player, data))
        {
            continue;
        }

        auto* info = out.add_list();
        info->set_id(data.Id);
        info->set_stock(PackageStock(player, data));
        info->set_refreshtime(GetNextRefreshTime(data.RefreshType));
    }

    return out;
}

proto::MallShopProductList MallMgr::BuildShopList(Player& player) const
{
    proto::MallShopProductList out;
    for (const auto& [_, data] : GameData::MallShopDataTable)
    {
        if (!IsShopVisible(data) || !MatchCondition(player, data.ListCondType, data.ListCond))
        {
            continue;
        }

        auto* info = out.add_list();
        info->set_id(data.Id);
        info->set_stock(ShopStock(player, data));
        info->set_refreshtime(GetNextRefreshTime(data.RefreshType));
    }

    return out;
}

proto::OrderInfo MallMgr::CreateOrder(GameSession* session, OrderType type, const std::string& id, const std::string& typeName)
{
    proto::OrderInfo out;
    if (!session || !session->HasPlayer())
    {
        return out;
    }

    const uint32_t uid = session->GetPlayer()->GetUid();
    const int64_t now = GameTime::NowSeconds();
    const std::string orderId = typeName + "." + std::to_string(uid) + "." + std::to_string(now);

    {
        std::lock_guard<std::mutex> lock(mMutex);
        mPendingCollects[uid] = { type, id, orderId };
    }

    proto::OrderStateChange notify;
    notify.set_orderid(orderId);
    notify.set_store(1);

    out.set_id(orderId);
    out.set_extradata(typeName + ":" + id + ":" + orderId);
    out.set_notifyurl("http://localhost:21000/mock-pay"); // DEBUG
    out.set_nextpackage(GameSession::EncodeMessage(order_paid_notify, notify.SerializeAsString()));
    return out;
}

bool MallMgr::CreateGemOrder(GameSession* session, const MallGemRes& data, proto::OrderInfo& out)
{
    out = CreateOrder(session, OrderType::Gem, data.Id, "gem");
    return !out.id().empty();
}

bool MallMgr::CreateMonthlyCardOrder(GameSession* session, const MallMonthlyCardRes& data, proto::OrderInfo& out)
{
    if (!session || !session->HasPlayer() || !CanPurchaseMonthlyCard(*session->GetPlayer(), data))
    {
        return false;
    }

    out = CreateOrder(session, OrderType::MonthlyCard, data.Id, "monthly");
    return !out.id().empty();
}

bool MallMgr::CreatePackageOrder(GameSession* session, const MallPackageRes& data, proto::OrderInfo& out)
{
    if (!session || !session->HasPlayer() || data.CurrencyType != GameConstants::CurrencyTypeCash || !CanPurchasePackage(*session->GetPlayer(), data))
    {
        return false;
    }

    out = CreateOrder(session, OrderType::Package, data.Id, "package");
    return !out.id().empty();
}

void MallMgr::ClearPendingCollect(uint32_t uid)
{
    std::lock_guard<std::mutex> lock(mMutex);
    mPendingCollects.erase(uid);
}

bool MallMgr::ConsumePendingCollect(GameSession* session, proto::ChangeInfo& out)
{
    if (!session || !session->HasPlayer())
    {
        return false;
    }

    PendingOrder order;
    {
        std::lock_guard<std::mutex> lock(mMutex);
        auto it = mPendingCollects.find(session->GetPlayer()->GetUid());
        if (it == mPendingCollects.end())
        {
            return true;
        }

        order = it->second;
        mPendingCollects.erase(it);
    }

    auto* player = session->GetPlayer();
    switch (order.Type)
    {
    case OrderType::Gem:
    {
        const auto it = GameData::MallGemDataTable.find(order.Id);
        return it != GameData::MallGemDataTable.end() && player->Inventory().BuyMallGem(it->second, out);
    }
    case OrderType::MonthlyCard:
    {
        const auto it = GameData::MallMonthlyCardDataTable.find(order.Id);
        if (it == GameData::MallMonthlyCardDataTable.end() || !CanPurchaseMonthlyCard(*player, it->second))
        {
            return false;
        }

        if (!player->Inventory().BuyMallMonthlyCard(it->second, out))
        {
            return false;
        }

        player->ActivateMonthlyCard(order.Id, GameConstants::MonthlyCardDurationDays);
        proto::ChangeInfo dailyChange;
        if (player->CreateMonthlyCardRewardChange(order.Id, dailyChange))
        {
            out.MergeFrom(dailyChange);

            proto::MonthlyCardRewards notify;
            notify.set_id(static_cast<uint32_t>(it->second.MonthlyCardId));
            notify.set_remaining(player->GetMonthlyCardRemainingDays(order.Id));
            notify.set_switch_(true);
            notify.set_endtime(player->GetMonthlyCardEndTime(order.Id));
            notify.mutable_change()->CopyFrom(dailyChange);

            const auto rewardIt = GameData::MonthlyCardDataTable.find(std::to_string(it->second.MonthlyCardId));
            if (rewardIt != GameData::MonthlyCardDataTable.end())
            {
                for (const auto& [tid, qty] : rewardIt->second.Rewards.Items)
                {
                    ChangeInfoUtil::AddItemTpl(notify.add_rewards(), static_cast<uint32_t>(tid), qty);
                }
            }

            player->PushNextPackage(monthly_card_rewards_notify, notify);
        }
        return true;
    }
    case OrderType::Package:
    {
        const auto it = GameData::MallPackageDataTable.find(order.Id);
        return it != GameData::MallPackageDataTable.end() && CanPurchasePackage(*player, it->second) && player->Inventory().BuyMallPackage(it->second, out);
    }
    default:
        return false;
    }
}
