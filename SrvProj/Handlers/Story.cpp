#include "Story.h"

#include "../Game/AchievementMgr.h"
#include "../Game/CharacterMgr.h"
#include "../Game/Player.h"
#include "../Game/StoryMgr.h"
#include "../GameSession.h"
#include "../proto/NetMsgId.h"
#include "../proto/proto_cpp/public.pb.h"
#include "../proto/proto_cpp/story_apply.pb.h"
#include "../proto/proto_cpp/story_set_info.pb.h"
#include "../proto/proto_cpp/story_set_reward_receive.pb.h"
#include "../proto/proto_cpp/story_sett.pb.h"

namespace {
bool HasPlayer(GameSession* session)
{
    return session && session->HasPlayer() && session->GetPlayer();
}
}

std::string story_apply_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, story_apply_failed_ack);
    }

    proto::StoryApplyReq request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, story_apply_failed_ack);
    }

    session->GetPlayer()->Stories().Apply(request.idx());
    return EncodeReply(session, story_apply_succeed_ack);
}

std::string story_settle_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, story_settle_failed_ack);
    }

    proto::StorySettleReq request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, story_settle_failed_ack);
    }

    proto::ChangeInfo change;
    session->GetPlayer()->Stories().Settle(request, change);
    session->GetPlayer()->Achievements().HandleClientEvents(request.events());
    session->SavePlayer();
    return EncodeReply(session, story_settle_succeed_ack, &change);
}

std::string story_set_info_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, story_set_info_failed_ack);
    }

    proto::StorySetInfoResp response;
    session->GetPlayer()->Stories().BuildStorySetInfo(response);
    return EncodeReply(session, story_set_info_succeed_ack, &response);
}

std::string story_set_reward_receive_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, story_set_reward_receive_failed_ack);
    }

    proto::StorySetRewardReceiveReq request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, story_set_reward_receive_failed_ack);
    }

    proto::ChangeInfo change;
    session->GetPlayer()->Stories().SettleSet(request.chapterid(), request.sectionid(), change);
    session->SavePlayer();
    return EncodeReply(session, story_set_reward_receive_succeed_ack, &change);
}

std::string plot_reward_receive_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasPlayer(session))
    {
        return EncodeReply(session, plot_reward_receive_failed_ack);
    }

    proto::UI32 request;
    if (!request.ParseFromString(req))
    {
        return EncodeReply(session, plot_reward_receive_failed_ack);
    }

    proto::ChangeInfo change;
    if (session->GetPlayer()->Characters().ReceivePlotReward(request.value(), change))
    {
        session->SavePlayer();
    }
    return EncodeReply(session, plot_reward_receive_succeed_ack, &change);
}
