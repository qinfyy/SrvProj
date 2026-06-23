#pragma once

#include <string>

class GameSession;

std::string player_formation_req__Handler(GameSession* session, const std::string& req);

std::string potential_preselection_list_req__Handler(GameSession* session, const std::string& req);
std::string potential_preselection_import_req__Handler(GameSession* session, const std::string& req);
std::string potential_preselection_name_set_req__Handler(GameSession* session, const std::string& req);
std::string potential_preselection_preference_set_req__Handler(GameSession* session, const std::string& req);
std::string potential_preselection_update_req__Handler(GameSession* session, const std::string& req);
std::string potential_preselection_delete_req__Handler(GameSession* session, const std::string& req);

std::string star_tower_build_brief_list_get_req__Handler(GameSession* session, const std::string& req);
std::string star_tower_apply_req__Handler(GameSession* session, const std::string& req);
std::string star_tower_build_delete_req__Handler(GameSession* session, const std::string& req);
std::string star_tower_build_detail_get_req__Handler(GameSession* session, const std::string& req);
std::string star_tower_build_lock_unlock_req__Handler(GameSession* session, const std::string& req);
std::string star_tower_build_name_set_req__Handler(GameSession* session, const std::string& req);
std::string star_tower_build_preference_set_req__Handler(GameSession* session, const std::string& req);
std::string star_tower_build_whether_save_req__Handler(GameSession* session, const std::string& req);
std::string star_tower_give_up_req__Handler(GameSession* session, const std::string& req);
std::string star_tower_info_req__Handler(GameSession* session, const std::string& req);
std::string star_tower_interact_req__Handler(GameSession* session, const std::string& req);
std::string star_tower_book_potential_brief_list_get_req__Handler(GameSession* session, const std::string& req);
std::string star_tower_book_char_potential_get_req__Handler(GameSession* session, const std::string& req);
std::string star_tower_book_potential_reward_receive_req__Handler(GameSession* session, const std::string& req);
std::string star_tower_book_event_reward_receive_req__Handler(GameSession* session, const std::string& req);
std::string tower_book_fate_card_detail_req__Handler(GameSession* session, const std::string& req);
std::string tower_book_fate_card_reward_receive_req__Handler(GameSession* session, const std::string& req);
std::string npc_affinity_book_get_req__Handler(GameSession* session, const std::string& req);
std::string npc_affinity_plot_reward_receive_req__Handler(GameSession* session, const std::string& req);
std::string tower_growth_detail_req__Handler(GameSession* session, const std::string& req);
std::string tower_growth_group_node_unlock_req__Handler(GameSession* session, const std::string& req);
std::string tower_growth_node_unlock_req__Handler(GameSession* session, const std::string& req);
