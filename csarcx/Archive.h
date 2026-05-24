#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <optional>
#include <sstream>
#include <filesystem>
#include "APIDef.h"
#include "ArchiveUtil.h"

//public sealed class Archive // TypeDefIndex: 17116
//{
//	// Fields
//	[CompilerGenerated]
//	private ulong _ArchiveSize_k__BackingField; // 0x10
//	[CompilerGenerated]
//		private ArchiveHeader _Header_k__BackingField; // 0x18
//	[CompilerGenerated]
//		private string _FilePath_k__BackingField; // 0x20
//	private readonly ArchiveEntry[] entries; // 0x28
//
//	// Properties
//	public ulong ArchiveSize{ [CompilerGenerated] get; [CompilerGenerated] private set; } // 0x0000000180730850-0x0000000180730860 0x0000000180FBFDE0-0x0000000180FBFDF0
//	public ArchiveHeader Header{ [CompilerGenerated] get; [CompilerGenerated] private set; } // 0x000000018072CC20-0x000000018072CC30 0x00000001807308C0-0x00000001807308D0
//	public string FilePath{ [CompilerGenerated] get; [CompilerGenerated] private set; } // 0x00000001806FB0B0-0x00000001806FB0C0 0x00000001806FB0F0-0x00000001806FB100
//	public ulong[] Hashs{ get; } // 0x000000018103F960-0x000000018103FA10 
//
//		// Constructors
//	public Archive(string path, ulong archiveSize, ArchiveHeader header, ArchiveEntry[] entries); // 0x000000018103F8F0-0x000000018103F960
//
//	// Methods
//	public ArchiveEntry ? FindEntry(ulong hash); // 0x000000018103EF20-0x000000018103F040
//	public byte[] ReadFile(ulong hash); // 0x000000018103F2C0-0x000000018103F700
//	public byte[] ReadFile(string fileName); // 0x000000018103F700-0x000000018103F8F0
//	public static Archive Open(string path); // 0x000000018103F040-0x000000018103F2C0
//}

//public struct ArchiveEntry // TypeDefIndex: 17118
//{
//	// Fields
//	[CompilerGenerated]
//	private ulong _Hash; // 0x00
//	[CompilerGenerated]
//		private uint _Offset; // 0x08
//	[CompilerGenerated]
//		private uint _OriginSize; // 0x0C
//	[CompilerGenerated]
//		private uint _Size; // 0x10
//	public const int SIZE = 20; // Metadata: 0x01309726
//
//	// Properties
//	public ulong Hash{ [CompilerGenerated] [IsReadOnly] get; [CompilerGenerated] private set; } // 0x00000001807CCDC0-0x00000001807CCDD0 0x000000018103CF70-0x000000018103CF80
//	public uint Offset{ [IsReadOnly] [CompilerGenerated] get; [CompilerGenerated] private set; } // 0x00000001807A87C0-0x00000001807A87D0 0x00000001807D00C0-0x00000001807D0120
//	public uint OriginSize{ [CompilerGenerated] [IsReadOnly] get; [CompilerGenerated] private set; } // 0x0000000180FF8690-0x0000000180FF86A0 0x0000000180FF86B0-0x0000000180FF86C0
//	public uint Size{ [IsReadOnly] [CompilerGenerated] get; [CompilerGenerated] private set; } // 0x00000001806FA010-0x00000001806FA070 0x00000001806FB100-0x00000001806FB110
//
//		// Constructors
//	public ArchiveEntry(ulong hash, uint offset, uint originSize, uint size); // 0x000000018103CF50-0x000000018103CF70
//
//	// Methods
//	[IsReadOnly]
//		public byte[] ReadFile(FileStream stream, bool isCompressed, bool isEncrypted); // 0x000000018103CEB0-0x000000018103CF50
//}

//public sealed class ArchiveHeader // TypeDefIndex: 17119
//{
//	// Fields
//	[CompilerGenerated]
//	private uint _Version; // 0x10
//	[CompilerGenerated]
//		private uint _Flag; // 0x14
//	[CompilerGenerated]
//		private uint _OriginSize; // 0x18
//	[CompilerGenerated]
//		private uint _Size; // 0x1C
//	[CompilerGenerated]
//		private uint _EntryCount; // 0x20
//	public const int SIZE = 24; // Metadata: 0x01309727
//
//	// Properties
//	public uint Version() const; // 0x00000001806FA010-0x00000001806FA070 0x00000001806FB100-0x00000001806FB110
//	public uint Flag() const; // 0x00000001806FB0C0-0x00000001806FB0D0 0x00000001806FB110-0x00000001806FB120
//	public uint OriginSize() const; // 0x00000001806FB0A0-0x00000001806FB0B0 0x00000001806FB0E0-0x00000001806FB0F0
//	public uint Size() const; // 0x00000001806FB0D0-0x00000001806FB0E0 0x00000001806FB120-0x00000001806FB130
//	public uint EntryCount() const; // 0x000000018072CCB0-0x000000018072CCC0 0x00000001807C1560-0x00000001807C1570
//	public bool IsHeaderEncrypted{ get; } // 0x000000018103CFA0-0x000000018103CFB0 
//	public bool IsHeaderCompressed{ get; } // 0x000000018086D7D0-0x000000018086D7E0 
//	public bool IsEntryCompressed{ get; } // 0x000000018103CF80-0x000000018103CF90 
//	public bool IsEntryEncrypted{ get; } // 0x000000018103CF90-0x000000018103CFA0 
//
//		// Constructors
//	public ArchiveHeader(uint version, uint flag, uint originSize, uint size, uint entryCount); // 0x0000000180980A30-0x0000000180980A90
//}

