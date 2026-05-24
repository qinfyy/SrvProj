#pragma once
#include "APIDef.h"
#include <cstdint>
#include <string_view>
#include <string>
#include <sstream>
#include <vector>

//public static class ArchiveUtil // TypeDefIndex: 17121
//{
//	// Fields
//	internal const uint MAGIC = 1380008474; // Metadata: 0x01309728
//	private static byte[] numberBuffer; // 0x00
//	internal const uint CURRENT_VERSION = 102; // Metadata: 0x0130972D
//	internal const uint HEADER_FLAG_COMPRESS = 1; // Metadata: 0x0130972E
//	internal const uint HEADER_FLAG_ENCRYPTED = 16; // Metadata: 0x0130972F
//	internal const uint ENTRY_FLAG_COMPRESS = 256; // Metadata: 0x01309730
//	internal const uint ENTRY_FLAG_ENCRYPTED = 4096; // Metadata: 0x01309732
//	private const string KEY_STR = "&^^%#$#_$!@![]<_>?GHBFR_7481SDR_"; // Metadata: 0x01309734
//	[CompilerGenerated]
//		private static byte[] _Key_k__BackingField; // 0x08
//
//	// Properties
//	internal static byte[] Key{ [CompilerGenerated] get; [CompilerGenerated] private set; } // 0x000000018103EE60-0x000000018103EEB0 0x000000018103EEB0-0x000000018103EF20
//
//		// Nested types
//		[CompilerGenerated]
//		private sealed class __c__DisplayClass16_0 // TypeDefIndex: 17120
//	{
//		// Fields
//		public string filePath; // 0x10
//
//		// Constructors
//		public __c__DisplayClass16_0(); // 0x00000001806FA690-0x00000001806FA6A0
//
//		// Methods
//		internal bool _ReadFileArray_b__0(string exclude); // 0x00000001810529A0-0x00000001810529D0
//	}
//
//	// Constructors
//	static ArchiveUtil(); // 0x000000018103EDA0-0x000000018103EE60
//
//	// Methods
//	public static ulong XXHash64(string input); // 0x000000018103EC50-0x000000018103ED20
//	public static long GetFileSize(string filePath); // 0x000000018103D460-0x000000018103D4D0
//	public static string NormalizeFileName(string fileName); // 0x000000018103D4D0-0x000000018103D540
//	public static string NormalizePath(string fileName); // 0x000000018103D540-0x000000018103D5A0
//	public static List<string> ReadFileArray(string dir, string[] exts = null, string[] excludes = null); // 0x000000018103DE10-0x000000018103E1C0
//	public static ushort ReadU16LittleEndian(Stream reader); // 0x000000018103E620-0x000000018103E770
//	public static uint ReadU32LittleEndian(Stream reader); // 0x000000018103E770-0x000000018103E8C0
//	public static ulong ReadU64LittleEndian(Stream reader); // 0x000000018103E8C0-0x000000018103EA10
//	internal static void XorEncryptDecrypt(byte[] data, byte[] key, int offset, int length); // 0x000000018103ED20-0x000000018103EDA0
//	private static void UncompressDecryptStream(Stream inputStream, byte[] data, uint originSize, uint size); // 0x000000018103EA10-0x000000018103EB80
//	private static void DecryptStream(Stream inputStream, byte[] data); // 0x000000018103D350-0x000000018103D460
//	private static void UncompressStream(Stream inputStream, byte[] data, uint originSize, uint size); // 0x000000018103EB80-0x000000018103EC50
//	public static byte[] DecodeStream(Stream stream, long offset, bool isCompressed, bool isEncrypted, uint originSize, uint size); // 0x000000018103CFB0-0x000000018103D350
//	public static ArchiveEntry ReadEntry(Stream reader); // 0x000000018103D7A0-0x000000018103DB80
//	public static ArchiveEntry[] ReadEntries(Stream stream, ArchiveHeader header); // 0x000000018103D5A0-0x000000018103D7A0
//	public static ArchiveHeader ReadHeader(Stream stream); // 0x000000018103E1C0-0x000000018103E620
//
//	// Extension methods
//	public static void ReadExactly(this Stream stream, byte[] buffer, int offset, int count); // 0x000000018103DB80-0x000000018103DE10
//}

