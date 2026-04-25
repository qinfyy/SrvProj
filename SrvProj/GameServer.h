#pragma once
#include <string>
#include <memory>
#include <httplib.h>

class GameServer {
public:
    GameServer(const std::string& host, int port);

    // 启动服务器（阻塞）
    void Start();

    // 停止服务器
    void Stop();

private:
    void SetupRoutes();

private:
    std::string host_;
    int port_;

    std::unique_ptr<httplib::Server> server_;
};