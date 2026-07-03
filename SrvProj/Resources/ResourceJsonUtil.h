#pragma once

#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <vector>
#include <nlohmann/json.hpp>

template<typename T>
inline void ResetResourceJsonValue(T& value) {
    value = T{};
}

template<typename T>
inline void ResetResourceJsonValue(std::vector<T>& value) {
    value.clear();
}

template<typename T>
inline void ResetResourceJsonValue(std::unordered_set<T>& value) {
    value.clear();
}

template<typename T>
inline void ReadResourceJsonField(const nlohmann::json& data, const char* fieldName, T& out) {
    auto it = data.find(fieldName);
    if (it == data.end() || it->is_null()) {
        ResetResourceJsonValue(out);
        return;
    }

    try {
        if constexpr (std::is_same_v<T, std::string>) {
            if (it->is_string()) {
                out = it->get<std::string>();
            }
            else {
                out = it->dump();
            }
        }
        else {
            out = it->get<T>();
        }
    }
    catch (const std::exception& e) {
        throw std::runtime_error(std::string("读取 JSON 字段失败: ") + fieldName + ", ERROR: " + e.what());
    }
}

template<typename T>
inline void ReadResourceJsonField(const nlohmann::json& data, const char* fieldName, std::vector<T>& out) {
    auto it = data.find(fieldName);
    out.clear();
    if (it == data.end() || it->is_null()) {
        return;
    }
    if (!it->is_array()) {
        throw std::runtime_error(std::string("读取 JSON 字段失败: ") + fieldName + " 不是数组");
    }

    try {
        out = it->get<std::vector<T>>();
    }
    catch (const std::exception& e) {
        throw std::runtime_error(std::string("读取 JSON 数组字段失败: ") + fieldName + ", ERROR: " + e.what());
    }
}

template<typename T>
inline void ReadResourceJsonField(const nlohmann::json& data, const char* fieldName, std::unordered_set<T>& out) {
    auto it = data.find(fieldName);
    out.clear();
    if (it == data.end() || it->is_null()) {
        return;
    }
    if (!it->is_array()) {
        throw std::runtime_error(std::string("读取 JSON 字段失败: ") + fieldName + " 不是数组");
    }

    try {
        for (const auto& item : *it) {
            out.insert(item.get<T>());
        }
    }
    catch (const std::exception& e) {
        throw std::runtime_error(std::string("读取 JSON 集合字段失败: ") + fieldName + ", ERROR: " + e.what());
    }
}
