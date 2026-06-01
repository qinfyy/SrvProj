#include "GameSession.h"

#include "AeadTool.h"
#include "DbMgr.h"
#include "Logger.h"
#include "Util.h"

#include <array>
#include <chrono>
#include <functional>
#include <iomanip>
#include <openssl/rand.h>
#include <random>
#include <sstream>
#include <vector>

#include "./Game/ActivityMgr.h"
#include "./Game/CharacterMgr.h"
#include "./Game/InventoryMgr.h"
#include "./Game/QuestMgr.h"
#include "./Game/InventoryMgr.h"

bool GameSession::GenerateServerKey() {
    try {
        auto EcdhPair = AeadTool::GetECDHKeyPair();

        if (EcdhPair.first.empty() || EcdhPair.second.empty()) {
            // LOG_ERROR("生成的 ECDH 密钥对为空");
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

std::string GameSession::BuildMessage(short msgId, const std::string& payload) {
    std::string message;
    message.reserve(2 + payload.size());
    message.push_back(static_cast<char>((msgId >> 8) & 0xFF));
    message.push_back(static_cast<char>(msgId & 0xFF));
    message.append(payload);
    return message;
}

std::string GameSession::GenerateToken() {
    std::array<uint8_t, 16> buf;
    if (RAND_bytes(buf.data(), buf.size()) != 1) {
        throw std::runtime_error("RAND_bytes 失败");
    }

    return ToHex(buf);
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
    auto player = std::make_unique<Player>();

    if (DbMgr::Instance().LoadPlayerByAccountUid(mAccountUid, uid, blob)) {
        if (!player->LoadFromBlob(uid, std::span<const uint8_t>(blob.data(), blob.size()))) {
            LOG_ERROR("玩家存档解析失败, accountUid: {}", mAccountUid);
            return false;
        }
    }
    else {
        uid = static_cast<uint32_t>(std::hash<std::string>{}(mAccountUid) & 0x7FFFFFFF);
        if (uid == 0) {
            uid = 1;
        }

        if (!player->InitNewPlayer(uid, "me", false)) {
            return false;
        }

        auto saveData = player->SaveToBlob();
        if (!DbMgr::Instance().CreatePlayer(uid, mAccountUid, std::span<const uint8_t>(saveData.data(), saveData.size()))) {
            LOG_ERROR("创建玩家存档失败, uid: {}, accountUid: {}", uid, mAccountUid);
            return false;
        }
    }

    player->OnLogin();
    mPlayer = std::move(player);
    SavePlayer();
    return true;
}

bool GameSession::SavePlayer() {
    if (!mPlayer) {
        return false;
    }

    auto saveData = mPlayer->SaveToBlob();
    return DbMgr::Instance().SavePlayer(mPlayer->GetUid(), std::span<const uint8_t>(saveData.data(), saveData.size()));
}
