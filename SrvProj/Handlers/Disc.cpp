#include "Disc.h"

#include "../Game/CharacterMgr.h"
#include "../Game/InventoryMgr.h"
#include "../Game/Player.h"
#include "../GameSession.h"
#include "../proto/NetMsgId.pb.h"
#include "../proto/proto_cpp/disc_all_limit_break.pb.h"
#include "../proto/proto_cpp/disc_limit_break.pb.h"
#include "../proto/proto_cpp/disc_promote.pb.h"
#include "../proto/proto_cpp/disc_read_reward_receive.pb.h"
#include "../proto/proto_cpp/disc_strengthen.pb.h"
#include "../proto/proto_cpp/public.pb.h"

namespace {
bool HasPlayer(GameSession* session)
{
    return session && session->HasPlayer();
}

void SaveAndPush(GameSession* session, const proto::ChangeInfo& change)
{
    if (!HasPlayer(session))
    {
        return;
    }

    session->GetPlayer()->Inventory().PushItemsChange(change);
    session->SavePlayer();
}

ItemParamMap FromItemInfos(const google::protobuf::RepeatedPtrField<proto::ItemInfo>& items)
{
    ItemParamMap out;
    for (const auto& item : items)
    {
        out.Add(static_cast<int>(item.tid()), static_cast<int>(item.qty()));
    }
    return out;
}
}

std::string disc_strengthen_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, disc_strengthen_failed_ack);
    }

    proto::DiscStrengthenReq request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, disc_strengthen_failed_ack);
    }

    proto::DiscStrengthenResp response;
    if (!session->GetPlayer()->Characters().StrengthenDisc(request.id(), FromItemInfos(request.items()), response))
    {
        return EncodeReply(session, disc_strengthen_failed_ack);
    }

    SaveAndPush(session, response.change());
    return EncodeReply(session, disc_strengthen_succeed_ack, &response);
}

std::string disc_promote_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, disc_promote_failed_ack);
    }

    proto::DiscPromoteReq request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, disc_promote_failed_ack);
    }

    proto::DiscPromoteResp response;
    if (!session->GetPlayer()->Characters().PromoteDisc(request.id(), response))
    {
        return EncodeReply(session, disc_promote_failed_ack);
    }

    SaveAndPush(session, response.change());
    return EncodeReply(session, disc_promote_succeed_ack, &response);
}

std::string disc_limit_break_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, disc_limit_break_failed_ack);
    }

    proto::DiscLimitBreakReq request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, disc_limit_break_failed_ack);
    }

    proto::DiscLimitBreakResp response;
    if (!session->GetPlayer()->Characters().LimitBreakDisc(request.id(), request.qty(), response))
    {
        return EncodeReply(session, disc_limit_break_failed_ack);
    }

    SaveAndPush(session, response.change());
    return EncodeReply(session, disc_limit_break_succeed_ack, &response);
}

std::string disc_all_limit_break_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, disc_all_limit_break_failed_ack);
    }

    proto::DiscAllLimitBreakResp response;
    if (!session->GetPlayer()->Characters().LimitBreakAllDiscs(response))
    {
        return EncodeReply(session, disc_all_limit_break_failed_ack);
    }

    SaveAndPush(session, response.change());
    return EncodeReply(session, disc_all_limit_break_succeed_ack, &response);
}

std::string disc_read_reward_receive_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, disc_read_reward_receive_failed_ack);
    }

    proto::DiscReadRewardReceiveReq request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, disc_read_reward_receive_failed_ack);
    }

    proto::ChangeInfo change;
    if (!session->GetPlayer()->Characters().ReceiveDiscReadReward(request.id(), static_cast<uint32_t>(request.readtype()), change))
    {
        return EncodeReply(session, disc_read_reward_receive_failed_ack);
    }

    SaveAndPush(session, change);
    return EncodeReply(session, disc_read_reward_receive_succeed_ack, &change);
}
