#include "AeadTool.h"
#include <sstream>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/params.h>
#include <openssl/core_names.h>
#include <openssl/kdf.h>
#include <stdexcept>
#include "logger.h"

// 对称加密部分

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

void AeadTool::InitAeadTool()
{
    static bool initialized = false;
    if (initialized)
        return;

    initialized = true;

    NonceSize = 12;
    MacSize = 128;
    KeySize = 32;
    IVSize = 16;

    associatedData = std::string(12, '\0');

    function = 1;

    Encrypt = &AeadTool::Encrypt_BouncyCastle;
    Decrypt = &AeadTool::Dencrypt_BouncyCastle;
}

// 对称Aead加密部分

void AeadTool::Encrypt_BouncyCastle(std::string& result, const std::string& key, const std::string& nonce, const std::string& data, int dataLen, bool needAssociatedData)
{
    try
    {
        static bool inited = false;
        if (!inited)
        {
            InitAeadTool();
            inited = true;
        }

        std::string associated;

        if (needAssociatedData)
        {
		    // iv 作为 associated data（IDA里是这么做的）
            associated = nonce;
        }

        if (function == 0)
        {
            Encrypt_BouncyCastle_AesGcm(key, nonce, data, dataLen, associated, result);
        }
        else
        {
            Encrypt_BouncyCastle_ChaCha20Poly1305(key, nonce, data, dataLen, associated, result);
        }
    }
    catch (const std::exception& e)
    {
        std::ostringstream errMsg;
        errMsg << "AeadTool: 发生错误: " << e.what();
        LOG_ERROR(errMsg.str());
        std::throw_with_nested(std::runtime_error(errMsg.str()));;
        return;
    }
    catch (...)
    {
        std::ostringstream errMsg;
        errMsg << "AeadTool: 未知错误: ";
        std::throw_with_nested(std::runtime_error(errMsg.str()));;
    }
}

bool AeadTool::Dencrypt_BouncyCastle(std::string& result, const std::string& key, const std::string& nonce, const std::string& data, int dataLen, bool needAssociatedData)
{
    try
    {
        static bool inited = false;
        if (!inited)
        {
            InitAeadTool();
            inited = true;
        }

        std::string associated;
        if (needAssociatedData)
            associated = nonce;

        if (function != 0)
        {
            Decrypt_BouncyCastle_ChaCha20Poly1305(key, nonce, data, dataLen, associated, result);
        }
        else
        {
            Decrypt_BouncyCastle_AesGcm(key, nonce, data, dataLen, associated, result);
        }

        return true;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Aead 发生错误: {}", e.what());
        return false;
    }
    catch (...)
    {
        LOG_ERROR("未知错误");
        return false;
    }
}

// AES GCM 加密
void AeadTool::Encrypt_BouncyCastle_AesGcm(const std::string& key, const std::string& nonce, const std::string& secretMessage, int dataLen, std::string& associated, std::string& result)
{
    if (key.size() != KeySize)
    {
        throw std::invalid_argument("Invalid key size");
    }

    if (secretMessage.empty() || dataLen <= 0 || secretMessage.size() < (size_t)dataLen)
    {
        throw std::invalid_argument("Invalid secretMessage");
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    if (!ctx)
    {
        throw std::runtime_error("EVP_CIPHER_CTX_new failed");
    }

    const EVP_CIPHER* cipher = EVP_aes_256_gcm();

    if (EVP_EncryptInit_ex(ctx, cipher, nullptr, nullptr, nullptr) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptInit failed");
    }

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, (int)nonce.size(), nullptr) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("IV length set failed");
    }

    if (EVP_EncryptInit_ex(ctx, nullptr, nullptr, reinterpret_cast<const unsigned char*>(key.data()), reinterpret_cast<const unsigned char*>(nonce.data())) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Key/Nonce init failed");
    }

    int len = 0;

    if (!associated.empty())
    {
        if (EVP_EncryptUpdate(ctx, nullptr, &len, reinterpret_cast<const unsigned char*>(associated.data()), static_cast<int>(associated.size())) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("AAD failed");
        }
    }

    result.resize(dataLen + 16);

    if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(result.data()), &len, reinterpret_cast<const unsigned char*>(secretMessage.data()), dataLen) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptUpdate failed");
    }

    int cipherLen = len;

    if (EVP_EncryptFinal_ex(ctx, nullptr, &len) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptFinal failed");
    }

    unsigned char tag[16];

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("GetTag failed");
    }

    result.resize(cipherLen);

    result.append(reinterpret_cast<char*>(tag), 16);

    EVP_CIPHER_CTX_free(ctx);
}

