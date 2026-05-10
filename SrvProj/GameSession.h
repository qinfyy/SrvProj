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

    std::string GenerateToken();

    static std::string BuildMessage(short msgId, const std::string& payload = "");

private:
    std::vector<uint8_t> clientPublicKey;
    std::vector<uint8_t> serverPublicKey;
    std::vector<uint8_t> serverPrivateKey;
};

