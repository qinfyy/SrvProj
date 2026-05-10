#pragma once
#include <string>
#include <vector>

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

	using AeadEncryptHandler = void(*)(std::string& result, const std::string& key, const std::string& nonce, const std::string& data, int dataLen, bool needAssociatedData);
	using AeadDecryptHandler = bool(*)(std::string& result, const std::string& key, const std::string& nonce, const std::string& data, int dataLen, bool needAssociatedData);

	static AeadEncryptHandler Encrypt;
	static AeadDecryptHandler Decrypt;

	static int function; // 0 = AES-GCM, 1 = chacha20
	static int NonceSize;
	static int MacSize;
	static int KeySize;
	static int IVSize;
	//static GcmBlockCipher cipher;
	//static AesEngine engine;
	//static ChaCha20Poly1305 cipherCha20Poly1305;

	static std::string DecryptAesCBCInfo(const char* key, const char* IV, const std::string& cipherBytes);
	
	static std::string EncryptAesCBCInfo(const char* key, const char* IV, const std::string& plainBytes);

	static std::string CalInfo(const std::string& clientPublic, const std::string& serverPublic); // 0x000000018130B500-0x000000018130B5D0
	static std::string CalSecretX(const std::string& serverPublic, const std::string& info, const std::string& sharedKey); // 0x000000018130B5D0-0x000000018130B700
	static void InitAeadTool(); // 0x000000018130CBA0-0x000000018130CF20
	//static void InitBouncyCastle(); // 0x000000018130CF20-0x000000018130D070

	static std::vector<std::vector<uint8_t>> GetECDHKeyPair(); // index0为公钥 index1为私钥
	static std::string CalECDHSharedKey(const std::string& clinetPrivate, const std::string& serverPublic); // 0x000000018130B2B0-0x000000018130B500

	// Temp Extend
	static void Encrypt_Static(std::string& result, const std::string& key, const std::string& nonce, const std::string& data, int dataLen, bool needAssociatedData, int function);
	static bool Decrypt_Static(std::string& result, const std::string& key, const std::string& nonce, const std::string& data, int dataLen, bool needAssociatedData, int function);
private:
	static std::string associatedData;

	static void Encrypt_BouncyCastle(std::string& result, const std::string& key, const std::string& nonce, const std::string& data, int dataLen, bool needAssociatedData); // 0x000000018130C710-0x000000018130C8E0
	static bool Dencrypt_BouncyCastle(std::string& result, const std::string& key, const std::string& nonce, const std::string& data, int dataLen, bool needAssociatedData); // 0x000000018130BE20-0x000000018130C000

	static void Encrypt_BouncyCastle_AesGcm(const std::string& key, const std::string& nonce, const std::string& secretMessage, int dataLen, std::string& associated, std::string& result); // 0x000000018130C000-0x000000018130C380
	static void Decrypt_BouncyCastle_AesGcm(const std::string& key, const std::string& nonce, const std::string& cipherText, int dataLen, std::string& associated, std::string& result); // 0x000000018130B8F0-0x000000018130BB80
	static void Encrypt_BouncyCastle_ChaCha20Poly1305(const std::string& key, const std::string& nonce, const std::string& secretMessage, int dataLen, std::string& associated, std::string& result); // 0x000000018130C380-0x000000018130C710
	static void Decrypt_BouncyCastle_ChaCha20Poly1305(const std::string& key, const std::string& nonce, const std::string& cipherText, int dataLen, std::string& associated, std::string& result); // 0x000000018130BB80-0x000000018130BE20
};

class AeadUtil
{
public:
	static std::string Obfuscate(const std::string& messageData, const std::string& key3);
	static std::string Wash(const std::string& messageData, const std::string& key3);
};