struct ArchiveEntry;
class  ArchiveHeader;

class CSARCX_DEF ArchiveUtil
{
private:
	inline static const std::string_view KEY_STR = "&^^%#$#_$!@![]<_>?GHBFR_7481SDR_";
	inline static std::string mKey = std::string(KEY_STR);

public:
	inline static const uint32_t MAGIC = 1380008474;
	inline static const uint32_t CURRENT_VERSION = 102;
	inline static const uint32_t HEADER_FLAG_COMPRESS = 1;
	inline static const uint32_t HEADER_FLAG_ENCRYPTED = 16;
	inline static const uint32_t ENTRY_FLAG_COMPRESS = 256;
	inline static const uint32_t ENTRY_FLAG_ENCRYPTED = 4096;

	static std::string get_Key();
	static void set_Key(std::string value);
	static uint64_t XXHash64(std::string_view input);
	static int64_t GetFileSize(std::string_view filePath);
    static std::string NormalizeFileName(std::string_view fileName);

    static std::string NormalizePath(std::string_view fileName);
	static std::vector<std::string> ReadFileArray(std::string_view dir, const std::vector<std::string>& exts, const std::vector<std::string>& excludes);
	static uint16_t ReadU16LittleEndian(std::istream& reader);
	static uint32_t ReadU32LittleEndian(std::istream& reader);
	static uint64_t ReadU64LittleEndian(std::istream& reader);
	static void XorEncryptDecrypt(std::vector<uint8_t>& data, std::string_view key, int offset, int length);
	static void UncompressDecryptStream(std::istream& inputStream, std::vector<uint8_t>& data, uint32_t originSize, uint32_t size);
	static void DecryptStream(std::istream& inputStream, std::vector<uint8_t>& data);
	static void UncompressStream(std::istream& inputStream, std::vector<uint8_t>& data, uint32_t originSize, uint32_t size);
	static std::vector<uint8_t> DecodeStream(std::istream& stream, long offset, bool isCompressed, bool isEncrypted, uint32_t originSize, uint32_t size);
	static ArchiveEntry ReadEntry(std::istream& reader);
	static std::vector<ArchiveEntry> ReadEntries(std::istream& stream, const ArchiveHeader& header);
	static ArchiveHeader ReadHeader(std::istream& stream);
	// Extension methods
	static void ReadExactly(std::istream& stream, std::vector<uint8_t>& buffer, int offset, int count);

	// 封包扩展
	static void WriteU32LittleEndian(std::ostream& os, uint32_t v);
	static void WriteU64LittleEndian(std::ostream& os, uint64_t v);
	static std::vector<uint8_t> ReadFile(const std::string& path);
	static std::vector<uint8_t> EncodeStream(const std::vector<uint8_t>& input, bool doCompress, bool doEncrypt);
	static bool WriteHeader(std::ostream& stream, const ArchiveHeader& header);
	static bool WriteEntry(std::ostream& outStream, const ArchiveEntry& entryTable);
	static bool WriteEntries(std::ostream& outStream, const std::vector<ArchiveEntry>& entries);

};

CSARCX_DEF void LZ4Decode(const std::vector<uint8_t>& buffer, std::vector<uint8_t>& result, size_t originSize);

CSARCX_DEF std::vector<uint8_t> LZ4Compress(const std::vector<uint8_t>& input);

std::vector<uint8_t> get___CK();

std::vector<uint8_t> DA(const std::vector<uint8_t>& input);

std::vector<uint8_t> EA(const std::vector<uint8_t>& input);

// 生成标志位
CSARCX_DEF uint32_t GenerateFlags(bool isHeaderCompressed, bool isHeaderEncrypted, bool isBlockCompressed, bool isBlockEncrypted);
