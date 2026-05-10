#include "GameSession.h"
#include "AeadTool.h"
#include "Util.h"
#include <random>
#include <iomanip>
#include <sstream>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/buffer.h>

void GameSession::GenerateServerKey() {
    auto EcdhPair = AeadTool::GetECDHKeyPair();

    serverPublicKey = EcdhPair[1];
    serverPublicKey = EcdhPair[0];
}

void GameSession::CalKey() {
    std::string sharedKey = AeadTool::CalECDHSharedKey(
        ByteVecToString(serverPrivateKey),   // 自己的私钥
        ByteVecToString(clientPublicKey)     // 对方的公钥
    );

    std::string info = AeadTool::CalInfo(
        ByteVecToString(clientPublicKey),    // 客户端公钥（第一个参数）
        ByteVecToString(serverPublicKey)     // 服务端公钥（第二个参数）
    );

    std::string key = AeadTool::CalSecretX(
        ByteVecToString(serverPublicKey),    // salt = 自己的公钥
        info,
        sharedKey
    );

    std::random_device rd;
    std::minstd_rand0 gen(rd);
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

std::string Base64Encode(const unsigned char* input, int length)
{
    BIO* bio = BIO_new(BIO_f_base64());
    BIO* mem = BIO_new(BIO_s_mem());
    bio = BIO_push(bio, mem);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, input, length);
    BIO_flush(bio);

    BUF_MEM* bufferPtr;
    BIO_get_mem_ptr(bio, &bufferPtr);

    std::string result(bufferPtr->data, bufferPtr->length);

    BIO_free_all(bio);
    return result;
}


std::string GameSession::GenerateToken() {
    // 1️⃣ 时间戳
    std::stringstream ss;
    ss << std::time(nullptr) << ":";

    // 2️⃣ 64字节随机数
    std::vector<unsigned char> random(64);
    if (RAND_bytes(random.data(), random.size()) != 1)
    {
        throw std::runtime_error("RAND_bytes failed");
    }

    ss.write(reinterpret_cast<const char*>(random.data()), random.size());
    std::string temp = ss.str();

    // 3️⃣ SHA-512
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

    return Base64Encode(hash, hashLen);

    //unsigned char buf[16];
    //RAND_bytes(buf, sizeof(buf));
    //std::ostringstream oss;
    //oss << std::hex << std::setfill('0');

    //for (int i = 0; i < sizeof(buf); ++i) {
    //    oss << std::setw(2) << (int)buf[i];
    //}

    //return oss.str();
}