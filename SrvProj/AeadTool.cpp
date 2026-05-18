#include "AeadTool.h"
#include <sstream>
#include <stdexcept>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/params.h>
#include <openssl/core_names.h>
#include <openssl/kdf.h>
#include <openssl/bn.h>

// 对称加密部分

// AES CBC 加密
std::string AeadTool::EncryptAesCBCInfo(std::string_view key, std::string_view IV, std::string_view plainBytes)
{
    if (key.size() != 16) {
        throw std::runtime_error("密钥长度不对");
    }

    if (IV.size() != IVSize) {
        throw std::runtime_error("IV 长度不对");
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("EVP_CIPHER_CTX_new 失败");
    }

    std::string result;
    int outLen = plainBytes.size() + EVP_CIPHER_block_size(EVP_aes_128_cbc());
    result.resize(outLen);

    int len = 0;
    int ciphertext_len = 0;

    if (EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(key.data())), const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(IV.data()))) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptInit 失败");
    }

    if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(result.data()), &len, reinterpret_cast<const unsigned char*>(plainBytes.data()), static_cast<int>(plainBytes.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptUpdate 失败");
    }
    ciphertext_len = len;

    if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(result.data()) + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptFinal 失败");
    }

    result.resize(ciphertext_len + len);
    EVP_CIPHER_CTX_free(ctx);

    return result;
}

// AES CBC 解密
std::string AeadTool::DecryptAesCBCInfo(std::string_view key, std::string_view IV, std::string_view cipherBytes)
{
    if (key.size() != 16) {
        throw std::runtime_error("密钥长度不对");
    }

    if (IV.size() != IVSize) {
        throw std::runtime_error("IV 长度不对");
    }

    if (cipherBytes.empty() || cipherBytes.size() % 16 != 0) {
        throw std::runtime_error("密文长度不对");
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("EVP_CIPHER_CTX_new 失败");
    }

    std::string result;
    result.resize(cipherBytes.size(), 0);

    int len = 0;
    int plaintext_len = 0;

    if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr,
        const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(key.data())), const_cast<unsigned char*>(reinterpret_cast<const unsigned char*>(IV.data()))) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("DecryptInit 失败");
    }

    if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(result.data()), &len, reinterpret_cast<const unsigned char*>(cipherBytes.data()), static_cast<int>(cipherBytes.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("DecryptUpdate 失败");
    }

    plaintext_len = len;

    if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(result.data()) + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("DecryptFinal 失败 (错误的填充方式或不正确的 key/IV)");
    }

    result.resize(plaintext_len + len);
    EVP_CIPHER_CTX_free(ctx);

    return result;
}

// 对称认证加密部分

// AES GCM 加密
void AeadTool::Encrypt_BouncyCastle_AesGcm(std::string_view key, std::string_view nonce, std::string_view secretMessage, int dataLen, std::string_view associated, std::string& result)
{
    if (key.size() != KeySize)
    {
        throw std::invalid_argument("无效的 AES GCM 密钥大小");
    }

    if (!nonce.data() || nonce.size() != NonceSize)
    {
        throw std::runtime_error("无效的 AES GCM nonce 大小");
    }

    if (secretMessage.empty() || dataLen <= 0 || secretMessage.size() < static_cast<size_t>(dataLen))
    {
        throw std::invalid_argument("要加密的数据是空的");
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

    if (!ctx)
    {
        throw std::runtime_error("EVP_CIPHER_CTX_new 失败");
    }

    const EVP_CIPHER* cipher = EVP_aes_256_gcm();

    if (EVP_EncryptInit_ex(ctx, cipher, nullptr, nullptr, nullptr) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EVP_EncryptInit_ex 失败");
    }

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, (int)nonce.size(), nullptr) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("IV 长度设置失败");
    }

    if (EVP_EncryptInit_ex(ctx, nullptr, nullptr, reinterpret_cast<const unsigned char*>(key.data()), reinterpret_cast<const unsigned char*>(nonce.data())) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Key/Nonce 初始化失败");
    }

    int len = 0;

    if (!associated.empty())
    {
        if (EVP_EncryptUpdate(ctx, nullptr, &len, reinterpret_cast<const unsigned char*>(associated.data()), static_cast<int>(associated.size())) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("AAD 更新失败");
        }
    }

    result.resize(dataLen + 16);

    if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(result.data()), &len, reinterpret_cast<const unsigned char*>(secretMessage.data()), dataLen) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptUpdate 失败");
    }

    int cipherLen = len;

    if (EVP_EncryptFinal_ex(ctx, nullptr, &len) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptFinal 失败");
    }

    unsigned char tag[16];

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("获取 Tag 失败");
    }

    result.resize(cipherLen);

    result.append(reinterpret_cast<char*>(tag), 16);

    EVP_CIPHER_CTX_free(ctx);
}

