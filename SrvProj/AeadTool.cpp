#include "AeadTool.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdexcept>

std::string AeadTool::EncryptAesCBCInfo(const char* key, const char* IV, const std::string& plainBytes) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("EVP_CIPHER_CTX_new failed");
    }

    std::string result;
    int outLen = plainBytes.size() + EVP_CIPHER_block_size(EVP_aes_128_cbc());
    result.resize(outLen);

    int len = 0;
    int ciphertext_len = 0;

    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key), reinterpret_cast<const unsigned char*>(IV)) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptInit failed");
    }

    if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(result.data()), &len, reinterpret_cast<const unsigned char*>(plainBytes.data()), static_cast<int>(plainBytes.size())) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptUpdate failed");
    }
    ciphertext_len = len;

    if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(result.data()) + len, &len) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptFinal failed");
    }

    result.resize(ciphertext_len + len, 0);
    EVP_CIPHER_CTX_free(ctx);

    return result;
}

std::string AeadTool::DecryptAesCBCInfo(const char* key, const char* IV, const std::string& cipherBytes) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("EVP_CIPHER_CTX_new failed");
    }

    std::string result;
    result.resize(cipherBytes.size(), 0);

    int len = 0;
    int plaintext_len = 0;

    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, reinterpret_cast<const unsigned char*>(key), reinterpret_cast<const unsigned char*>(IV)) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("DecryptInit failed");
    }

    if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(result.data()), &len, reinterpret_cast<const unsigned char*>(cipherBytes.data()), static_cast<int>(cipherBytes.size())) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("DecryptUpdate failed");
    }

    plaintext_len = len;

    if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(result.data()) + len, &len) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("DecryptFinal failed (bad padding or incorrect key/IV)");
    }

    result.resize(plaintext_len + len);

    EVP_CIPHER_CTX_free(ctx);

    return result;
}