// ChaCha20Poly1305 加密
void AeadTool::Encrypt_BouncyCastle_ChaCha20Poly1305(const std::string& key, const std::string& nonce, const std::string& secretMessage, int dataLen, std::string& associated, std::string& result)
{
    try
    {
        if (!key.data() || key.size() != 32)
        {
            throw std::runtime_error("Invalid ChaCha20Poly1305 key size");
        }

        if (!nonce.data() || nonce.size() != 12)
        {
            throw std::runtime_error("Invalid ChaCha20Poly1305 nonce size");
        }

        if (!secretMessage.data() || dataLen <= 0 || secretMessage.size() < static_cast<size_t>(dataLen))
        {
            throw std::runtime_error("Invalid plaintext");
        }

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx)
        {
            throw std::runtime_error("EVP_CIPHER_CTX_new failed");
        }

        const EVP_CIPHER* cipher = EVP_chacha20_poly1305();
        if (!cipher)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("EVP_chacha20_poly1305 not available");
        }

        if (EVP_EncryptInit_ex(ctx, cipher, nullptr, nullptr, nullptr) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("EncryptInit failed");
        }

        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, 12, nullptr) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Set IV length failed");
        }

        if (EVP_EncryptInit_ex(ctx, nullptr, nullptr, reinterpret_cast<const unsigned char*>(key.data()), reinterpret_cast<const unsigned char*>(nonce.data())) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Key/Nonce set failed");
        }

        if (!associated.empty())
        {
            int outLen = 0;
            if (EVP_EncryptUpdate(ctx, nullptr, &outLen, reinterpret_cast<const unsigned char*>(associated.data()), static_cast<int>(associated.size())) != 1)
            {
                EVP_CIPHER_CTX_free(ctx);
                throw std::runtime_error("AAD update failed");
            }
        }

        std::string out;
        out.resize(static_cast<size_t>(dataLen) + 16);

        int len = 0;
        int totalLen = 0;

        if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(out.data()), &len, reinterpret_cast<const unsigned char*>(secretMessage.data()), dataLen) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("EncryptUpdate failed");
        }

        totalLen += len;

        if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(out.data()) + totalLen, &len) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("EncryptFinal failed");
        }

        totalLen += len;

        unsigned char tag[16];
        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, 16, tag) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Get tag failed");
        }

        EVP_CIPHER_CTX_free(ctx);

        out.resize(static_cast<size_t>(totalLen));
        out.append(reinterpret_cast<char*>(tag), 16);

        result = out;
    }
    catch (...)
    {
        result.clear();
        return;
    }
}

