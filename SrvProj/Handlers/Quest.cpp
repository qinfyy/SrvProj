#include "Quest.h"

#include "../Game/AchievementMgr.h"
#include "../Game/Player.h"
#include "../Game/QuestMgr.h"
#include "../GameSession.h"
#include "../proto/NetMsgId.pb.h"
#include "../proto/proto_cpp/achievement_reward_receive.pb.h"
#include "../proto/proto_cpp/battle_pass_quest_reward_receive.pb.h"
#include "../proto/proto_cpp/public.pb.h"
#include "../proto/proto_cpp/quest_daily_active_reward_recevie.pb.h"
#include "../proto/proto_cpp/quest_weekly_active_reward_recevie.pb.h"

#include <vector>

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

std::string quest_daily_reward_receive_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, quest_daily_reward_receive_failed_ack);
    }

    uint32_t questId = 0;
    if (!ParseOptionalUI32(req, questId))
    {
        return EncodeReply(session, quest_daily_reward_receive_failed_ack);
    }

    proto::ChangeInfo change;
    if (!session->GetPlayer()->Quests().ClaimDailyQuestReward(questId, change))
    {
        return EncodeReply(session, quest_daily_reward_receive_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, quest_daily_reward_receive_succeed_ack, &change);
}

std::string quest_weekly_reward_receive_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, quest_weekly_reward_receive_failed_ack);
    }

    uint32_t questId = 0;
    if (!ParseOptionalUI32(req, questId))
    {
        return EncodeReply(session, quest_weekly_reward_receive_failed_ack);
    }

    proto::ChangeInfo change;
    if (!session->GetPlayer()->Quests().ClaimWeeklyQuestReward(questId, change))
    {
        return EncodeReply(session, quest_weekly_reward_receive_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, quest_weekly_reward_receive_succeed_ack, &change);
}

std::string quest_daily_active_reward_receive_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, quest_daily_active_reward_receive_failed_ack);
    }

    proto::ChangeInfo change;
    std::vector<uint32_t> activeIds;
    if (!session->GetPlayer()->Quests().ClaimDailyActiveRewards(activeIds, change))
    {
        return EncodeReply(session, quest_daily_active_reward_receive_failed_ack);
    }

    proto::QuestDailyActiveRewardReceiveResp rsp;
    for (uint32_t id : activeIds)
    {
        rsp.add_activeids(id);
    }
    rsp.mutable_change()->CopyFrom(change);

    session->SavePlayer();
    return EncodeReply(session, quest_daily_active_reward_receive_succeed_ack, &rsp);
}

std::string quest_weekly_active_reward_receive_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, quest_weekly_active_reward_receive_failed_ack);
    }

    proto::ChangeInfo change;
    std::vector<uint32_t> activeIds;
    if (!session->GetPlayer()->Quests().ClaimWeeklyActiveRewards(activeIds, change))
    {
        return EncodeReply(session, quest_weekly_active_reward_receive_failed_ack);
    }

    proto::QuestWeeklyActiveRewardReceiveResp rsp;
    for (uint32_t id : activeIds)
    {
        rsp.add_activeids(id);
    }
    rsp.mutable_change()->CopyFrom(change);

    session->SavePlayer();
    return EncodeReply(session, quest_weekly_active_reward_receive_succeed_ack, &rsp);
}

std::string achievement_info_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, achievement_info_failed_ack);
    }

    auto rsp = session->GetPlayer()->Achievements().ToProto();
    return EncodeReply(session, achievement_info_succeed_ack, &rsp);
}

std::string achievement_reward_receive_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, achievement_reward_receive_failed_ack);
    }

    proto::AchievementRewardReq reqPb;
    if (!reqPb.ParseFromString(req) || reqPb.ids_size() <= 0)
    {
        return EncodeReply(session, achievement_reward_receive_failed_ack);
    }

    proto::ChangeInfo change;
    if (!session->GetPlayer()->Achievements().ClaimRewards(reqPb.ids(), change))
    {
        return EncodeReply(session, achievement_reward_receive_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, achievement_reward_receive_succeed_ack, &change);
}

std::string client_event_report_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, client_event_report_failed_ack);
    }

    proto::Nil rsp;

    proto::Events events;
    if (!req.empty() && events.ParseFromString(req))
    {
        session->GetPlayer()->Achievements().HandleClientEvents(events);
    }

    session->SavePlayer();
    return EncodeReply(session, client_event_report_succeed_ack, &rsp);
}

std::string battle_pass_quest_reward_receive_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, battle_pass_quest_reward_receive_failed_ack);
    }

    uint32_t questId = 0;
    if (!ParseOptionalUI32(req, questId) || questId == 0)
    {
        return EncodeReply(session, battle_pass_quest_reward_receive_failed_ack);
    }

    uint32_t level = 0;
    uint32_t exp = 0;
    uint32_t expThisWeek = 0;
    if (!session->GetPlayer()->Quests().ClaimBattlePassQuestReward(questId, level, exp, expThisWeek))
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

std::string daily_shop_reward_receive_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, daily_shop_reward_receive_failed_ack);
    }

    proto::ChangeInfo change;
    if (!session->GetPlayer()->Quests().ClaimDailyShopGift(change))
    {
        return EncodeReply(session, daily_shop_reward_receive_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, daily_shop_reward_receive_succeed_ack, &change);
}

std::string daily_mall_reward_receive_req__Handler(GameSession* session, const std::string& req)
{
    if (!HasLoggedInPlayer(session))
    {
        return EncodeReply(session, daily_mall_reward_receive_failed_ack);
    }

    proto::ChangeInfo change;
    if (!session->GetPlayer()->Quests().ClaimDailyMallGift(change))
    {
        return EncodeReply(session, daily_mall_reward_receive_failed_ack);
    }

    session->SavePlayer();
    return EncodeReply(session, daily_mall_reward_receive_succeed_ack, &change);
}
