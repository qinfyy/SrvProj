#pragma once

#include <string>

class GameSession;

std::string char_upgrade_req__Handler(GameSession* session, const std::string& req);
std::string char_advance_req__Handler(GameSession* session, const std::string& req);
std::string char_skill_upgrade_req__Handler(GameSession* session, const std::string& req);
std::string char_skin_set_req__Handler(GameSession* session, const std::string& req);
std::string char_affinity_gift_send_req__Handler(GameSession* session, const std::string& req);
std::string char_favorite_set_req__Handler(GameSession* session, const std::string& req);
std::string char_gem_use_preset_req__Handler(GameSession* session, const std::string& req);
std::string char_gem_equip_gem_req__Handler(GameSession* session, const std::string& req);
std::string char_gem_refresh_req__Handler(GameSession* session, const std::string& req);
std::string char_gem_replace_attribute_req__Handler(GameSession* session, const std::string& req);
std::string char_gem_update_gem_lock_status_req__Handler(GameSession* session, const std::string& req);
std::string char_gem_overlock_req__Handler(GameSession* session, const std::string& req);
