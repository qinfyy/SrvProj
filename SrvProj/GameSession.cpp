#include "GameSession.h"

#include "AeadTool.h"
#include "DbMgr.h"
#include "GameServices.h"
#include "Logger.h"
#include "Util.h"

#include <array>
#include <chrono>
#include <openssl/rand.h>
#include <random>
#include <sstream>
#include <span>
#include <vector>

#include "./Game/ActivityMgr.h"
#include "./Game/CharacterMgr.h"
#include "./Game/InventoryMgr.h"
#include "./Game/QuestMgr.h"

#include "proto/proto_cpp/public.pb.h"
using namespace proto;

bool GameSession::GenerateServerKey() {
    try {
        auto EcdhPair = AeadTool::GetECDHKeyPair();

        if (EcdhPair.first.empty() || EcdhPair.second.empty()) {
            return false;
        }

        mServerPrivateKey = EcdhPair.second;
        mServerPublicKey = EcdhPair.first;
        return true;
	}
    catch (const std::exception& ex) {
        LOG_ERROR("生成 ECDH 密钥对失败: {}", ex.what());
        return false;
    }
}

bool GameSession::CalKey() {
    try {
        auto sharedKey = AeadTool::CalECDHSharedKey(mServerPrivateKey, mClientPublicKey);
        auto info = AeadTool::CalInfo(mClientPublicKey, mServerPublicKey);
        auto secretX = AeadTool::CalSecretX(mServerPublicKey, info, sharedKey);

	    LOG_DEBUG("clientPunlicKey: {}", Base64Encode(mClientPublicKey));
        LOG_DEBUG("serverPublicKey: {}", Base64Encode(mServerPublicKey));
        LOG_DEBUG("serverPrivateKey: {}", Base64Encode(mServerPrivateKey));
        LOG_DEBUG("sharedKey: {}", Base64Encode(sharedKey));
        LOG_DEBUG("secretX: {}", Base64Encode(secretX));
        mKey = secretX;
    }
    catch (const std::exception& ex) {
        LOG_ERROR("计算密钥失败: {}", ex.what());
        return false;
    }

    std::random_device rd;
    std::minstd_rand0 gen(rd());
    std::uniform_int_distribution<int> dis(0, 1);
    mEncryptFunction = dis(gen);

	return true;
}

bool GameSession::Login(std::string loginToken) {
    if (loginToken.empty()) {
        if (!mToken.empty()) {
            loginToken = mToken;
        }
        else {
            LOG_ERROR("会话令牌为空");
            return false;
        }
    }

    mAccountUid = loginToken;

    uint32_t uid = 0;
    std::vector<uint8_t> blob;
    auto player = std::make_unique<Player>(this);

    if (DbMgr::Instance().LoadPlayerByAccountUid(mAccountUid, uid, blob)) {
        if (!player->LoadFromBlob(uid, std::span<const uint8_t>(blob.data(), blob.size()))) {
            LOG_ERROR("玩家存档解析失败, accountUid: {}", mAccountUid);
            return false;
        }

        auto* oldSession = GameServices::Instance().GetSessionByPlayerUid(uid);
        if (oldSession && oldSession != this)
        {
            GameServices::Instance().KickSessionByPlayerUid(uid);
        }

        SetPlayer(std::move(player));
        SavePlayer();
    }

    return true;
}

bool GameSession::SavePlayer() {
    if (!mPlayer) {
        return false;
    }

	return mPlayer->Save();
}

void GameSession::SetPlayer(std::unique_ptr<Player> player) {
    ClearNextPackages();

    if (!player) {
        mPlayer.reset();
        return;
    }

    player->SetSessionRef(this);
    mPlayer = std::move(player);
    mPlayer->OnLogin();
}

Player* GameSession::GetPlayer() const {
    return mPlayer.get();
}

bool GameSession::HasPlayer() const {
    return mPlayer != nullptr;
}

void GameSession::ClearNextPackages() {
    while (!mPushList.empty()) {
        mPushList.pop();
    }
}

std::string GameSession::EncodeMessage(short msgId, const std::string& data) {
    std::string message;
    message.reserve(2 + data.size());
    message.push_back(static_cast<char>((msgId >> 8) & 0xFF));
    message.push_back(static_cast<char>(msgId & 0xFF));
    message.append(data);
    return message;
}

