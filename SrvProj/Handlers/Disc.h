#pragma once

#include <string>

class GameSession;

std::string disc_strengthen_req__Handler(GameSession* session, const std::string& req);
std::string disc_promote_req__Handler(GameSession* session, const std::string& req);
std::string disc_limit_break_req__Handler(GameSession* session, const std::string& req);
std::string disc_all_limit_break_req__Handler(GameSession* session, const std::string& req);
std::string disc_read_reward_receive_req__Handler(GameSession* session, const std::string& req);
