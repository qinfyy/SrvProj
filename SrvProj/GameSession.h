#pragma once
#include <string>
#include <mutex>
#include "./Game/Player.h"

class GameSession
{
public:
    std::string mToken;
    std::string mKey;
    int mEncryptFunction; // 0 = AES GCM, 1 = ChaCha20Poly1305

    std::unique_ptr<Player> mPlayer;

    bool GenerateServerKey();

    bool CalKey();

    static std::string GenerateToken();

    static std::string BuildMessage(short msgId, const std::string& payload = "");

    bool Login(std::string loginToken);

    std::string mClientPublicKey;
    std::string mServerPublicKey;
    std::string mServerPrivateKey;
    int64_t mLastActiveTime;
};