// AES GCM 解密
void AeadTool::Decrypt_BouncyCastle_AesGcm(const std::string& key, const std::string& nonce, const std::string& cipherText, int dataLen, std::string& associated, std::string& result)
{
    try
    {
        if (!key.data() || key.size() != 32)
        {
            throw std::runtime_error("Invalid key size");
        }

        if (!nonce.data() || nonce.size() != 12)
        {
            throw std::runtime_error("Invalid nonce size");
        }

        if (!cipherText.data() || dataLen <= 16)
        {
            throw std::runtime_error("Invalid cipherText");
        }

        static_cast<void>(0);

        const int tagLen = 16;
        const int encLen = dataLen - tagLen;

        const unsigned char* tag = reinterpret_cast<const unsigned char*>(cipherText.data() + encLen);

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx)
        {
            throw std::runtime_error("ctx alloc failed");
        }

        const EVP_CIPHER* cipher = EVP_aes_256_gcm();
        if (!cipher)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("AES-GCM not available");
        }

        if (EVP_DecryptInit_ex(ctx, cipher, nullptr, nullptr, nullptr) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("DecryptInit failed");
        }

        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, 12, nullptr) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Set IV failed");
        }

        if (EVP_DecryptInit_ex(ctx, nullptr, nullptr,
            reinterpret_cast<const unsigned char*>(key.data()),
            reinterpret_cast<const unsigned char*>(nonce.data())) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Key/Nonce set failed");
        }

        if (!associated.empty())
        {
            int outLen = 0;
            if (EVP_DecryptUpdate(ctx, nullptr, &outLen, reinterpret_cast<const unsigned char*>(associated.data()), static_cast<int>(associated.size())) != 1)
            {
                EVP_CIPHER_CTX_free(ctx);
                throw std::runtime_error("AAD failed");
            }
        }

        std::string out;
        out.resize(static_cast<size_t>(encLen));

        int len = 0;
        int totalLen = 0;

        if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(out.data()), &len, reinterpret_cast<const unsigned char*>(cipherText.data()), encLen) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("DecryptUpdate failed");
        }

        totalLen += len;

        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, tagLen, (void*)tag) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Set tag failed");
        }

        int finalLen = 0;

        if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(out.data()) + totalLen, &finalLen) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("DecryptFinal failed (auth failed)");
        }

        totalLen += finalLen;

        EVP_CIPHER_CTX_free(ctx);

        out.resize(static_cast<size_t>(totalLen));

        result = out;
    }
    catch (...)
    {
        result.clear();
        return;
    }
}

// ChaCha20Poly1305 解密
void AeadTool::Decrypt_BouncyCastle_ChaCha20Poly1305(const std::string& key, const std::string& nonce, const std::string& cipherText, int dataLen, std::string& associated, std::string& result)
{
    try
    {
        if (!key.data() || key.size() != 32)
        {
            throw std::runtime_error("Invalid key");
        }

        if (!nonce.data() || nonce.size() != 12)
        {
            throw std::runtime_error("Invalid nonce");
        }

        if (!cipherText.data() || dataLen <= 16)
        {
            throw std::runtime_error("Invalid cipherText");
        }

        const int tagLen = 16;
        const int encLen = dataLen - tagLen;

        const unsigned char* tag = reinterpret_cast<const unsigned char*>(cipherText.data() + encLen);

        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        if (!ctx)
        {
            throw std::runtime_error("ctx alloc failed");
        }

        const EVP_CIPHER* cipher = EVP_chacha20_poly1305();
        if (!cipher)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("chacha20-poly1305 not available");
        }

        if (EVP_DecryptInit_ex(ctx, cipher, nullptr, nullptr, nullptr) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("init failed");
        }

        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, 12, nullptr) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("ivlen failed");
        }

        if (EVP_DecryptInit_ex(ctx, nullptr, nullptr,
            reinterpret_cast<const unsigned char*>(key.data()),
            reinterpret_cast<const unsigned char*>(nonce.data())) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("key/nonce failed");
        }

        if (!associated.empty())
        {
            int outLen = 0;
            if (EVP_DecryptUpdate(ctx, nullptr, &outLen, reinterpret_cast<const unsigned char*>(associated.data()), static_cast<int>(associated.size())) != 1)
            {
                EVP_CIPHER_CTX_free(ctx);
                throw std::runtime_error("aad failed");
            }
        }

        std::string out;
        out.resize(static_cast<size_t>(encLen));

        int len = 0;
        int totalLen = 0;

        if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(out.data()), &len, reinterpret_cast<const unsigned char*>(cipherText.data()), encLen) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("decrypt update failed");
        }

        totalLen += len;

        if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, tagLen, (void*)tag) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("set tag failed");
        }

        int finalLen = 0;

        if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(out.data()) + totalLen, &finalLen) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("auth failed");
        }

        totalLen += finalLen;

        EVP_CIPHER_CTX_free(ctx);

        out.resize(static_cast<size_t>(totalLen));

        result = out;
    }
    catch (...)
    {
        result.clear();
        return;
    }
}

