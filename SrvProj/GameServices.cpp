#include "GameServices.h"
#include <mutex>
#include "Logger.h"
#include "GameTime.h"
#include "Util.h"

GameServices& GameServices::Instance()
{
    static GameServices instance;
    return instance;
}

void GameServices::Init() {

}

namespace {
constexpr int64_t SessionTimeoutMilliseconds = 5LL * 60LL * 1000LL;
}

void GameServices::Shutdown() {
	ForceSaveAllPlayerData();
}

GameSession* GameServices::GetSessionByToken(const std::string& token)
{
    std::lock_guard lock(mSessionMutex);

    auto it = mSessionsByToken.find(token);
    if (it != mSessionsByToken.end())
    {
        return it->second.get();
    }

    return nullptr;
}

Player* GameServices::GetPlayerByUid(uint32_t uid)
{
    std::lock_guard lock(mSessionMutex);

    for (auto& [token, session] : mSessionsByToken)
    {
        if (!session || !session->HasPlayer())
        {
            continue;
        }

        auto* player = session->GetPlayer();
        if (player && player->GetUid() == uid)
        {
            return player;
        }
    }

    return nullptr;
}

GameSession* GameServices::GetSessionByPlayerUid(uint32_t uid)
{
    std::lock_guard lock(mSessionMutex);

    for (auto& [token, session] : mSessionsByToken)
    {
        if (!session || !session->HasPlayer())
        {
            continue;
        }

        auto* player = session->GetPlayer();
        if (player && player->GetUid() == uid)
        {
            return session.get();
        }
    }

    return nullptr;
}

void GameServices::AddSession(std::unique_ptr<GameSession> session)
{
    std::lock_guard lock(mSessionMutex);

    mSessionsByToken[session->mToken] = std::move(session);
}

bool GameServices::ForceSaveAllPlayerData()
{
    std::lock_guard lock(mSessionMutex);

    size_t totalCount = 0;
    size_t successCount = 0;
    size_t failedCount = 0;

    for (const auto& pair : mSessionsByToken)
    {
        auto* session = pair.second.get();
        if (!session || !session->HasPlayer())
        {
            continue;
        }

        ++totalCount;
        auto* player = session->GetPlayer();
        const uint32_t uid = player ? player->GetUid() : 0;
        if (session->SavePlayer())
        {
            ++successCount;
            LOG_DEBUG("强制保存在线玩家成功, uid: {}", uid);
        }
        else
        {
            ++failedCount;
            LOG_ERROR("强制保存在线玩家失败, uid: {}", uid);
        }
    }

    LOG_INFO("强制保存在线玩家完成, 总数: {}, 成功: {}, 失败: {}", totalCount, successCount, failedCount);
    return failedCount == 0;
}

GameSession* GameServices::CreateSession() {
    std::lock_guard lock(mSessionMutex);

    std::string token;
    do {
        bool success = GenerateToken(token, false);
		if (!success) {
			LOG_ERROR("生成会话令牌失败");
			return nullptr;
		}
    } while (mSessionsByToken.find(token) != mSessionsByToken.end());

    mSessionsByToken[token] = std::make_unique<GameSession>();
    auto rawPtr = mSessionsByToken[token].get();

    rawPtr->mToken = token;

    return rawPtr;
}


void GameServices::CleanupExpiredSessions()
{
    std::lock_guard lock(mSessionMutex);
    const int64_t now = GameTime::NowMilliseconds();

    for (auto it = mSessionsByToken.begin(); it != mSessionsByToken.end(); )
    {
        auto* session = it->second.get();
        if (!session)
        {
            it = mSessionsByToken.erase(it);
            continue;
        }

        if (session->mLastActiveTime > 0 && now - session->mLastActiveTime >= SessionTimeoutMilliseconds)
        {
            if (session->HasPlayer())
            {
                session->SavePlayer();
            }
            LOG_INFO("会话过期, token: {}", session->mToken);
            it = mSessionsByToken.erase(it);
            continue;
        }

        ++it;
    }
}

bool GameServices::KickSessionByPlayerUid(uint32_t uid)
{
    std::lock_guard lock(mSessionMutex);
    for (auto it = mSessionsByToken.begin(); it != mSessionsByToken.end(); ++it)
    {
        auto* session = it->second.get();
        if (!session || !session->HasPlayer())
        {
            continue;
        }

        auto* player = session->GetPlayer();
        if (!player || player->GetUid() != uid)
        {
            continue;
        }

        session->SavePlayer();
        LOG_INFO("踢出玩家, uid: {}, token: {}", uid, session->mToken);
        mSessionsByToken.erase(it);
        return true;
    }

    return false;
}

void GameServices::RegisterWebOrderContext(const WebOrderContext& context)
{
    if (context.Token.empty())
    {
        return;
    }

    std::lock_guard lock(mWebOrderMutex);
    mWebOrderContexts[context.Token] = context;
}

bool GameServices::GetWebOrderContext(const std::string& token, WebOrderContext& outContext)
{
    if (token.empty())
    {
        return false;
    }

    std::lock_guard lock(mWebOrderMutex);
    const auto it = mWebOrderContexts.find(token);
    if (it == mWebOrderContexts.end())
    {
        return false;
    }

    outContext = it->second;
    return true;
}

void GameServices::RemoveWebOrderContext(const std::string& token)
{
    if (token.empty())
    {
        return;
    }

    std::lock_guard lock(mWebOrderMutex);
    mWebOrderContexts.erase(token);
}
