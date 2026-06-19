#pragma once

#include <string>
#include "../GameSession.h"

std::string battle_pass_info_req__Handler(GameSession* session, const std::string& req);
std::string battle_pass_reward_receive_req__Handler(GameSession* session, const std::string& req);
std::string battle_pass_level_buy_req__Handler(GameSession* session, const std::string& req);
std::string battle_pass_order_req__Handler(GameSession* session, const std::string& req);
std::string battle_pass_order_collect_req__Handler(GameSession* session, const std::string& req);
std::string battle_pass_quest_reward_receive_req__Handler(GameSession* session, const std::string& req);
