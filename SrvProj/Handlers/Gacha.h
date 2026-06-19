#pragma once

#include <string>

class GameSession;

std::string gacha_spin_req__Handler(GameSession* session, const std::string& req);
std::string gacha_information_req__Handler(GameSession* session, const std::string& req);
std::string gacha_histories_req__Handler(GameSession* session, const std::string& req);
std::string gacha_guarantee_reward_receive_req__Handler(GameSession* session, const std::string& req);
std::string gacha_newbie_spin_req__Handler(GameSession* session, const std::string& req);
std::string gacha_newbie_save_req__Handler(GameSession* session, const std::string& req);
std::string gacha_newbie_obtain_req__Handler(GameSession* session, const std::string& req);
std::string gacha_newbie_info_req__Handler(GameSession* session, const std::string& req);
