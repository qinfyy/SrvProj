#pragma once
#include <string>
#include <vector>
#include <mutex>

class GameSession
{
public:
    std::string token;
    std::string key;
    int encryptFunction; // 0 = gcm, 1 = chacha20

    void GenerateServerKey();

    void CalKey();

    std::string GenerateToken() const;

    static std::string BuildMessage(short msgId, const std::string& payload = "");

    bool Login(std::string loginToken);

    std::string clientPublicKey;
    std::string serverPublicKey;
    std::string serverPrivateKey;

    int platform;
};

