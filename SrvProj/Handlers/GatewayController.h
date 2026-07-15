#pragma once
#include "../AsyncTask.h"
#include "../HttpClient.h"

class RouteContext;

AsyncTask<void> AgentHandler(RouteContext& context, const HttpRequest& req, HttpResponseWriter& writer);

std::string DummyHandler(short reqId);
