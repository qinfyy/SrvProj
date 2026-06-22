#pragma once
#include <string>
#include <array>
#include <vector>
#include <nlohmann/json.hpp>

class Config
{
public:
    static Config& Get()
    {
        static Config instance;
        return instance;
    }

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    class HttpServerConfig {
    public:
        std::string ip = "0.0.0.0";
        std::string publicIp = "127.0.0.1";
        int port = 21000;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(HttpServerConfig, ip, publicIp, port)
    };

    class ServerTime {
    public:
        bool spoofTime = false;
        std::string spoofDate = "2025-11-01 08:00:00"; // yyyy-mm-dd HH:MM:SS

        NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ServerTime, spoofTime, spoofDate)
    };

    HttpServerConfig httpServerConfig;
    std::string DatabasePath = ".\\save.db";
    std::string TimeZone = "UTC";
    ServerTime serverTime;
    std::vector<std::string> playerDefaultPermissions = { "*" };

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Config, httpServerConfig, DatabasePath, TimeZone, serverTime, playerDefaultPermissions)

    bool LoadFromFile(const std::string& filename = ".\\Config.json");
    bool SaveToFile(const std::string& filename = ".\\Config.json");

private:
    Config() = default;
};
