#pragma once

#include <string>

class GameSession;

std::string mall_gem_list_req__Handler(GameSession* session, const std::string& req);
std::string mall_gem_order_req__Handler(GameSession* session, const std::string& req);
std::string mall_order_cancel_req__Handler(GameSession* session, const std::string& req);
std::string mall_order_collect_req__Handler(GameSession* session, const std::string& req);
std::string mall_monthlyCard_list_req__Handler(GameSession* session, const std::string& req);
std::string mall_monthlyCard_order_req__Handler(GameSession* session, const std::string& req);
std::string mall_package_list_req__Handler(GameSession* session, const std::string& req);
std::string mall_package_order_req__Handler(GameSession* session, const std::string& req);
std::string mall_shop_list_req__Handler(GameSession* session, const std::string& req);
std::string mall_shop_order_req__Handler(GameSession* session, const std::string& req);
std::string gem_convert_req__Handler(GameSession* session, const std::string& req);
