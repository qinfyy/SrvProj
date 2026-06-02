// DbMgr.h
#pragma once

#include <sqlite3.h>
#include <string>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <vector>
#include <cstdint>
#include <span>

class DbMgr
{
public:
    struct User
    {
        std::string uid;
        std::string openId;
        std::string token;
    };

public:
    static DbMgr& Instance();

    bool Init(const std::string& dbFile);

    void UnInit();

    // Account
    bool LoginByOpenId(const std::string& openid, User& outUser);

    bool RegisterByOpenId(const std::string& openid, User& outUser);

    bool LoginByUidToken(const std::string& uid, const std::string& token, User& outUser);

    // Game Data
    bool SavePlayer(uint32_t uid, std::span<const uint8_t> data);

    bool LoadPlayer(uint32_t uid, std::vector<uint8_t>& outData);
    bool LoadPlayerByUid(uint32_t uid, std::vector<uint8_t>& outData);
    bool LoadPlayerByAccountUid(const std::string& accountUid, uint32_t& outUid, std::vector<uint8_t>& outData);

    bool CreatePlayer(uint32_t uid, const std::string& accountUid, std::span<const uint8_t> data);
    bool CreatePlayer(uint32_t uid, std::span<const uint8_t> data);

    // Token
    bool GenerateToken(std::string& outToken);

private:
    DbMgr() = default;
    ~DbMgr();

    void CheckInitialized() const;

    std::string GetNextUid();
    uint32_t GetNextPlayerUid();

private:
    sqlite3* mDb = nullptr;
    std::mutex mDbMutex;

    std::mutex mUidMutex;
};