// ChaCha20Poly1305 加密
void AeadTool::Encrypt_BouncyCastle_ChaCha20Poly1305(std::string_view key, std::string_view nonce, std::string_view secretMessage, int dataLen, std::string_view associated, std::string& result)
{
    if (!key.data() || key.size() != KeySize)
    {
        throw std::runtime_error("无效的 ChaCha20Poly1305 密钥大小");
    }

    if (!nonce.data() || nonce.size() != NonceSize)
    {
        throw std::runtime_error("无效的 ChaCha20Poly1305 nonce 大小");
    }

    if (!secretMessage.data() || dataLen <= 0 || secretMessage.size() < static_cast<size_t>(dataLen))
    {
        throw std::runtime_error("要加密的数据是空的");
    }

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        throw std::runtime_error("EVP_CIPHER_CTX_new 失败");
    }

    const EVP_CIPHER* cipher = EVP_chacha20_poly1305();
    if (!cipher)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EVP_chacha20_poly1305 不可用");
    }

    if (EVP_EncryptInit_ex(ctx, cipher, nullptr, nullptr, nullptr) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptInit 失败");
    }

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, 12, nullptr) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("IV 长度设置失败");
    }

    if (EVP_EncryptInit_ex(ctx, nullptr, nullptr, reinterpret_cast<const unsigned char*>(key.data()), reinterpret_cast<const unsigned char*>(nonce.data())) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Key/Nonce 设置失败");
    }

    if (!associated.empty())
    {
        int outLen = 0;
        if (EVP_EncryptUpdate(ctx, nullptr, &outLen, reinterpret_cast<const unsigned char*>(associated.data()), static_cast<int>(associated.size())) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("AAD 更新失败");
        }
    }

    std::string out;
    out.resize(static_cast<size_t>(dataLen) + 16);

    int len = 0;
    int totalLen = 0;

    if (EVP_EncryptUpdate(ctx, reinterpret_cast<unsigned char*>(out.data()), &len, reinterpret_cast<const unsigned char*>(secretMessage.data()), dataLen) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptUpdate 失败");
    }

    totalLen += len;

    if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(out.data()) + totalLen, &len) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("EncryptFinal 失败");
    }

    totalLen += len;

    unsigned char tag[16];
    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_GET_TAG, 16, tag) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("获取 tag 失败");
    }

    EVP_CIPHER_CTX_free(ctx);

    out.resize(static_cast<size_t>(totalLen));
    out.append(reinterpret_cast<char*>(tag), 16);

    result = out;
}

// AES GCM 解密
void AeadTool::Decrypt_BouncyCastle_AesGcm(std::string_view key, std::string_view nonce, std::string_view cipherText, int dataLen, std::string_view associated, std::string& result)
{
    if (!key.data() || key.size() != KeySize)
    {
        throw std::runtime_error("无效的 AES GCM 密钥大小");
    }

    if (!nonce.data() || nonce.size() != NonceSize)
    {
        throw std::runtime_error("无效的 AES GCM nonce 大小");
    }

    if (!cipherText.data() || dataLen <= 16)
    {
        throw std::runtime_error("密文长度不对");
    }

    static_cast<void>(0);

    const int tagLen = 16;
    const int encLen = dataLen - tagLen;

    const unsigned char* tag = reinterpret_cast<const unsigned char*>(cipherText.data() + encLen);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        throw std::runtime_error("EVP_CIPHER_CTX_new 失败");
    }

    const EVP_CIPHER* cipher = EVP_aes_256_gcm();
    if (!cipher)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("AES-GCM 不可用");
    }

    if (EVP_DecryptInit_ex(ctx, cipher, nullptr, nullptr, nullptr) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("DecryptInit 失败");
    }

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, 12, nullptr) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("设置 IV 长度失败");
    }

    if (EVP_DecryptInit_ex(ctx, nullptr, nullptr, reinterpret_cast<const unsigned char*>(key.data()), reinterpret_cast<const unsigned char*>(nonce.data())) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Key/Nonce 设置失败");
    }

    if (!associated.empty())
    {
        int outLen = 0;
        if (EVP_DecryptUpdate(ctx, nullptr, &outLen, reinterpret_cast<const unsigned char*>(associated.data()), static_cast<int>(associated.size())) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("AAD 更新失败");
        }
    }

    std::string out;
    out.resize(static_cast<size_t>(encLen));

    int len = 0;
    int totalLen = 0;

    if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(out.data()), &len, reinterpret_cast<const unsigned char*>(cipherText.data()), encLen) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("DecryptUpdate 失败");
    }

    totalLen += len;

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, tagLen, (void*)tag) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("设置 tag 失败");
    }

    int finalLen = 0;

    if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(out.data()) + totalLen, &finalLen) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("DecryptFinal 失败（认证失败）");
    }

    totalLen += finalLen;

    EVP_CIPHER_CTX_free(ctx);

    out.resize(static_cast<size_t>(totalLen));

    result = out;
}