// Ecdh 部分

// index0为公钥 index1为私钥
std::vector<std::vector<uint8_t>> AeadTool::GetECDHKeyPair()
{
    std::vector<std::vector<uint8_t>> result;

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
    if (!ctx)
    {
        return result;
    }

    if (EVP_PKEY_keygen_init(ctx) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        return result;
    }

    if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_X9_62_prime256v1) <= 0)
    {
        EVP_PKEY_CTX_free(ctx);
        return result;
    }

    EVP_PKEY* pkey = nullptr;
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0 || !pkey)
    {
        EVP_PKEY_CTX_free(ctx);
        return result;
    }

    EVP_PKEY_CTX_free(ctx);

    std::vector<uint8_t> priv(32);
    size_t privLen = priv.size();

    if (EVP_PKEY_get_raw_private_key(pkey, priv.data(), &privLen) <= 0)
    {
        EVP_PKEY_free(pkey);
        return result;
    }

    priv.resize(privLen);

    std::vector<uint8_t> pub(65);
    size_t pubLen = pub.size();

    if (EVP_PKEY_get_raw_public_key(pkey, pub.data(), &pubLen) <= 0)
    {
        EVP_PKEY_free(pkey);
        return result;
    }

    pub.resize(pubLen);

    EVP_PKEY_free(pkey);

    result.push_back(pub);
    result.push_back(priv);

    return result;
}

// std::string& clientPublic, std::string& serverPrivate
std::string AeadTool::CalECDHSharedKey(const std::string& clinetPrivate, const std::string& serverPublic)
{
    std::string result;

    EVP_PKEY* peerKey = nullptr;
    EVP_PKEY* privKey = nullptr;

    EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
    if (!pctx)
    {
        return result;
    }

    if (EVP_PKEY_fromdata_init(pctx) <= 0)
    {
        EVP_PKEY_CTX_free(pctx);
        return result;
    }

    {
        OSSL_PARAM params[3];
        const char* group = "prime256v1";

        params[0] = OSSL_PARAM_construct_utf8_string(OSSL_PKEY_PARAM_GROUP_NAME, (char*)group, 0);
        params[1] = OSSL_PARAM_construct_octet_string(OSSL_PKEY_PARAM_PRIV_KEY, const_cast<void*>(reinterpret_cast<const void*>(clinetPrivate.data())), clinetPrivate.size());
        params[2] = OSSL_PARAM_construct_end();

        if (EVP_PKEY_fromdata(pctx, &privKey, EVP_PKEY_KEYPAIR, params) <= 0)
        {
            EVP_PKEY_CTX_free(pctx);
            return result;
        }
    }

    {
        OSSL_PARAM params[3];
        const char* group = "prime256v1";

        params[0] = OSSL_PARAM_construct_utf8_string(OSSL_PKEY_PARAM_GROUP_NAME, (char*)group, 0);
        params[1] = OSSL_PARAM_construct_octet_string(OSSL_PKEY_PARAM_PUB_KEY, const_cast<void*>(reinterpret_cast<const void*>(serverPublic.data())), serverPublic.size());
        params[2] = OSSL_PARAM_construct_end();

        if (EVP_PKEY_fromdata(pctx, &peerKey, EVP_PKEY_PUBLIC_KEY, params) <= 0)
        {
            EVP_PKEY_free(privKey);
            EVP_PKEY_CTX_free(pctx);
            return result;
        }
    }

    EVP_PKEY_CTX_free(pctx);

    EVP_PKEY_CTX* dctx = EVP_PKEY_CTX_new(privKey, nullptr);
    if (!dctx)
    {
        EVP_PKEY_free(privKey);
        EVP_PKEY_free(peerKey);
        return result;
    }

    if (EVP_PKEY_derive_init(dctx) <= 0)
    {
        EVP_PKEY_free(privKey);
        EVP_PKEY_free(peerKey);
        EVP_PKEY_CTX_free(dctx);
        return result;
    }

    if (EVP_PKEY_derive_set_peer(dctx, peerKey) <= 0)
    {
        EVP_PKEY_free(privKey);
        EVP_PKEY_free(peerKey);
        EVP_PKEY_CTX_free(dctx);
        return result;
    }

    size_t secretLen = 0;

    if (EVP_PKEY_derive(dctx, nullptr, &secretLen) <= 0)
    {
        EVP_PKEY_free(privKey);
        EVP_PKEY_free(peerKey);
        EVP_PKEY_CTX_free(dctx);
        return result;
    }

    std::vector<uint8_t> secret(secretLen);

    if (EVP_PKEY_derive(dctx, secret.data(), &secretLen) <= 0)
    {
        EVP_PKEY_free(privKey);
        EVP_PKEY_free(peerKey);
        EVP_PKEY_CTX_free(dctx);
        return result;
    }

    result.assign((char*)secret.data(), secretLen);

    EVP_PKEY_free(privKey);
    EVP_PKEY_free(peerKey);
    EVP_PKEY_CTX_free(dctx);

    return result;
}

