#include "BattlePass.h"

#include "../Game/BattlePassMgr.h"
#include "../Game/Player.h"
#include "../GameSession.h"
#include "../proto/NetMsgId.h"
#include "../proto/proto_cpp/battle_pass_level_buy.pb.h"
#include "../proto/proto_cpp/battle_pass_order.pb.h"
#include "../proto/proto_cpp/battle_pass_order_collect.pb.h"
#include "../proto/proto_cpp/battle_pass_quest_reward_receive.pb.h"
#include "../proto/proto_cpp/battle_pass_reward_receive.pb.h"
#include "../proto/proto_cpp/public.pb.h"

namespace {
bool HasLoggedInPlayer(GameSession* session)
{
    return session && session->HasPlayer() && session->GetPlayer();
}

bool ParseOptionalUI32(const std::string& req, uint32_t& out)
{
    out = 0;
    if (req.empty())
    {
        return true;
    }

    proto::UI32 reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return false;
    }

    out = reqPb.value();
    return true;
}
}

std::string battle_pass_info_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, battle_pass_info_failed_ack);
    }

    auto rsp = session->GetPlayer()->BattlePasses().ToProto();
    return EncodeReply(session, battle_pass_info_succeed_ack, &rsp);
}

std::string battle_pass_quest_reward_receive_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, battle_pass_quest_reward_receive_failed_ack);
    }

    uint32_t questId = 0;
    if (!ParseOptionalUI32(req, questId))
    {
        return EncodeReply(session, battle_pass_quest_reward_receive_failed_ack);
    }

    uint32_t level = 0;
    uint32_t exp = 0;
    uint32_t expThisWeek = 0;
    if (!session->GetPlayer()->BattlePasses().ClaimQuestReward(questId, level, exp, expThisWeek))
    {
        return EncodeReply(session, battle_pass_quest_reward_receive_failed_ack);
    }

    proto::BattlePassQuestRewardResp rsp;
    rsp.set_level(level);
    rsp.set_exp(exp);
    rsp.set_expthisweek(expThisWeek);

    session->SavePlayer();
    return EncodeReply(session, battle_pass_quest_reward_receive_succeed_ack, &rsp);
}

std::string battle_pass_reward_receive_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, battle_pass_reward_receive_failed_ack);
    }

    proto::BattlePassRewardReceiveReq reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, battle_pass_reward_receive_failed_ack);
    }

    proto::BattlePassRewardReceiveResp rsp;
    if (!session->GetPlayer()->BattlePasses().ClaimReward(reqPb, rsp))
    {
        return EncodeReply(session, battle_pass_reward_receive_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, battle_pass_reward_receive_succeed_ack, &rsp);
}

std::string battle_pass_level_buy_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, battle_pass_level_buy_failed_ack);
    }

    uint32_t levels = 1;
    if (!ParseOptionalUI32(req, levels))
    {
        return EncodeReply(session, battle_pass_level_buy_failed_ack);
    }

    proto::BattlePassLevelBuyResp rsp;
    if (!session->GetPlayer()->BattlePasses().BuyLevels(levels, rsp))
    {
        return EncodeReply(session, battle_pass_level_buy_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, battle_pass_level_buy_succeed_ack, &rsp);
}

std::string battle_pass_order_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, battle_pass_order_failed_ack);
    }

    proto::BattlePassOrderReq reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, battle_pass_order_failed_ack);
    }

    proto::OrderInfo rsp;
    if (!session->GetPlayer()->BattlePasses().CreateOrder(reqPb.mode(), rsp))
    {
        return EncodeReply(session, battle_pass_order_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, battle_pass_order_succeed_ack, &rsp);
}

std::string battle_pass_order_collect_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, battle_pass_order_collect_failed_ack);
    }

    proto::BattlePassOrderCollectResp rsp;
    if (!session->GetPlayer()->BattlePasses().CollectOrder(rsp))
    {
        return EncodeReply(session, battle_pass_order_collect_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, battle_pass_order_collect_succeed_ack, &rsp);
}
