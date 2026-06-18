#pragma once

#include <string>
#include "../GameSession.h"

std::string player_learn_req__Handler(GameSession* session, const std::string& req);
std::string player_signature_edit_req__Handler(GameSession* session, const std::string& req);
