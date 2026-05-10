#include "GameServices.h"
#include <mutex>

GameServices& GameServices::Instance()
{
    static GameServices instance;
    return instance;
}

GameSession* GameServices::GetSessionByToken(const std::string& token)
{
    std::lock_guard lock(mSessionMutex);

    auto it = mSessionsByToken.find(token);
    if (it != mSessionsByToken.end())
    {
        return it->second;
    }

    return nullptr;
}

void GameServices::AddSession(GameSession* session)
{
    std::lock_guard lock(mSessionMutex);

    mSessionsByToken[session->token] = session;
}