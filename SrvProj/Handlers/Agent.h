#pragma once

#include <string>
#include "../GameSession.h"

std::string agent_apply_req__Handler(GameSession* session, const std::string& req);
std::string agent_give_up_req__Handler(GameSession* session, const std::string& req);
std::string agent_reward_receive_req__Handler(GameSession* session, const std::string& req);
