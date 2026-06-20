#include "GameServices.h"
#include <mutex>
#include "Logger.h"

GameServices& GameServices::Instance()
{
    static GameServices instance;
    return instance;
}

void GameServices::Init() {

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
        token = GameSession::GenerateToken();
    } while (mSessionsByToken.find(token) != mSessionsByToken.end());

    mSessionsByToken[token] = std::make_unique<GameSession>();
    auto rawPtr = mSessionsByToken[token].get();

    rawPtr->mToken = token;

    return rawPtr;
}
