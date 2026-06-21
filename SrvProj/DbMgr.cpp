#include "DbMgr.h"
#include <sstream>
#include <functional>
#include <ctime>
#include <stdexcept>
#include "Util.h"
#include "Logger.h"

DbMgr& DbMgr::Instance()
{
    static DbMgr inst;
    return inst;
}

DbMgr::~DbMgr()
{
    if (mDb)
        sqlite3_close(mDb);
}

bool DbMgr::Init(const std::string& dbFile)
{
    std::lock_guard lock(mDbMutex);

    if (mDb) {
        return true;
    }

    LOG_INFO_CFMT("Initializing database: %s", dbFile.c_str());

    if (sqlite3_open(dbFile.c_str(), &mDb) != SQLITE_OK) {
        LOG_ERROR_CFMT("[DbMgr] sqlite3_open failed: %s", (mDb ? sqlite3_errmsg(mDb) : "unknown error"));
        return false;
    }

    sqlite3_exec(mDb, "PRAGMA journal_mode=WAL;", nullptr, nullptr, nullptr);
    sqlite3_exec(mDb, "PRAGMA synchronous=NORMAL;", nullptr, nullptr, nullptr);
    sqlite3_exec(mDb, "PRAGMA temp_store=MEMORY;", nullptr, nullptr, nullptr);
    sqlite3_exec(mDb, "PRAGMA mmap_size=268435456;", nullptr, nullptr, nullptr);
    sqlite3_exec(mDb, "PRAGMA cache_size=-20000;", nullptr, nullptr, nullptr);

    const char* accountSql =
        "CREATE TABLE IF NOT EXISTS users ("
        "   uid TEXT PRIMARY KEY,"
        "   openid TEXT UNIQUE,"
        "   token TEXT"
        ");";

    char* err = nullptr;
    if (sqlite3_exec(mDb, accountSql, nullptr, nullptr, &err) != SQLITE_OK)
    {
        LOG_ERROR_CFMT("[DbMgr] sqlite3_exec failed: %s", (err ? err : "unknown error"));
        sqlite3_free(err);
        return false;
    }

    // 玩家数据表
    const char* gameSql =
        "CREATE TABLE IF NOT EXISTS players ("
        "   uid INTEGER PRIMARY KEY,"
        "   account_uid TEXT UNIQUE NOT NULL,"
        "   data BLOB NOT NULL"
        ");";

    err = nullptr;
    if (sqlite3_exec(mDb, gameSql, nullptr, nullptr, &err) != SQLITE_OK)
    {
        LOG_ERROR_CFMT("[DbMgr] sqlite3_exec players failed: %s", (err ? err : "unknown error"));
        sqlite3_free(err);
        return false;
    }

    LOG_INFO_CFMT("Database initialized successfully");
    return true;
}

void DbMgr::UnInit() {
    std::lock_guard lock(mDbMutex);

    if (!mDb)
    {
        return;
    }

    LOG_INFO("Closing database...");

    int rc = sqlite3_close(mDb);

    if (rc != SQLITE_OK)
    {
        LOG_ERROR("sqlite3_close failed: {}", sqlite3_errmsg(mDb));
        return;
    }

    mDb = nullptr;

    LOG_INFO("Database closed");
}

void DbMgr::CheckInitialized() const {
    if (!mDb)
    {
        throw std::runtime_error("数据库未初始化。请先调用 Init() 方法");
    }
}

std::string DbMgr::GetNextUid() {
    std::lock_guard lock(mUidMutex);

    std::string sql = "SELECT IFNULL(MAX(CAST(uid AS INTEGER)), 0) + 1 FROM users;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return "1";
    }

    std::string uid = "1";

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int64_t v = sqlite3_column_int64(stmt, 0);
        uid = std::to_string(v);
    }

    sqlite3_finalize(stmt);
    return uid;
}

bool DbMgr::LoginByUidToken(const std::string& uid, const std::string& token, User& outUser) {
    std::lock_guard lock(mDbMutex);
    CheckInitialized();

    std::string sql = "SELECT openid FROM users WHERE uid = '" + uid + "' AND token = '" + token + "' LIMIT 1;";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    bool ok = false;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        outUser.uid = uid;
        outUser.token = token;

        const char* openid = (const char*)sqlite3_column_text(stmt, 0);
        outUser.openId = openid ? openid : "";

        ok = true;
    }

    sqlite3_finalize(stmt);
    return ok;
}

