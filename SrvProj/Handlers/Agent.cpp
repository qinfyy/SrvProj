#include "Agent.h"

#include "../Game/AgentMgr.h"
#include "../Game/Player.h"
#include "../GameSession.h"
#include "../proto/NetMsgId.h"
#include "../proto/proto_cpp/agent_apply.pb.h"
#include "../proto/proto_cpp/agent_give_up.pb.h"
#include "../proto/proto_cpp/agent_reward_receive.pb.h"

std::string agent_apply_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, agent_apply_failed_ack);
    }

    proto::AgentApplyReq reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, agent_apply_failed_ack);
    }

    proto::AgentApplyResp resp;
    for (const auto& apply : reqPb.apply())
    {
        const auto* agent = session->GetPlayer()->Agents().Apply(apply);
        if (!agent)
        {
            return EncodeReply(session, agent_apply_failed_ack);
        }
        auto* info = resp.add_infos();
        info->set_id(agent->id());
        info->set_begintime(agent->starttime());
    }

    if (resp.infos_size() > 0)
    {
        session->SavePlayer();
    }
    return EncodeReply(session, agent_apply_succeed_ack, &resp);
}

std::string agent_give_up_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, agent_give_up_failed_ack);
    }

    proto::AgentGiveUpReq reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, agent_give_up_failed_ack);
    }

    proto::AgentGiveUpResp resp;
    if (!session->GetPlayer()->Agents().GiveUp(reqPb.id(), resp))
    {
        return EncodeReply(session, agent_give_up_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, agent_give_up_succeed_ack, &resp);
}

std::string agent_reward_receive_req__Handler(GameSession* session, const std::string& req)
{
    if (!IsLoggedIn(session))
    {
        return EncodeReply(session, agent_reward_receive_failed_ack);
    }

    proto::AgentRewardReceiveReq reqPb;
    if (!reqPb.ParseFromString(req))
    {
        return EncodeReply(session, agent_reward_receive_failed_ack);
    }

    proto::AgentRewardReceiveResp resp;
    if (!session->GetPlayer()->Agents().ReceiveReward(reqPb.id(), resp))
    {
        return EncodeReply(session, agent_reward_receive_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, agent_reward_receive_succeed_ack, &resp);
}