std::string AeadTool::CalInfo(const std::string& clientPublic, const std::string& serverPublic)
{
    if (clientPublic.empty())
    {
        return std::string();
    }

    if (serverPublic.empty())
    {
        return std::string();
    }

    std::string result;
    result.resize(clientPublic.size());

    const size_t clientSize = clientPublic.size();
    const size_t serverSize = serverPublic.size();

    for (size_t i = 0; i < clientSize; i++)
    {
        uint8_t c = static_cast<uint8_t>(clientPublic[i]);
        uint8_t s = static_cast<uint8_t>(serverPublic[i % serverSize]);

        if (c > s)
        {
            result[i] = static_cast<char>((static_cast<uint8_t>(2 * s)) ^ c);
        }
        else
        {
            result[i] = static_cast<char>((static_cast<uint8_t>(s >> 1)) ^ c);
        }
    }

    return result;
}

std::string AeadTool::CalSecretX(const std::string& serverPublic, const std::string& info, const std::string& sharedKey)
{
    EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL);
    if (!pctx)
    {
        return {};
    }

    if (EVP_PKEY_derive_init(pctx) <= 0)
    {
        EVP_PKEY_CTX_free(pctx);
        return {};
    }

    if (EVP_PKEY_CTX_set_hkdf_md(pctx, EVP_sha256()) <= 0)
    {
        EVP_PKEY_CTX_free(pctx);
        return {};
    }

    if (EVP_PKEY_CTX_set1_hkdf_key(pctx, reinterpret_cast<const unsigned char*>(sharedKey.data()), sharedKey.size()) <= 0)
    {
        EVP_PKEY_CTX_free(pctx);
        return {};
    }

    if (EVP_PKEY_CTX_set1_hkdf_salt(pctx, reinterpret_cast<const unsigned char*>(serverPublic.data()), serverPublic.size()) <= 0)
    {
        EVP_PKEY_CTX_free(pctx);
        return {};
    }

    if (EVP_PKEY_CTX_add1_hkdf_info(pctx, reinterpret_cast<const unsigned char*>(info.data()), info.size()) <= 0)
    {
        EVP_PKEY_CTX_free(pctx);
        return {};
    }

    std::string out;
    out.resize(32);

    size_t outLen = 32;
    if (EVP_PKEY_derive(pctx, reinterpret_cast<unsigned char*>(out.data()), &outLen) <= 0)
    {
        EVP_PKEY_CTX_free(pctx);
        return {};
    }

    EVP_PKEY_CTX_free(pctx);

    if (outLen != 32)
    {
        return {};
    }

    return out;
}


