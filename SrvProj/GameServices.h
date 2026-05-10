#pragma once
#include "GameSession.h"
#include <string>
#include <unordered_map>
#include <mutex>

class GameServices
{
public:
    static GameServices& Instance();

    GameServices(const GameServices&) = delete;
    GameServices& operator=(const GameServices&) = delete;
    GameServices(GameServices&&) = delete;
    GameServices& operator=(GameServices&&) = delete;

    void Init()
    {

    }

    void Shutdown()
    {

    }
    
    GameSession* GetSessionByToken(const std::string& token);
    void AddSession(GameSession* session);

private:
    GameServices() = default;
    ~GameServices() = default;

	std::unordered_map<std::string, GameSession*> mSessionsByToken;
    std::mutex mSessionMutex;

};