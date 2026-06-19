#include "Mall.h"

#include "../Game/InventoryMgr.h"
#include "../Game/MallMgr.h"
#include "../Game/Player.h"
#include "../GameConstants.h"
#include "../GameSession.h"
#include "../Resources/BinClass/ShopsRes.h"
#include "../Resources/GameData.h"
#include "../proto/NetMsgId.pb.h"
#include "../proto/proto_cpp/mall_gem_list.pb.h"
#include "../proto/proto_cpp/mall_gem_order.pb.h"
#include "../proto/proto_cpp/mall_monthlycard_list.pb.h"
#include "../proto/proto_cpp/mall_package_list.pb.h"
#include "../proto/proto_cpp/mall_package_order.pb.h"
#include "../proto/proto_cpp/mall_shop_list.pb.h"
#include "../proto/proto_cpp/mall_shop_order.pb.h"
#include "../proto/proto_cpp/public.pb.h"

namespace {
bool HasPlayer(GameSession* session)
{
    return session && session->HasPlayer();
}
}

std::string mall_gem_list_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, mall_gem_list_failed_ack);
    }

    auto response = MallMgr::Instance().BuildGemList(*session->GetPlayer());
    return EncodeReply(session, mall_gem_list_succeed_ack, &response);
}

std::string mall_gem_order_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, mall_gem_order_failed_ack);
    }

    proto::OrderInfo request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, mall_gem_order_failed_ack);
    }

    const auto it = GameData::MallGemDataTable.find(request.id());
    if (it == GameData::MallGemDataTable.end())
    {
        return EncodeReply(session, mall_gem_order_failed_ack);
    }

    proto::OrderInfo response;
    if (!MallMgr::Instance().CreateGemOrder(session, it->second, response))
    {
        return EncodeReply(session, mall_gem_order_failed_ack);
    }

    return EncodeReply(session, mall_gem_order_succeed_ack, &response);
}

std::string mall_order_cancel_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, mall_order_cancel_failed_ack);
    }

    MallMgr::Instance().ClearPendingCollect(session->GetPlayer()->GetUid());
    return EncodeReply(session, mall_order_cancel_succeed_ack);
}

std::string mall_order_collect_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, mall_order_collect_failed_ack);
    }

    proto::ChangeInfo change;
    if (!MallMgr::Instance().ConsumePendingCollect(session, change))
    {
        return EncodeReply(session, mall_order_collect_failed_ack);
    }

    proto::CollectResp response;
    response.set_status(proto::CollectResp::Done);
    if (change.props_size() > 0)
    {
        response.mutable_items()->CopyFrom(change);
        session->GetPlayer()->Inventory().PushItemsChange(change);
    }

    session->SavePlayer();
    return EncodeReply(session, mall_order_collect_succeed_ack, &response);
}

std::string mall_monthlyCard_list_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, mall_monthlyCard_list_failed_ack);
    }

    auto response = MallMgr::Instance().BuildMonthlyCardList(*session->GetPlayer());
    return EncodeReply(session, mall_monthlyCard_list_succeed_ack, &response);
}

std::string mall_monthlyCard_order_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, mall_monthlyCard_order_failed_ack);
    }

    proto::MonthlyCardInfo request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, mall_monthlyCard_order_failed_ack);
    }

    const auto it = GameData::MallMonthlyCardDataTable.find(request.id());
    if (it == GameData::MallMonthlyCardDataTable.end())
    {
        return EncodeReply(session, mall_monthlyCard_order_failed_ack);
    }

    proto::OrderInfo response;
    if (!MallMgr::Instance().CreateMonthlyCardOrder(session, it->second, response))
    {
        return EncodeReply(session, mall_monthlyCard_order_failed_ack);
    }

    return EncodeReply(session, mall_monthlyCard_order_succeed_ack, &response);
}

std::string mall_package_list_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, mall_package_list_failed_ack);
    }

    auto response = MallMgr::Instance().BuildPackageList(*session->GetPlayer());
    return EncodeReply(session, mall_package_list_succeed_ack, &response);
}

std::string mall_package_order_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, mall_package_order_failed_ack);
    }

    proto::OrderInfo request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, mall_package_order_failed_ack);
    }

    const auto it = GameData::MallPackageDataTable.find(request.id());
    if (it == GameData::MallPackageDataTable.end() || !MallMgr::Instance().CanPurchasePackage(*session->GetPlayer(), it->second))
    {
        return EncodeReply(session, mall_package_order_failed_ack);
    }

    proto::MallPackageOrder response;
    if (it->second.CurrencyType == GameConstants::CurrencyTypeCash)
    {
        proto::OrderInfo order;
        if (!MallMgr::Instance().CreatePackageOrder(session, it->second, order))
        {
            return EncodeReply(session, mall_package_order_failed_ack);
        }

        response.mutable_order()->CopyFrom(order);
        if (!order.nextpackage().empty())
        {
            response.set_nextpackage(order.nextpackage());
        }
        return EncodeReply(session, mall_package_order_succeed_ack, &response);
    }

    proto::ChangeInfo change;
    if (!session->GetPlayer()->Inventory().BuyMallPackage(it->second, change))
    {
        return EncodeReply(session, mall_package_order_failed_ack);
    }

    response.mutable_change()->CopyFrom(change);
    session->GetPlayer()->Inventory().PushItemsChange(change);
    if (it->second.CurrencyType == GameConstants::CurrencyTypeFree)
    {
        session->GetPlayer()->QueueMallPackageStateNotify();
    }
    session->SavePlayer();
    return EncodeReply(session, mall_package_order_succeed_ack, &response);
}

std::string mall_shop_list_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, mall_shop_list_failed_ack);
    }

    auto response = MallMgr::Instance().BuildShopList(*session->GetPlayer());
    return EncodeReply(session, mall_shop_list_succeed_ack, &response);
}

std::string mall_shop_order_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, mall_shop_order_failed_ack);
    }

    proto::MallShopOrderReq request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, mall_shop_order_failed_ack);
    }

    const auto it = GameData::MallShopDataTable.find(request.id());
    if (it == GameData::MallShopDataTable.end() || !MallMgr::Instance().CanPurchaseShopItem(*session->GetPlayer(), it->second, request.qty()))
    {
        return EncodeReply(session, mall_shop_order_failed_ack);
    }

    proto::ChangeInfo change;
    if (!session->GetPlayer()->Inventory().BuyMallShopItem(it->second, request.qty(), change))
    {
        return EncodeReply(session, mall_shop_order_failed_ack);
    }

    session->GetPlayer()->Inventory().PushItemsChange(change);
    session->SavePlayer();
    return EncodeReply(session, mall_shop_order_succeed_ack, &change);
}
