#pragma once

#include "../proto/proto_cpp/mall_gem_list.pb.h"
#include "../proto/proto_cpp/mall_gem_order.pb.h"
#include "../proto/proto_cpp/mall_monthlycard_list.pb.h"
#include "../proto/proto_cpp/mall_package_list.pb.h"
#include "../proto/proto_cpp/mall_shop_list.pb.h"
#include "../proto/proto_cpp/public.pb.h"

#include <cstdint>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

class GameSession;
class MallGemRes;
class MallMonthlyCardRes;
class MallPackageRes;
class MallShopRes;
class Player;

class MallMgr
{
public:
    static MallMgr& Instance();

    proto::MallGemList BuildGemList(Player& player) const;
    proto::MallMonthlyCardList BuildMonthlyCardList(Player& player) const;
    proto::MallPackageList BuildPackageList(Player& player) const;
    proto::MallShopProductList BuildShopList(Player& player) const;

    bool CreateGemOrder(GameSession* session, const MallGemRes& data, proto::OrderInfo& out);
    bool CreateMonthlyCardOrder(GameSession* session, const MallMonthlyCardRes& data, proto::OrderInfo& out);
    bool CreatePackageOrder(GameSession* session, const MallPackageRes& data, proto::OrderInfo& out);
    void ClearPendingCollect(uint32_t uid);
    bool ConsumePendingCollect(GameSession* session, proto::ChangeInfo& out);

    bool IsPackageVisible(const Player& player, const MallPackageRes& data) const;
    bool CanPurchasePackage(const Player& player, const MallPackageRes& data) const;
    bool IsShopVisible(const MallShopRes& data) const;
    bool CanPurchaseShopItem(const Player& player, const MallShopRes& data, uint32_t count) const;
    bool CanPurchaseMonthlyCard(const Player& player, const MallMonthlyCardRes& data) const;
    int64_t GetNextRefreshTime(int refreshType) const;

private:
    enum class OrderType
    {
        Gem,
        MonthlyCard,
        Package,
    };

    struct PendingOrder
    {
        OrderType Type = OrderType::Gem;
        std::string Id;
        std::string OrderId;
    };

    proto::OrderInfo CreateOrder(GameSession* session, OrderType type, const std::string& id, const std::string& typeName);
    bool MatchCondition(const Player& player, int condType, const std::vector<int>& params) const;

    mutable std::mutex mMutex;
    std::unordered_map<uint32_t, PendingOrder> mPendingCollects;
};
