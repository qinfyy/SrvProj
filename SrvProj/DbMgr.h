#pragma once

#include "sqlite3.h"
#include <string>
#include <mutex>
#include <optional>
#include <stdexcept>

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

    bool LoginByOpenId(const std::string& openid, User& outUser);

    bool RegisterByOpenId(const std::string& openid, User& outUser);

    bool LoginByUidToken(const std::string& uid, const std::string& token, User& outUser);

private:
    DbMgr() = default;
    ~DbMgr();

    void CheckInitialized() const;

    std::string GenerateToken();

    std::string GetNextUid();

private:
    sqlite3* mDb = nullptr;
    std::mutex mDbMutex;
};