// ChaCha20Poly1305 解密
void AeadTool::Decrypt_BouncyCastle_ChaCha20Poly1305(std::string_view key, std::string_view nonce, std::string_view cipherText, int dataLen, std::string_view associated, std::string& result)
{
    if (!key.data() || key.size() != KeySize)
    {
        throw std::runtime_error("无效的 ChaCha20Poly1305 密钥大小");
    }

    if (!nonce.data() || nonce.size() != NonceSize)
    {
        throw std::runtime_error("无效的 ChaCha20Poly1305 nonce 大小");
    }

    if (!cipherText.data() || dataLen <= 16)
    {
        throw std::runtime_error("密文 长度不对");
    }

    const int tagLen = 16;
    const int encLen = dataLen - tagLen;

    const unsigned char* tag = reinterpret_cast<const unsigned char*>(cipherText.data() + encLen);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        throw std::runtime_error("EVP_CIPHER_CTX_new 失败");
    }

    const EVP_CIPHER* cipher = EVP_chacha20_poly1305();
    if (!cipher)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("chacha20-poly1305 不可用");
    }

    if (EVP_DecryptInit_ex(ctx, cipher, nullptr, nullptr, nullptr) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("DecryptInit 失败");
    }

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_IVLEN, 12, nullptr) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("设置 IV 长度失败");
    }

    if (EVP_DecryptInit_ex(ctx, nullptr, nullptr, reinterpret_cast<const unsigned char*>(key.data()), reinterpret_cast<const unsigned char*>(nonce.data())) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Key/Nonce 设置失败");
    }

    if (!associated.empty())
    {
        int outLen = 0;
        if (EVP_DecryptUpdate(ctx, nullptr, &outLen, reinterpret_cast<const unsigned char*>(associated.data()), static_cast<int>(associated.size())) != 1)
        {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("AAD 更新失败");
        }
    }

    std::string out;
    out.resize(static_cast<size_t>(encLen));

    int len = 0;
    int totalLen = 0;

    if (EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char*>(out.data()), &len, reinterpret_cast<const unsigned char*>(cipherText.data()), encLen) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("解密更新失败");
    }

    totalLen += len;

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_AEAD_SET_TAG, tagLen, (void*)tag) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("设置 tag 失败");
    }

    int finalLen = 0;

    if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char*>(out.data()) + totalLen, &finalLen) != 1)
    {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("DecryptFinal 失败（认证失败）");
    }

    totalLen += finalLen;

    EVP_CIPHER_CTX_free(ctx);

    out.resize(static_cast<size_t>(totalLen));

    result = out;
}

// Ecdh 部分