std::string CalSecretX2(const std::string& serverPublic, const std::string& info, const std::string& sharedKey)
{
    EVP_KDF* kdf = EVP_KDF_fetch(NULL, "HKDF", NULL);
    if (!kdf)
    {
        return {};
    }

    EVP_KDF_CTX* ctx = EVP_KDF_CTX_new(kdf);
    EVP_KDF_free(kdf);

    if (!ctx)
    {
        return {};
    }

    OSSL_PARAM params[5];
    size_t i = 0;

    params[i++] = OSSL_PARAM_construct_utf8_string(OSSL_KDF_PARAM_DIGEST, const_cast<char*>("SHA256"), 0);

    params[i++] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_KEY, const_cast<void*>(reinterpret_cast<const void*>(sharedKey.data())), sharedKey.size());

    params[i++] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_SALT, const_cast<void*>(reinterpret_cast<const void*>(serverPublic.data())), serverPublic.size());

    params[i++] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_INFO, const_cast<void*>(reinterpret_cast<const void*>(info.data())), info.size());

    params[i] = OSSL_PARAM_construct_end();

    std::string out;
    out.resize(32);

    if (EVP_KDF_derive(ctx, reinterpret_cast<unsigned char*>(out.data()), 32, params) <= 0) {
        EVP_KDF_CTX_free(ctx);
        return {};
    }

    EVP_KDF_CTX_free(ctx);

    return out;
}

void AeadTool::Encrypt_Static(std::string& result, const std::string& key, const std::string& nonce, const std::string& data, int dataLen, bool needAssociatedData, int function) {
    try
    {
        std::string associated;

        if (needAssociatedData)
        {
            // iv 作为 associated data（IDA里是这么做的）
            associated = nonce;
        }

        if (function == 0)
        {
            Encrypt_BouncyCastle_AesGcm(key, nonce, data, dataLen, associated, result);
        }
        else
        {
            Encrypt_BouncyCastle_ChaCha20Poly1305(key, nonce, data, dataLen, associated, result);
        }
    }
    catch (const std::exception& e)
    {
        std::ostringstream errMsg;
        errMsg << "AeadTool: 发生错误: " << e.what();
        LOG_ERROR(errMsg.str());
        std::throw_with_nested(std::runtime_error(errMsg.str()));;
        return;
    }
    catch (...)
    {
        std::ostringstream errMsg;
        errMsg << "AeadTool: 未知错误: ";
        std::throw_with_nested(std::runtime_error(errMsg.str()));;
    }
}

bool AeadTool::Decrypt_Static(std::string& result, const std::string& key, const std::string& nonce, const std::string& data, int dataLen, bool needAssociatedData, int function) {
    try
    {
        std::string associated;
        if (needAssociatedData)
            associated = nonce;

        if (function != 0)
        {
            Decrypt_BouncyCastle_ChaCha20Poly1305(key, nonce, data, dataLen, associated, result);
        }
        else
        {
            Decrypt_BouncyCastle_AesGcm(key, nonce, data, dataLen, associated, result);
        }

        return true;
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Aead 发生错误: {}", e.what());
        return false;
    }
    catch (...)
    {
        LOG_ERROR("未知错误");
        return false;
    }
}


// 数据弄脏和洗白部分

std::string AeadUtil::Obfuscate(const std::string& messageData, const std::string& key3) {
    if (messageData.empty() || key3.empty())
        return messageData;

    std::string result = messageData;
    uint8_t len = static_cast<uint8_t>(result.size());
    size_t keyLen = key3.size();

    for (size_t i = 0; i < result.size(); ++i) {
        uint8_t b = static_cast<uint8_t>(result[i]);
        b ^= static_cast<uint8_t>(key3[i % keyLen]);
        b = (b << 1) | (b >> 7);
        b ^= len;
        result[i] = static_cast<char>(b);
    }
    return result;
}

std::string AeadUtil::Wash(const std::string& messageData, const std::string& key3) {
    if (messageData.empty() || key3.empty())
        return messageData;

    std::string result = messageData;
    uint8_t len = static_cast<uint8_t>(result.size());
    size_t keyLen = key3.size();

    for (size_t i = 0; i < result.size(); ++i) {
        uint8_t b = static_cast<uint8_t>(result[i]);
        b ^= len;
        b = (b >> 1) | ((b & 1) << 7);
        b ^= static_cast<uint8_t>(key3[i % keyLen]);
        result[i] = static_cast<char>(b);
    }
    return result;
}
