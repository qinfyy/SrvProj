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

    class ResourceConfig {
    public:
        std::string type = "arcx";
        std::string path = ".\\data.arcx";

        NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ResourceConfig, type, path)
    };

    class ServerTime {
    public:
        bool spoofTime = false;
        std::string spoofDate = "2025-11-01 08:00:00"; // yyyy-mm-dd HH:MM:SS

        NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ServerTime, spoofTime, spoofDate)
    };

    HttpServerConfig httpServerConfig;
    ResourceConfig resourceConfig;
    std::string DatabasePath = ".\\save.db";
    std::string TimeZone = "UTC";
    ServerTime serverTime;
    std::vector<std::string> playerDefaultPermissions = { "*" };
    bool unlockAllStarTower = true;
    bool unlockInstances = true;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Config, httpServerConfig, resourceConfig, DatabasePath, TimeZone, serverTime, playerDefaultPermissions, unlockAllStarTower, unlockInstances)

    bool LoadFromFile(const std::string& filename = ".\\Config.json");
    bool SaveToFile(const std::string& filename = ".\\Config.json");

private:
    Config() = default;
};
