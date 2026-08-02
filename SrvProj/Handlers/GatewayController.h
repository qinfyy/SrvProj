#pragma once
// HttpClient 先带入 winsock2，再间接提供 AsyncTask
#include "../HttpClient.h"

class RouteContext;

AsyncTask<void> AgentHandler(RouteContext& context, const HttpRequest& req, HttpResponseWriter& writer);

std::string DummyHandler(short reqId);
