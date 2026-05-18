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
        return it->second.get();
    }

    return nullptr;
}

void GameServices::AddSession(std::unique_ptr<GameSession> session)
{
    std::lock_guard lock(mSessionMutex);

    mSessionsByToken[session->token] = std::move(session);
}

GameSession* GameServices::CreateSession() {
    auto session = std::make_unique<GameSession>();

    std::lock_guard lock(mSessionMutex);

    std::string token;
    do {
        token = session->GenerateToken();
    } while (mSessionsByToken.find(token) != mSessionsByToken.end());

    session->token = token;
    mSessionsByToken[token] = std::move(session);

    return mSessionsByToken[token].get();
}
