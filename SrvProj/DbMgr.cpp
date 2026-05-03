#include "DbMgr.h"
#include <sstream>
#include <functional>
#include <ctime>
#include <openssl/rand.h>
#include <iomanip>
#include <stdexcept>
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

    const char* sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "   uid TEXT PRIMARY KEY,"
        "   openid TEXT UNIQUE,"
        "   token TEXT"
        ");";

    char* err = nullptr;
    if (sqlite3_exec(mDb, sql, nullptr, nullptr, &err) != SQLITE_OK)
    {
        LOG_ERROR_CFMT("[DbMgr] sqlite3_exec failed: %s", (err ? err : "unknown error"));
        sqlite3_free(err);
        return false;
    }

    LOG_INFO_CFMT("Database initialized successfully");
    return true;
}

void DbMgr::CheckInitialized() const
{
    if (!mDb)
    {
        throw std::runtime_error("数据库未初始化。请先调用 Init() 方法");
    }
}

std::string DbMgr::GetNextUid()
{
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

bool DbMgr::LoginByUidToken(const std::string& uid, const std::string& token, User& outUser)
{
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

bool DbMgr::RegisterByOpenId(const std::string& openid, User& outUser)
{
    std::lock_guard lock(mDbMutex);
    CheckInitialized();

    outUser.uid = GetNextUid();
    outUser.openId = openid;
    outUser.token = GenerateToken();
    std::string sql = "INSERT INTO users(uid, openid, token) VALUES('" + outUser.uid + "', '" + openid + "', '" + outUser.token + "');";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(mDb, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }

    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

bool DbMgr::LoginByOpenId(const std::string& openid, User& outUser)
{
    std::lock_guard lock(mDbMutex);
    CheckInitialized();

    std::string sql = "SELECT uid, token FROM users WHERE openid = '" + openid + "' LIMIT 1;";
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

std::string DbMgr::GenerateToken()
{
    unsigned char buf[16];
    RAND_bytes(buf, sizeof(buf));
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');

    for (int i = 0; i < sizeof(buf); ++i) {
        oss << std::setw(2) << (int)buf[i];
    }

    return oss.str();
}
