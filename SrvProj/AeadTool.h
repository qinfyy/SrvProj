#pragma once
#include <string>
#include <vector>
#include <string_view>
#include <span>

/* public static class AeadTool // TypeDefIndex: 4625
{
    // Fields
    public static AeadEncryptHandler Encrypt; // 0x00
    public static AeadDecryptHandler Decrypt; // 0x08
    private static readonly byte[] associatedData; // 0x10
    public static int function; // 0x18
    public static readonly int NonceSize; // 0x1C
    public static readonly int MacSize; // 0x20
    public static readonly int KeySize; // 0x24
    public static readonly int IVSize; // 0x28
    private static GcmBlockCipher cipher; // 0x30
    private static AesEngine engine; // 0x38
    private static ChaCha20Poly1305 cipherCha20Poly1305; // 0x40

    // Properties
    public static ReadOnlySpan<byte> AEADMARK{ get; } // 0x000000018130D150-0x000000018130D250 

        // Nested types
    public delegate void AeadEncryptHandler(byte[] result, byte[] key, byte[] nonce, byte[] data, int dataLen, bool needAssociatedData); // TypeDefIndex: 4623; 0x000000018130B1C0-0x000000018130B1E0

    public delegate bool AeadDecryptHandler(byte[] result, byte[] key, byte[] nonce, byte[] data, int dataLen, bool needAssociatedData); // TypeDefIndex: 4624; 0x000000018130AFF0-0x000000018130B010

    // Constructors
    static AeadTool(); // 0x000000018130D070-0x000000018130D150

    // Methods
    private static void Encrypt_BouncyCastle(byte[] result, byte[] key, byte[] nonce, byte[] data, int dataLen, bool needAssociatedData); // 0x000000018130C710-0x000000018130C8E0
    private static bool Dencrypt_BouncyCastle(byte[] result, byte[] key, byte[] nonce, byte[] data, int dataLen, bool needAssociatedData); // 0x000000018130BE20-0x000000018130C000
    public static byte[] CalInfo(byte[] clientPublic, byte[] serverPublic); // 0x000000018130B500-0x000000018130B5D0
    public static byte[] CalSecretX(byte[] serverPublic, byte[] info, byte[] sharedKey); // 0x000000018130B5D0-0x000000018130B700
    public static void InitAeadTool(); // 0x000000018130CBA0-0x000000018130CF20
    public static void InitBouncyCastle(); // 0x000000018130CF20-0x000000018130D070
    private static void Encrypt_BouncyCastle_AesGcm(byte[] key, byte[] nonce, byte[] secretMessage, int dataLen, byte[] associated, byte[] result); // 0x000000018130C000-0x000000018130C380
    private static void Decrypt_BouncyCastle_AesGcm(byte[] key, byte[] nonce, byte[] cipherText, int dataLen, byte[] associated, byte[] result); // 0x000000018130B8F0-0x000000018130BB80
    private static void Encrypt_BouncyCastle_ChaCha20Poly1305(byte[] key, byte[] nonce, byte[] secretMessage, int dataLen, byte[] associated, byte[] result); // 0x000000018130C380-0x000000018130C710
    private static void Decrypt_BouncyCastle_ChaCha20Poly1305(byte[] key, byte[] nonce, byte[] cipherText, int dataLen, byte[] associated, byte[] result); // 0x000000018130BB80-0x000000018130BE20
    public static byte[][] GetECDHKeyPair(); // 0x000000018130C8E0-0x000000018130CBA0
    public static byte[] CalECDHSharedKey(byte[] clinetPrivate, byte[] serverPublic); // 0x000000018130B2B0-0x000000018130B500
    public static byte[] DecryptAesCBCInfo(byte[] key, byte[] IV, byte[] cipherBytes); // 0x000000018130B700-0x000000018130B8F0
} */

class AeadTool
{
public:
    inline static const char* twServerMetaKey = "owGYVDmfHrxi^4pm";
    inline static const char* twServerGarbleKey = "N&mfco452ZH5!nE3s&o5uxB57UGPENVo";

    inline static const int NonceSize = 12;
    inline static const int MacSize = 128; // 单位 bit
    inline static const int KeySize = 32;
    inline static const int IVSize = 16;

    static std::string EncryptAesCBCInfo(std::string_view key, std::string_view IV, std::string_view plainBytes);
    
    static std::string DecryptAesCBCInfo(std::string_view key, std::string_view IV, std::string_view cipherBytes);

    static std::string CalInfo(std::string_view clientPublic, std::string_view serverPublic);
    static std::string CalSecretX(std::string_view serverPublic, std::string_view info, std::string_view sharedKey);

    // first 为 Q（公钥，未压缩点，65 字节：04 || x || y），second 为 d （私钥，大整数，32 字节）
    static std::pair<std::string, std::string> GetECDHKeyPair();
    static std::string CalECDHSharedKey(std::string_view clinetPrivate, std::string_view serverPublic);

    static void Encrypt_BouncyCastle(std::string& result, std::string_view key, std::string_view nonce, std::string_view data, int dataLen, bool needAssociatedData, int function);
    static bool Dencrypt_BouncyCastle(std::string& result, std::string_view key, std::string_view nonce, std::string_view data, int dataLen, bool needAssociatedData, int function);
private:
    static void Encrypt_BouncyCastle_AesGcm(std::string_view key, std::string_view nonce, std::string_view secretMessage, int dataLen, std::string_view associated, std::string& result);
    static void Decrypt_BouncyCastle_AesGcm(std::string_view key, std::string_view nonce, std::string_view cipherText, int dataLen, std::string_view associated, std::string& result);
    static void Encrypt_BouncyCastle_ChaCha20Poly1305(std::string_view key, std::string_view nonce, std::string_view secretMessage, int dataLen, std::string_view associated, std::string& result);
    static void Decrypt_BouncyCastle_ChaCha20Poly1305(std::string_view key, std::string_view nonce, std::string_view cipherText, int dataLen, std::string_view associated, std::string& result);
};

class AeadUtil
{
public:
    static std::string Obfuscate(std::string_view messageData, std::string_view key3);
    static std::string Wash(std::string_view messageData, std::string_view key3);
};
