#pragma once
#include "GameSession.h"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <mutex>

class Player;

class GameServices
{
public:
    static GameServices& Instance();

    GameServices(const GameServices&) = delete;
    GameServices& operator=(const GameServices&) = delete;
    GameServices(GameServices&&) = delete;
    GameServices& operator=(GameServices&&) = delete;

    void Init();

    void Shutdown();
    
    GameSession* GetSessionByToken(const std::string& token);
    Player* GetPlayerByUid(uint32_t uid);
    GameSession* GetSessionByPlayerUid(uint32_t uid);
    void AddSession(std::unique_ptr<GameSession> session);
    bool ForceSaveAllPlayerData();
    void CleanupExpiredSessions();
    bool KickSessionByPlayerUid(uint32_t uid);

    GameSession* CreateSession();

private:
    GameServices() = default;
    ~GameServices() = default;

    std::unordered_map<std::string, std::unique_ptr<GameSession>> mSessionsByToken;
    std::mutex mSessionMutex;

};
