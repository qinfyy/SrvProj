#pragma once

#include <algorithm>
#include <cctype>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <map>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <nlohmann/json.hpp>

constexpr int GOLD_ITEM_ID = 1;
constexpr int QUEST_TYPE_BATTLE_PASS_DAILY = 6;
constexpr int QUEST_TYPE_BATTLE_PASS_WEEKLY = 7;

struct ItemRewardParam {
    int Id = 0;
    int Min = 0;
    int Max = 0;

    ItemRewardParam() = default;
    ItemRewardParam(int id, int min, int max) : Id(id), Min(min), Max(max) {}

    int GetRandomCount() const {
        if (Min == Max) {
            return Min;
        }

        const int low = std::min(Min, Max);
        const int high = std::max(Min, Max);
        static thread_local std::mt19937 rng{ std::random_device{}() };
        std::uniform_int_distribution<int> dist(low, high);
        return dist(rng);
    }
};

using ItemRewardList = std::vector<ItemRewardParam>;

struct ItemParamMap {
    std::map<int, int> Items;

    void Add(int itemId, int count) {
        if (itemId <= 0 || count == 0) {
            return;
        }

        Items[itemId] += count;
    }

    bool Empty() const { return Items.empty(); }
    size_t Size() const { return Items.size(); }

    static ItemParamMap FromJsonString(const std::string& jsonString) {
        ItemParamMap out;
        if (jsonString.empty()) {
            return out;
        }

        try {
            auto json = nlohmann::json::parse(jsonString);
            if (json.is_object()) {
                for (auto it = json.begin(); it != json.end(); ++it) {
                    out.Add(std::stoi(it.key()), it.value().get<int>());
                }
            }
            else if (json.is_array()) {
                for (const auto& item : json) {
                    if (item.is_array() && item.size() >= 2) {
                        out.Add(item.at(0).get<int>(), item.at(1).get<int>());
                    }
                    else if (item.is_object()) {
                        int id = 0;
                        int count = 0;
                        if (item.contains("Id")) id = item.at("Id").get<int>();
                        else if (item.contains("id")) id = item.at("id").get<int>();
                        else if (item.contains("Tid")) id = item.at("Tid").get<int>();
                        else if (item.contains("tid")) id = item.at("tid").get<int>();

                        if (item.contains("Count")) count = item.at("Count").get<int>();
                        else if (item.contains("count")) count = item.at("count").get<int>();
                        else if (item.contains("Qty")) count = item.at("Qty").get<int>();
                        else if (item.contains("qty")) count = item.at("qty").get<int>();

                        out.Add(id, count);
                    }
                }
            }
        }
        catch (...) {
        }

        return out;
    }
};

template<typename T>
struct WeightedList {
    std::vector<std::pair<double, T>> Items;
    double Total = 0.0;

    WeightedList<T>& Add(double weight, const T& item) {
        if (weight <= 0) {
            return *this;
        }

        Total += weight;
        Items.emplace_back(Total, item);
        return *this;
    }

    void Clear() {
        Items.clear();
        Total = 0.0;
    }

    size_t Size() const { return Items.size(); }
    bool Empty() const { return Items.empty(); }
};

struct QuestParams {
    int CompleteCond = 0;
    std::vector<int> CompleteCondParams;
};

inline std::vector<int> ParseIntArrayText(const std::string& text) {
    std::vector<int> values;
    std::string token;

    auto flush = [&]() {
        if (!token.empty() && token != "-") {
            values.push_back(std::stoi(token));
        }
        token.clear();
    };

    for (char c : text) {
        if (std::isdigit(static_cast<unsigned char>(c)) || (c == '-' && token.empty())) {
            token.push_back(c);
        }
        else {
            flush();
        }
    }
    flush();

    return values;
}

inline std::vector<std::vector<int>> ParseIntMatrixJson(const std::string& text) {
    std::vector<std::vector<int>> rows;
    if (text.empty()) {
        return rows;
    }

    try {
        auto json = nlohmann::json::parse(text);
        if (!json.is_array()) {
            return rows;
        }

        for (const auto& rowJson : json) {
            if (!rowJson.is_array()) {
                continue;
            }

            std::vector<int> row;
            for (const auto& value : rowJson) {
                row.push_back(value.get<int>());
            }
            if (!row.empty()) {
                rows.emplace_back(std::move(row));
            }
        }
    }
    catch (...) {
    }

    return rows;
}

inline void ParseRewardPreview(const std::string& text, ItemRewardList& firstRewards, ItemRewardList& rewards, bool useThirdValueAsMax = true) {
    firstRewards.clear();
    rewards.clear();

    for (const auto& award : ParseIntMatrixJson(text)) {
        if (award.size() < 2) {
            continue;
        }

        int itemId = award[0];
        int min = award[1];
        int max = (useThirdValueAsMax && award.size() >= 4) ? award[2] : min;
        const bool isFirst = award.back() == 1;

        if (min == -1) {
            min = 0;
            max = 1;
        }

        ItemRewardParam reward(itemId, min, max);
        if (isFirst) {
            firstRewards.push_back(reward);
        }
        else {
            rewards.push_back(reward);
        }
    }
}

inline void ParseRewardPreviewNoFirst(const std::string& text, ItemRewardList& rewards) {
    ItemRewardList ignored;
    ParseRewardPreview(text, ignored, rewards);
}

inline long long DateToSeconds(const std::string& text) {
    if (text.empty()) {
        return 0;
    }

    std::tm tm{};
    std::istringstream ss(text.substr(0, 19));
    if (text.find('T') != std::string::npos) {
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    }
    else {
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    }

    if (ss.fail()) {
        return 0;
    }

#if defined(_WIN32)
    return static_cast<long long>(_mkgmtime(&tm));
#else
    return static_cast<long long>(timegm(&tm));
#endif
}

inline QuestParams GetBattlePassQuestParams(int id) {
    switch (id) {
    case 1001: return { 51, { 1 } };
    case 1002: return { 39, { 160 } };
    case 1003: return { 3, { 6 } };
    case 1004: return { 55, { 5, 2 } };
    case 2001: return { 538, { 1 } };
    case 2002: return { 92, { 3 } };
    case 2003: return { 3, { 20 } };
    case 2004: return { 51, { 5 } };
    case 2005: return { 83, { 3 } };
    case 2006: return { 49, { 100000, 1 } };
    case 2007: return { 45, { 5 } };
    case 2008: return { 39, { 1200 } };
    default: return { 0, { 0 } };
    }
}

inline int GetAchievementParamCondOrEquals(int value) {
    return (value >= 0 && value <= 6) ? value : 0;
}

inline bool TestAchievementParamCond(int cond, int param, int value) {
    switch (GetAchievementParamCondOrEquals(cond)) {
    case 1: return true;
    case 2: return value != param;
    case 3: return value > param;
    case 4: return value >= param;
    case 5: return value < param;
    case 6: return value <= param;
    case 0:
    default: return value == param;
    }
}
