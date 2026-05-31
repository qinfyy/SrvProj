#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "./Game/Player.h"

class GameSession
{
public:
    std::string mToken;
    std::string mKey;
    int mEncryptFunction = 0; // 0 = AES GCM, 1 = ChaCha20Poly1305

    std::unique_ptr<Player> mPlayer;

    bool GenerateServerKey();
    bool CalKey();

    static std::string GenerateToken();
    static std::string BuildMessage(short msgId, const std::string& payload = "");

    bool Login(std::string loginToken);
    bool SavePlayer();

    std::string mClientPublicKey;
    std::string mServerPublicKey;
    std::string mServerPrivateKey;
    std::string mAccountUid;
    int64_t mLastActiveTime = 0;
};