struct ArchiveEntry
{
	uint64_t Hash;
	uint32_t Offset;
	uint32_t OriginSize;
	uint32_t Size;
};

class CSARCX_DEF ArchiveHeader
{
public:
	uint32_t mVersion;
	uint32_t mFlag;
	uint32_t mOriginSize;
	uint32_t mSize;
	uint32_t mEntryCount;
	inline static const int SIZE = 24; // Metadata: 0x01309727

	bool IsHeaderEncrypted() const {
		return (mFlag & ArchiveUtil::HEADER_FLAG_ENCRYPTED) != 0;
	}

	bool IsHeaderCompressed() const {
		return (mFlag & ArchiveUtil::HEADER_FLAG_COMPRESS) != 0;
	}

	bool IsEntryCompressed() const {
		return (mFlag & ArchiveUtil::ENTRY_FLAG_COMPRESS) != 0;
	}

	bool IsEntryEncrypted() const {
		return (mFlag & ArchiveUtil::ENTRY_FLAG_ENCRYPTED) != 0;
	}

	// Constructors
	ArchiveHeader(uint32_t version, uint32_t flag, uint32_t originSize, uint32_t size, uint32_t entryCount) :
		mVersion(version),
		mFlag(flag),
		mOriginSize(originSize),
		mSize(size),
		mEntryCount(entryCount)
	{
	};

	// 扩展
	void SetHeaderEncrypted(bool headerEncrypted) {
		if (headerEncrypted) {
			mFlag |= ArchiveUtil::HEADER_FLAG_ENCRYPTED;
		}
		else {
			mFlag &= ~ArchiveUtil::HEADER_FLAG_ENCRYPTED;
		}
	}

	void SetHeaderCompressed(bool headerCompressed) {
		if (headerCompressed) {
			mFlag |= ArchiveUtil::HEADER_FLAG_COMPRESS;
		}
		else {
			mFlag &= ~ArchiveUtil::HEADER_FLAG_COMPRESS;
		}
	}

	void SetEntryCompressed(bool entryCompressed) {
		if (entryCompressed) {
			mFlag |= ArchiveUtil::ENTRY_FLAG_COMPRESS;
		}
		else {
			mFlag &= ~ArchiveUtil::ENTRY_FLAG_COMPRESS;
		}
	}

	void SetEntryEncrypted(bool entryEncrypted) {
		if (entryEncrypted) {
			mFlag |= ArchiveUtil::ENTRY_FLAG_ENCRYPTED;
		}
		else {
			mFlag &= ~ArchiveUtil::ENTRY_FLAG_ENCRYPTED;
		}
	}
};

class CSARCX_DEF Archive
{
public:
	uint64_t mArchiveSize;
	ArchiveHeader mHeader;
	std::stringstream mBuffer;
	std::vector<ArchiveEntry> mEntries;

	// Methods
	std::optional<ArchiveEntry> FindEntry(uint64_t hash);
	std::vector<uint8_t> ReadFile(uint64_t hash);
	std::vector<uint8_t> ReadFile(std::string_view fileName);
	static std::unique_ptr<Archive> Open(std::string path);

	// Constructors
	Archive(std::stringstream buffer, uint64_t archiveSize, const ArchiveHeader&, std::vector<ArchiveEntry> entries);
};

CSARCX_DEF int Unpack(const std::string& inputFilePath, const std::string& outPath, const std::string& pathDictionary = "", bool displayMessage = false);

enum PackFlag : uint32_t {
	HEADER_COMPRESSED = ArchiveUtil::HEADER_FLAG_COMPRESS,
	HEADER_ENCRYPTED = ArchiveUtil::HEADER_FLAG_ENCRYPTED,
	BLOCK_COMPRESSED = ArchiveUtil::ENTRY_FLAG_COMPRESS,
	BLOCK_ENCRYPTED = ArchiveUtil::ENTRY_FLAG_ENCRYPTED
};

constexpr uint32_t DEFAULT_PACK_FLAGS = HEADER_COMPRESSED | HEADER_ENCRYPTED | BLOCK_COMPRESSED | BLOCK_ENCRYPTED;

CSARCX_DEF bool PackSingleThread(const std::string& inputPath, const std::string& outPath, bool rePack, uint32_t flags = DEFAULT_PACK_FLAGS, bool displayMessage = false);

CSARCX_DEF bool PackMultiThread(const std::string& inputPath, const std::string& outPath, bool rePack, uint32_t flags = DEFAULT_PACK_FLAGS, size_t threadCount = 0, bool displayMessage = false);
