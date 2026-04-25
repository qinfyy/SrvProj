#include "GameServer.h"
#include <iostream>
#include "logger.h"

GameServer::GameServer(const std::string& host, int port)
    : host_(host), port_(port)
{
    server_ = std::make_unique<httplib::Server>();
}

void GameServer::SetupRoutes() {
    // 测试接口
    server_->Get("/ping", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("pong", "text/plain");
        });

    // 模拟登录接口
    server_->Post("/login", [](const httplib::Request& req, httplib::Response& res) {
        // 简单演示：直接返回JSON
        std::string response = R"({
            "code": 0,
            "msg": "login success"
        })";

        res.set_content(response, "application/json");
        });

    // 示例：带参数
    server_->Get(R"(/player/(\d+))", [](const httplib::Request& req, httplib::Response& res) {
        std::string playerId = req.matches[1];

        std::string response = "{ \"player_id\": " + playerId + " }";
        res.set_content(response, "application/json");
        });
}

void GameServer::Start() {
    SetupRoutes();

    LOG_INFO_CFMT("GameServer starting at %s:%d", host_.c_str(), port_);

    if (!server_->listen(host_.c_str(), port_)) {
        std::cerr << "Failed to start server!" << std::endl;
    }
}

void GameServer::Stop() {
    if (server_) {
        server_->stop();
    }
}
