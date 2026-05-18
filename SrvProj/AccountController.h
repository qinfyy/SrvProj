#pragma once
#include <iterator>
#include <algorithm>
#include <optional>
#include "proto/dump.pb.h"
#include "HttpMessage.h"

std::optional<ServerListMeta> GetServerList();

void ServerListHandler(const HttpRequest& req, HttpResponse& rsp);

void NoticeListHandler(const HttpRequest& req, HttpResponse& rsp);

void QuickLoginHandler(const HttpRequest& req, HttpResponse& rsp);

void LoginHandler(const HttpRequest& req, HttpResponse& rsp);

void DetailHandler(const HttpRequest& req, HttpResponse& rsp);

void SmsHandler(const HttpRequest& req, HttpResponse& rsp);

void AuthHandler(const HttpRequest& req, HttpResponse& rsp);

void VersionHandler(const HttpRequest& req, HttpResponse& rsp);

void CommonConfigHandler(const HttpRequest& req, HttpResponse& rsp);
