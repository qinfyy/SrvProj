#include "Item.h"

#include "../Game/InventoryMgr.h"
#include "../Game/Player.h"
#include "../GameSession.h"
#include "../proto/NetMsgId.pb.h"
#include "../proto/proto_cpp/item_product.pb.h"
#include "../proto/proto_cpp/item_quick_growth.pb.h"
#include "../proto/proto_cpp/item_use.pb.h"
#include "../proto/proto_cpp/public.pb.h"

namespace {
bool HasPlayer(GameSession* session)
{
    return session && session->HasPlayer();
}

void FinishChange(GameSession* session, const proto::ChangeInfo& change)
{
    if (!session || !session->HasPlayer())
    {
        return;
    }

    session->SavePlayer();
}
}

std::string item_use_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, item_use_failed_ack);
    }

    proto::ItemUseReq request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, item_use_failed_ack);
    }

    proto::ChangeInfo change;
    bool changed = false;
    if (request.has_use())
    {
        for (const auto& item : request.use().list())
        {
            changed = session->GetPlayer()->Inventory().UseItem(item.tid(), item.qty(), 0, change) || changed;
        }
    }
    if (request.has_pick())
    {
        for (const auto& item : request.pick().list())
        {
            changed = session->GetPlayer()->Inventory().UseItem(item.tid(), item.qty(), item.selecttid(), change) || changed;
        }
    }

    if (!changed)
    {
        return EncodeReply(session, item_use_failed_ack);
    }

    FinishChange(session, change);
    return EncodeReply(session, item_use_succeed_ack, &change);
}

std::string item_product_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, item_product_failed_ack);
    }

    proto::ItemProductReq request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, item_product_failed_ack);
    }

    proto::ChangeInfo change;
    if (!session->GetPlayer()->Inventory().Produce(request.id(), request.num(), change))
    {
        return EncodeReply(session, item_product_failed_ack);
    }

    FinishChange(session, change);
    return EncodeReply(session, item_product_succeed_ack, &change);
}

std::string item_quick_growth_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, item_quick_growth_failed_ack);
    }

    proto::ItemGrowthReq request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, item_quick_growth_failed_ack);
    }

    proto::ChangeInfo change;
    bool changed = false;
    for (const auto& step : request.list())
    {
        if (step.has_product())
        {
            changed = session->GetPlayer()->Inventory().Produce(step.product().id(), step.product().num(), change) || changed;
        }
        if (step.has_pick())
        {
            for (const auto& item : step.pick().list())
            {
                changed = session->GetPlayer()->Inventory().UseItem(item.tid(), item.qty(), item.selecttid(), change) || changed;
            }
        }
    }

    if (!changed)
    {
        return EncodeReply(session, item_quick_growth_failed_ack);
    }

    FinishChange(session, change);
    return EncodeReply(session, item_quick_growth_succeed_ack, &change);
}
