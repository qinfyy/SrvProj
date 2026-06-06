#pragma once
#include <string>
#include "../GameSession.h"

std::string ike_req__Handler(GameSession* session, const std::string& req);

std::string player_login_req__Handler(GameSession* session, const std::string& req);

std::string player_data_req__Handler(GameSession* session, const std::string& req);

std::string player_reg_req__Handler(GameSession* session, const std::string& req);

std::string player_ping_req__Handler(GameSession* session, const std::string& req);

std::string energy_info_req__Handler(GameSession* session, const std::string& req);

std::string mall_package_list_req__Handler(GameSession* session, const std::string& req);

std::string potential_preselection_list_req__Handler(GameSession* session, const std::string& req);
