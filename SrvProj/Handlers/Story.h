#pragma once

#include <string>

class GameSession;

std::string story_apply_req__Handler(GameSession* session, const std::string& req);
std::string story_settle_req__Handler(GameSession* session, const std::string& req);
std::string story_set_info_req__Handler(GameSession* session, const std::string& req);
std::string story_set_reward_receive_req__Handler(GameSession* session, const std::string& req);
std::string plot_reward_receive_req__Handler(GameSession* session, const std::string& req);