bool DbMgr::RegisterByOpenId(const std::string& openid, User& outUser) {
    std::lock_guard lock(mDbMutex);
    CheckInitialized();

    bool ok = false;

    outUser.uid = GetNextUid();
    outUser.openId = openid;

    if (!GenerateToken(outUser.token, true)) {
        LOG_ERROR("用户令牌生成失败");
        return ok;
    }

    std::string sql = "INSERT INTO users(uid, openid, token) VALUES('" + outUser.uid + "', '" + openid + "', '" + outUser.token + "');";
    
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    ok = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    return ok;
}

bool DbMgr::LoginByOpenId(const std::string& openid, User& outUser) {
    std::lock_guard lock(mDbMutex);
    CheckInitialized();

    std::string sql =
        "SELECT uid, token FROM users WHERE openid = '" +
        openid + "' LIMIT 1;";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    bool ok = false;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const char* uid = (const char*)sqlite3_column_text(stmt, 0);
        const char* token = (const char*)sqlite3_column_text(stmt, 1);

        outUser.uid = uid ? uid : "";
        outUser.token = token ? token : "";
        outUser.openId = openid;
        ok = true;
    }

    sqlite3_finalize(stmt);
    return ok;
}

uint32_t DbMgr::GetNextPlayerUid() {
    std::string sql = "SELECT IFNULL(MAX(uid), 0) + 1 FROM players;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return 1;
    }

    uint32_t uid = 1;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        uid = static_cast<uint32_t>(sqlite3_column_int64(stmt, 0));
    }

    sqlite3_finalize(stmt);
    return uid;
}

// Game Data
bool DbMgr::SavePlayer(uint32_t uid, std::span<const uint8_t> data) {
    std::lock_guard lock(mDbMutex);
    CheckInitialized();

    sqlite3_stmt* stmt = nullptr;

    const char* sql = "UPDATE players SET data = ? WHERE uid = ?;";

    if (sqlite3_prepare_v2(mDb, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_blob(stmt, 1, data.data(), static_cast<int>(data.size()), SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 2, uid);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok && sqlite3_changes(mDb) > 0;
}

bool DbMgr::LoadPlayer(uint32_t uid, std::vector<uint8_t>& outData) {
    return LoadPlayerByUid(uid, outData);
}

bool DbMgr::LoadPlayerByUid(uint32_t uid, std::vector<uint8_t>& outData) {
    std::lock_guard lock(mDbMutex);
    CheckInitialized();

    sqlite3_stmt* stmt = nullptr;

    std::string sql = "SELECT data FROM players WHERE uid = " + std::to_string(uid) + " LIMIT 1;";

    if (sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        return false;
    }

    bool ok = false;

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        const void* blob = sqlite3_column_blob(stmt, 0);
        int size = sqlite3_column_bytes(stmt, 0);
        outData.resize(size);

        if (size > 0 && blob)
        {
            memcpy(outData.data(), blob, size);
        }

        ok = true;
    }

    sqlite3_finalize(stmt);

    return ok;
}

bool DbMgr::LoadPlayerByAccountUid(const std::string& accountUid, uint32_t& outUid, std::vector<uint8_t>& outData) {
    std::lock_guard lock(mDbMutex);
    CheckInitialized();

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT uid, data FROM players WHERE account_uid = ? LIMIT 1;";

    if (sqlite3_prepare_v2(mDb, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_text(stmt, 1, accountUid.c_str(), static_cast<int>(accountUid.size()), SQLITE_TRANSIENT);

    bool ok = false;
    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        outUid = static_cast<uint32_t>(sqlite3_column_int64(stmt, 0));
        const void* blob = sqlite3_column_blob(stmt, 1);
        int size = sqlite3_column_bytes(stmt, 1);
        outData.resize(size);

        if (size > 0 && blob)
        {
            memcpy(outData.data(), blob, size);
        }

        ok = true;
    }

    sqlite3_finalize(stmt);
    return ok;
}

bool DbMgr::CreatePlayer(uint32_t uid, const std::string& accountUid, std::span<const uint8_t> data) {
    std::lock_guard lock(mDbMutex);
    CheckInitialized();

    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO players(uid, account_uid, data) VALUES(?, ?, ?);";

    if (uid == 0)
    {
        uid = GetNextPlayerUid();
    }

    if (sqlite3_prepare_v2(mDb, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        return false;
    }

    sqlite3_bind_int64(stmt, 1, uid);
    sqlite3_bind_text(stmt, 2, accountUid.c_str(), static_cast<int>(accountUid.size()), SQLITE_TRANSIENT);
    sqlite3_bind_blob(stmt, 3, data.data(), static_cast<int>(data.size()), SQLITE_TRANSIENT);

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

bool DbMgr::CreatePlayer(uint32_t uid, std::span<const uint8_t> data) {
    return CreatePlayer(uid, std::to_string(uid), data);
}
