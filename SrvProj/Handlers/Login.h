#pragma once
#include <string>
#include "../GameSession.h"

void ike_req_Handler(GameSession* session, const std::string& req, std::string& rsp);
