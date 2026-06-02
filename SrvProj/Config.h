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

    HttpServerConfig httpServerConfig;
    std::string DatabsasePath = ".\\save.db";

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Config, httpServerConfig, DatabsasePath)

    bool LoadFromFile(const std::string& filename = ".\\Config.json");
    bool SaveToFile(const std::string& filename = ".\\Config.json");

private:
    Config() = default;
};