std::string GameSession::BuildMessage(short msgId, google::protobuf::Message* payload) {
    std::string result;
    if (HasNextPackages()) {
        if (!payload) {
            auto* nilPayload = new Nil();
            AddPacketListToMe(nilPayload);
            result = EncodeMessage(msgId, nilPayload->SerializeAsString());
            delete nilPayload;
        }
        else {
            AddPacketListToMe(payload);
            result = EncodeMessage(msgId, payload->SerializeAsString());
        }
    }
    else {
        if (payload) {
            result = EncodeMessage(msgId, payload->SerializeAsString());
        }
        else {
            result = EncodeMessage(msgId, "");
        }
    }

    return result;
}

bool HasNextPackageField(const google::protobuf::Message* message) {
    if (!message) return false;

    const auto* descriptor = message->GetDescriptor();
    const auto* field = descriptor->FindFieldByNumber(2047);
    if (!field) {
        field = descriptor->FindFieldByName("NextPackage");
    }
    if (!field) {
        field = descriptor->FindFieldByName("nextPackage");
    }

    return field != nullptr && field->cpp_type() == google::protobuf::FieldDescriptor::CPPTYPE_STRING;
}

void SetNextPackage(google::protobuf::Message* message, const std::string& data) {
    if (!message) {
        return;
    }

    const auto* descriptor = message->GetDescriptor();
    const auto* field = descriptor->FindFieldByNumber(2047);
    if (!field) {
        field = descriptor->FindFieldByName("NextPackage");
    }
    if (!field) {
        field = descriptor->FindFieldByName("nextPackage");
    }
    if (!field || field->cpp_type() != google::protobuf::FieldDescriptor::CPPTYPE_STRING) {
        return;
    }

    auto* reflection = message->GetReflection();

    reflection->SetString(message, field, data);
}

void GameSession::AddPacketListToMe(google::protobuf::Message* payload) {
    if (!payload || !HasNextPackageField(payload) || !HasNextPackages()) {
        return;
    }

    std::pair<short, std::unique_ptr<google::protobuf::Message>> curPacket;
    bool hasCurPacket = false;

    while (HasNextPackages()) {
        auto nextPacket = std::move(mPushList.top());
        mPushList.pop();

        Nil* nextNilPayload = nullptr;
        google::protobuf::Message* nextPayload = nextPacket.second.get();
        if (!nextPayload) {
            nextNilPayload = new Nil();
            nextPayload = nextNilPayload;
        }

        if (!hasCurPacket) {
            curPacket = std::move(nextPacket);
            hasCurPacket = true;
            if (nextNilPayload) {
                delete nextNilPayload;
            }
            continue;
        }

        Nil* curNilPayload = nullptr;
        google::protobuf::Message* curPayload = curPacket.second.get();
        if (!curPayload) {
            curNilPayload = new Nil();
            curPayload = curNilPayload;
        }

        if (!HasNextPackageField(curPayload)) {
            if (curNilPayload) {
                delete curNilPayload;
            }
            if (nextNilPayload) {
                delete nextNilPayload;
            }
            mPushList.push(std::move(nextPacket));
            break;
        }

        SetNextPackage(nextPayload, EncodeMessage(curPacket.first, curPayload->SerializeAsString()));

        if (curNilPayload) {
            delete curNilPayload;
        }
        if (nextNilPayload) {
            delete nextNilPayload;
        }

        curPacket = std::move(nextPacket);
    }

    if (hasCurPacket) {
        Nil* curNilPayload = nullptr;
        google::protobuf::Message* curPayload = curPacket.second.get();
        if (!curPayload) {
            curNilPayload = new Nil();
            curPayload = curNilPayload;
        }

        SetNextPackage(payload, EncodeMessage(curPacket.first, curPayload->SerializeAsString()));

        if (curNilPayload) {
            delete curNilPayload;
        }
    }
}

void GameSession::PushNextPackageImpl(short msgId, std::unique_ptr<google::protobuf::Message> payload) {
    mPushList.emplace(msgId, std::move(payload));

    //__debugbreak();
}

bool GameSession::HasNextPackages() {
    return !mPushList.empty();

    //__debugbreak();
}

std::string EncodeReply(GameSession* session, short msgId, google::protobuf::Message* payload) {
    if (session) {
        return session->BuildMessage(msgId, payload);
    }

    return GameSession::EncodeMessage(msgId, payload ? payload->SerializeAsString() : "");
}
