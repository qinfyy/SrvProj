#pragma once
#include "../HttpMessage.h"

void AgentHandler(const HttpRequest& req, HttpResponse& rsp);

void SetupRoutes();

std::string DummyHandler(short reqId);