// first 为 Q（未压缩点，65 字节：04 || x || y），second 为 d （私钥 大整数，32 字节）
std::pair<std::string, std::string> AeadTool::GetECDHKeyPair()
{
    std::pair<std::string, std::string> result;

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
    if (!ctx) {
        throw std::runtime_error("创建 EVP_PKEY_CTX 上下文失败");
    }

    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);

        throw std::runtime_error("初始化密钥生成上下文失败");
    }

    if (EVP_PKEY_CTX_set_ec_paramgen_curve_nid(ctx, NID_X9_62_prime256v1) <= 0) {
        EVP_PKEY_CTX_free(ctx);

        throw std::runtime_error("设置椭圆曲线参数（prime256v1）失败");
    }

    EVP_PKEY* pkey = nullptr;
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0 || !pkey) {
        EVP_PKEY_CTX_free(ctx);

        throw std::runtime_error("生成密钥对失败");
    }

    EVP_PKEY_CTX_free(ctx);

    // 获取私钥 d（BIGNUM）
    BIGNUM* priv_bn = nullptr;
    if (EVP_PKEY_get_bn_param(pkey, OSSL_PKEY_PARAM_PRIV_KEY, &priv_bn) <= 0 || !priv_bn) {
        EVP_PKEY_free(pkey);

        throw std::runtime_error("获取私钥 BIGNUM 参数失败");
    }

    std::string priv(32, '\0');
    if (BN_bn2binpad(priv_bn, reinterpret_cast<unsigned char*>(priv.data()), 32) != 32) {
        BN_free(priv_bn);
        EVP_PKEY_free(pkey);

        throw std::runtime_error("私钥转换为 32 字节大端整数失败");
    }

    BN_free(priv_bn);

    // 获取公钥 Q（未压缩点，65 字节）
    unsigned char* pub_buf = nullptr;
    size_t pub_len = 0;

    if (EVP_PKEY_get_octet_string_param(pkey, OSSL_PKEY_PARAM_PUB_KEY, nullptr, 0, &pub_len) <= 0) {
        EVP_PKEY_free(pkey);

        throw std::runtime_error("获取公钥参数长度失败");
    }

    pub_buf = static_cast<unsigned char*>(OPENSSL_malloc(pub_len));
    if (!pub_buf) {
        EVP_PKEY_free(pkey);

        throw std::runtime_error("分配公钥缓冲区内存失败");
    }

    if (EVP_PKEY_get_octet_string_param(pkey, OSSL_PKEY_PARAM_PUB_KEY, pub_buf, pub_len, &pub_len) <= 0) {
        OPENSSL_free(pub_buf);
        EVP_PKEY_free(pkey);

        throw std::runtime_error("获取公钥原始数据失败");
    }

    result.first.assign(reinterpret_cast<char*>(pub_buf), pub_len); // 公钥
    result.second = std::move(priv); // 私钥

    OPENSSL_free(pub_buf);
    EVP_PKEY_free(pkey);

    return result;
}

//std::string_view serverPrivate, std::string_view clientPublic
std::string AeadTool::CalECDHSharedKey(std::string_view clientPrivate, std::string_view serverPublic)
{
    if (serverPublic.size() != 65 || clientPrivate.size() != 32)
    {
        throw std::runtime_error("ECDH 参数长度无效：公钥需 65 字节，私钥需 32 字节");
    }

    EC_GROUP* group = EC_GROUP_new_by_curve_name(NID_X9_62_prime256v1);
    if (!group) {
        throw std::runtime_error("创建椭圆曲线组（prime256v1）失败");
    }

    EC_POINT* pubPoint = EC_POINT_new(group);
    if (!pubPoint) {
        EC_GROUP_free(group);
        throw std::runtime_error("创建椭圆曲线点失败");
    }

    if (!EC_POINT_oct2point(group, pubPoint, reinterpret_cast<const unsigned char*>(serverPublic.data()), serverPublic.size(), nullptr))
    {
        EC_POINT_free(pubPoint);
        EC_GROUP_free(group);
        throw std::runtime_error("公钥字节解码为椭圆曲线点失败");
    }

    BIGNUM* priv = BN_bin2bn(reinterpret_cast<const unsigned char*>(clientPrivate.data()), clientPrivate.size(), nullptr);
    if (!priv)
    {
        EC_POINT_free(pubPoint);
        EC_GROUP_free(group);
        throw std::runtime_error("私钥字节转换为大整数失败");
    }

    EC_POINT* sharedPoint = EC_POINT_new(group);
    if (!sharedPoint) {
        BN_free(priv);
        EC_POINT_free(pubPoint);
        EC_GROUP_free(group);
        throw std::runtime_error("创建共享点失败");
    }

    if (!EC_POINT_mul(group, sharedPoint, nullptr, pubPoint, priv, nullptr))
    {
        BN_free(priv);
        EC_POINT_free(pubPoint);
        EC_POINT_free(sharedPoint);
        EC_GROUP_free(group);
        throw std::runtime_error("椭圆曲线点乘计算失败");
    }

    BIGNUM* x = BN_new();
    if (!x) {
        BN_free(priv);
        EC_POINT_free(pubPoint);
        EC_POINT_free(sharedPoint);
        EC_GROUP_free(group);
        throw std::runtime_error("创建 BIGNUM 失败");
    }

    if (!EC_POINT_get_affine_coordinates_GFp(group, sharedPoint, x, nullptr, nullptr)) {
        BN_free(priv);
        BN_free(x);
        EC_POINT_free(pubPoint);
        EC_POINT_free(sharedPoint);
        EC_GROUP_free(group);
        throw std::runtime_error("获取共享点 x 坐标失败");
    }

    int len = BN_num_bytes(x);
    std::string result(32, '\0');

    if (BN_bn2binpad(x, reinterpret_cast<unsigned char*>(result.data()), 32) != 32) {
        BN_free(priv);
        BN_free(x);
        EC_POINT_free(pubPoint);
        EC_POINT_free(sharedPoint);
        EC_GROUP_free(group);
        throw std::runtime_error("转换共享密钥 x 坐标为 32 字节失败");
    }

    BN_free(priv);
    BN_free(x);
    EC_POINT_free(pubPoint);
    EC_POINT_free(sharedPoint);
    EC_GROUP_free(group);

    return result;
}

