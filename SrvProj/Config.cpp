#include "Config.h"
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include "Logger.h"

bool Config::LoadFromFile(const std::string & filename)
{
    std::ifstream in(filename);
    if (!in.is_open())
    {
        SaveToFile(filename);
        return false;
    }

    try
    {
        nlohmann::json j;
        in >> j;

        from_json(j, *this);
        return true;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("加载配置失败: {}", e.what());
        return false;
    }
}

bool Config::SaveToFile(const std::string& filename)
{
    try
    {
        nlohmann::json j = *this;
        std::ofstream out(filename);
        if (!out.is_open()) return false;

        out << std::setw(4) << j;
        return true;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("保存配置失败: {}", e.what());
        return false;
    }
}
