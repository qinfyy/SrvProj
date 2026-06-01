#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "./Game/Player.h"
#include <stack>

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
    static std::string EncodeMessage(short msgId, const std::string& data);
    std::string BuildMessage(short msgId, google::protobuf::Message* payload = nullptr);

    bool Login(std::string loginToken);
    bool SavePlayer();

    std::string mClientPublicKey;
    std::string mServerPublicKey;
    std::string mServerPrivateKey;
    std::string mAccountUid;
    int64_t mLastActiveTime = 0;

    void PushNextPackage(short msgId, std::unique_ptr<google::protobuf::Message> payload);
    bool HasNextPackages();

private:
    std::stack<std::pair<short, std::unique_ptr<google::protobuf::Message>>> mPushList;

	void AddPacketListToMe(google::protobuf::Message* payload);
};
