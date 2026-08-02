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

    static std::string EncodeMessage(short msgId, const std::string& data);
    std::string BuildMessage(short msgId, google::protobuf::Message* payload = nullptr);

    bool Login(std::string loginToken);
    bool SavePlayer();
    void SetPlayer(std::unique_ptr<Player> player);
    Player* GetPlayer() const;
    bool HasPlayer() const;
    void ClearNextPackages();

    Player* GetLoggedInPlayer() const;

    std::string mClientPublicKey;
    std::string mServerPublicKey;
    std::string mServerPrivateKey;
    std::string mAccountUid;
    int64_t mLastActiveTime = 0;

    template<typename T>
    void PushNextPackage(short msgId, T&& payload) {
        using DecayedT = std::decay_t<T>;
        static_assert(std::is_base_of_v<google::protobuf::Message, DecayedT>, "T must derive from Message");

        if constexpr (std::is_same_v<DecayedT, std::unique_ptr<google::protobuf::Message>>) {
            // 情况1：已经是 unique_ptr<Message>
            PushNextPackageImpl(msgId, std::move(payload));
        }
        else if constexpr (std::is_pointer_v<DecayedT>) {
            // 情况2：raw 指针
            if (payload) {
                PushNextPackageImpl(msgId, std::unique_ptr<google::protobuf::Message>(payload));
            }
        }
        else {
            // 情况3：栈对象或临时对象
            auto ptr = std::make_unique<DecayedT>(std::forward<T>(payload));
            PushNextPackageImpl(msgId, std::move(ptr));
        }
    }

    void PushNextPackageImpl(short msgId, std::unique_ptr<google::protobuf::Message> payload);
    bool HasNextPackages();

private:
    std::stack<std::pair<short, std::unique_ptr<google::protobuf::Message>>> mPushList;

    void AddPacketListToMe(google::protobuf::Message* payload);
};

std::string EncodeReply(GameSession* session, short msgId, google::protobuf::Message* payload = nullptr);

bool IsLoggedIn(GameSession* session);
