#pragma once

#include <string>

class GameSession;

std::string mail_list_req__Handler(GameSession* session, const std::string& req);
std::string mail_read_req__Handler(GameSession* session, const std::string& req);
std::string mail_recv_req__Handler(GameSession* session, const std::string& req);
std::string mail_remove_req__Handler(GameSession* session, const std::string& req);
std::string mail_pin_req__Handler(GameSession* session, const std::string& req);
