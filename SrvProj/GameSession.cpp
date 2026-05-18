#include "GameSession.h"
#include "AeadTool.h"
#include "Util.h"
#include <random>
#include <iomanip>
#include <sstream>
#include <openssl/rand.h>
#include "Logger.h"
#include <chrono>
#include <array>

bool GameSession::GenerateServerKey() {
    try {
        auto EcdhPair = AeadTool::GetECDHKeyPair();

        if (EcdhPair.first.empty() || EcdhPair.second.empty()) {
            LOG_ERROR("生成的 ECDH 密钥对为空");
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
    mEncryptFunction = dis(gen);  // 0 = AES-GCM, 1 = ChaCha20-Poly1305

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
    mPlayer = std::make_unique<Player>();

	mPlayer->Init();

	return true;
}
