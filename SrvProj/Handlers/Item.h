#pragma once

#include <string>

class GameSession;

std::string item_use_req__Handler(GameSession* session, const std::string& req);
std::string item_product_req__Handler(GameSession* session, const std::string& req);
std::string item_quick_growth_req__Handler(GameSession* session, const std::string& req);
