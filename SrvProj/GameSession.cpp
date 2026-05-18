#include "GameSession.h"
#include "AeadTool.h"
#include "Util.h"
#include <random>
#include <iomanip>
#include <sstream>
#include <openssl/rand.h>
#include "Logger.h"

void GameSession::GenerateServerKey() {
    auto EcdhPair = AeadTool::GetECDHKeyPair();

    if (EcdhPair.first.empty() || EcdhPair.second.empty()) {
        throw std::runtime_error("无法生成 ECDH 密钥对");
    }

    serverPrivateKey = EcdhPair.second;
    serverPublicKey = EcdhPair.first;
    //__debugbreak();
}

void GameSession::CalKey() {
    //std::string sharedKey = AeadTool::CalECDHSharedKey(
    //    serverPrivateKey,   // 自己的私钥
    //    clientPublicKey     // 对方的公钥
    //);

    //std::string info = AeadTool::CalInfo(
    //    clientPublicKey,    // 客户端公钥（第一个参数）
    //    serverPublicKey     // 服务端公钥（第二个参数）
    //);

    //key = AeadTool::CalSecretX(
    //    serverPublicKey,    // salt = 自己的公钥
    //    info,
    //    sharedKey
    //);


    auto sharedKey = AeadTool::CalECDHSharedKey(serverPrivateKey, clientPublicKey);
    //auto info = AeadTool::CalInfo(serverPunlicKey, clientPunlicKey);
    auto info = AeadTool::CalInfo(clientPublicKey, serverPublicKey);
    auto secretX = AeadTool::CalSecretX(serverPublicKey, info, sharedKey);
	LOG_DEBUG("clientPunlicKey: {}", Base64Encode(clientPublicKey));
    LOG_DEBUG("serverPublicKey: {}", Base64Encode(serverPublicKey));
    LOG_DEBUG("serverPrivateKey: {}", Base64Encode(serverPrivateKey));
    LOG_DEBUG("sharedKey: {}", Base64Encode(sharedKey));
    LOG_DEBUG("secretX: {}", Base64Encode(secretX));
    key = secretX;

    std::random_device rd;
    std::minstd_rand0 gen(rd());
    std::uniform_int_distribution<int> dis(0, 1);
    encryptFunction = dis(gen);  // 0 = AES-GCM, 1 = ChaCha20-Poly1305
}

std::string GameSession::BuildMessage(short msgId, const std::string& payload) {
    std::string message;
    message.reserve(2 + payload.size());
    message.push_back(static_cast<char>((msgId >> 8) & 0xFF));
    message.push_back(static_cast<char>(msgId & 0xFF));
    message.append(payload);
    return message;
}

std::string GameSession::GenerateToken() const {
    std::stringstream ss;
    ss << std::time(nullptr) << ":";

    std::vector<unsigned char> random(64);
    if (RAND_bytes(random.data(), random.size()) != 1)
    {
        throw std::runtime_error("RAND_bytes failed");
    }

    ss.write(reinterpret_cast<const char*>(random.data()), random.size());
    std::string temp = ss.str();

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen = 0;

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx)
        throw std::runtime_error("EVP_MD_CTX_new failed");

    if (EVP_DigestInit_ex(ctx, EVP_sha512(), nullptr) != 1 || EVP_DigestUpdate(ctx, temp.data(), temp.size()) != 1 || EVP_DigestFinal_ex(ctx, hash, &hashLen) != 1)
    {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("SHA-512 failed");
    }

    EVP_MD_CTX_free(ctx);

    return Base64Encode(std::string_view(reinterpret_cast<const char*>(hash), hashLen));

    //unsigned char buf[16];
    //RAND_bytes(buf, sizeof(buf));
    //std::ostringstream oss;
    //oss << std::hex << std::setfill('0');

    //for (int i = 0; i < sizeof(buf); ++i) {
    //    oss << std::setw(2) << (int)buf[i];
    //}

    //return oss.str();
}


bool GameSession::Login(std::string loginToken) {
    return true;
}

