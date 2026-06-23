#pragma once

#include <string>
#include "../GameSession.h"

std::string quest_daily_reward_receive_req__Handler(GameSession* session, const std::string& req);
std::string quest_weekly_reward_receive_req__Handler(GameSession* session, const std::string& req);
std::string quest_daily_active_reward_receive_req__Handler(GameSession* session, const std::string& req);
std::string quest_weekly_active_reward_receive_req__Handler(GameSession* session, const std::string& req);
std::string achievement_info_req__Handler(GameSession* session, const std::string& req);
std::string achievement_reward_receive_req__Handler(GameSession* session, const std::string& req);
std::string client_event_report_req__Handler(GameSession* session, const std::string& req);
std::string daily_shop_reward_receive_req__Handler(GameSession* session, const std::string& req);
std::string daily_mall_reward_receive_req__Handler(GameSession* session, const std::string& req);