// KDF 密钥派生部分

std::string AeadTool::CalInfo(std::string_view clientPublic, std::string_view serverPublic)
{
    if (clientPublic.empty())
    {
        throw std::runtime_error("客户端公钥为空");
    }

    if (serverPublic.empty())
    {
        throw std::runtime_error("服务端公钥为空");
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

std::string AeadTool::CalSecretX(std::string_view serverPublic, std::string_view info, std::string_view sharedKey)
{
    EVP_KDF* kdf = EVP_KDF_fetch(NULL, OSSL_KDF_NAME_HKDF_SHA256, NULL);
    if (!kdf) {
        throw std::runtime_error("获取 HKDF-SHA256 算法失败");
    }

    EVP_KDF_CTX* ctx = EVP_KDF_CTX_new(kdf);
    EVP_KDF_free(kdf);

    if (!ctx) {
        throw std::runtime_error("创建 HKDF 上下文失败");
    }

    OSSL_PARAM params[4];
    size_t i = 0;

    params[i++] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_KEY, const_cast<void*>(reinterpret_cast<const void*>(sharedKey.data())), sharedKey.size());

    params[i++] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_SALT, const_cast<void*>(reinterpret_cast<const void*>(serverPublic.data())), serverPublic.size());

    params[i++] = OSSL_PARAM_construct_octet_string(OSSL_KDF_PARAM_INFO, const_cast<void*>(reinterpret_cast<const void*>(info.data())), info.size());

    params[i] = OSSL_PARAM_construct_end();

    std::string out;
    out.resize(32);

    if (EVP_KDF_derive(ctx, reinterpret_cast<unsigned char*>(out.data()), 32, params) <= 0) {
        EVP_KDF_CTX_free(ctx);

        throw std::runtime_error("HKDF 密钥派生失败");
    }

    EVP_KDF_CTX_free(ctx);

    return out;
}

// 加密分发

void AeadTool::Encrypt_BouncyCastle(std::string& result, std::string_view key, std::string_view nonce, std::string_view data, int dataLen, bool needAssociatedData, int function)
{
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
        std::throw_with_nested(std::runtime_error(errMsg.str()));
        return;
    }
    catch (...)
    {
        std::ostringstream errMsg;
        errMsg << "AeadTool: 未知错误: ";
        std::throw_with_nested(std::runtime_error(errMsg.str()));
    }
}

bool AeadTool::Dencrypt_BouncyCastle(std::string& result, std::string_view key, std::string_view nonce, std::string_view data, int dataLen, bool needAssociatedData, int function)
{
    try
    {
        std::string associated;
        if (needAssociatedData) {
            associated = nonce;
        }

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
        std::ostringstream errMsg;
        errMsg << "AeadTool: 发生错误: " << e.what();
        std::throw_with_nested(std::runtime_error(errMsg.str()));
    }
    catch (...)
    {
        std::ostringstream errMsg;
        errMsg << "AeadTool: 未知错误: ";
        std::throw_with_nested(std::runtime_error(errMsg.str()));
    }
}

// 数据弄脏和洗白部分

std::string AeadUtil::Obfuscate(std::string_view messageData, std::string_view key3)
{
    if (messageData.empty() || key3.empty()) {
        return std::string(messageData);
    }

    std::string result(messageData);
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

std::string AeadUtil::Wash(std::string_view messageData, std::string_view key3)
{
    if (messageData.empty() || key3.empty()) {
        return std::string(messageData);
    }

    std::string result(messageData);
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
