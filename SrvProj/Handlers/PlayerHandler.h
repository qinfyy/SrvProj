#pragma once

#include <string>
#include "../GameSession.h"

std::string player_learn_req__Handler(GameSession* session, const std::string& req);
std::string player_signature_edit_req__Handler(GameSession* session, const std::string& req);
std::string phone_contacts_info_req__Handler(GameSession* session, const std::string& req);
std::string phone_contacts_report_req__Handler(GameSession* session, const std::string& req);
std::string phone_contacts_top_req__Handler(GameSession* session, const std::string& req);
std::string player_board_set_req__Handler(GameSession* session, const std::string& req);
std::string player_chars_show_req__Handler(GameSession* session, const std::string& req);
std::string player_gender_edit_req__Handler(GameSession* session, const std::string& req);
std::string player_head_icon_info_req__Handler(GameSession* session, const std::string& req);
std::string player_head_icon_set_req__Handler(GameSession* session, const std::string& req);
std::string player_honor_edit_req__Handler(GameSession* session, const std::string& req);
std::string player_music_set_req__Handler(GameSession* session, const std::string& req);
std::string player_name_edit_req__Handler(GameSession* session, const std::string& req);
std::string player_skin_show_req__Handler(GameSession* session, const std::string& req);
std::string player_title_edit_req__Handler(GameSession* session, const std::string& req);
std::string player_world_class_reward_receive_req__Handler(GameSession* session, const std::string& req);
