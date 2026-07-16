#pragma once
#include <iterator>
#include <algorithm>
#include <optional>
#include "HttpClient.h"
#include "HttpMessage.h"
#include "proto/dump.pb.h"

class RouteContext;

AsyncTask<std::optional<ServerListMeta>> GetServerList(RouteContext& context);

AsyncTask<void> ServerListHandler(RouteContext& context, const HttpRequest& req, HttpResponseWriter& writer);
AsyncTask<void> NoticeListHandler(RouteContext& context, const HttpRequest& req, HttpResponseWriter& writer);
AsyncTask<void> QuickLoginHandler(RouteContext& context, const HttpRequest& req, HttpResponseWriter& writer);
AsyncTask<void> LoginHandler(RouteContext& context, const HttpRequest& req, HttpResponseWriter& writer);
AsyncTask<void> DetailHandler(RouteContext& context, const HttpRequest& req, HttpResponseWriter& writer);
AsyncTask<void> SmsHandler(RouteContext& context, const HttpRequest& req, HttpResponseWriter& writer);
AsyncTask<void> AuthHandler(RouteContext& context, const HttpRequest& req, HttpResponseWriter& writer);
AsyncTask<void> VersionHandler(RouteContext& context, const HttpRequest& req, HttpResponseWriter& writer);
AsyncTask<void> CommonConfigHandler(RouteContext& context, const HttpRequest& req, HttpResponseWriter& writer);
