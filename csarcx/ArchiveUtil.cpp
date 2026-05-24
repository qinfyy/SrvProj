#include "pch.h"
#include "ArchiveUtil.h"
#include <iostream>
#include <stdexcept>
#include "lz4.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include "xxHash64.h"
#include "Archive.h"
#include "md5.h"
#include "xxtea.h"

std::string ArchiveUtil::get_Key() {
    if (mKey.empty()) {
        mKey = std::string(KEY_STR);
    }

    return mKey;
}

void ArchiveUtil::set_Key(std::string value) {
    mKey = value;
}

uint64_t ArchiveUtil::XXHash64(std::string_view input) {
    if (input.empty()) {
        return 0;
    }

    uint64_t hash = K4os_Hash_xxHash_XXH64_XXH64_hash(const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(input.data())), static_cast<int32_t>(input.size()), 0);
    return hash;
}

int64_t ArchiveUtil::GetFileSize(std::string_view filePath) {
    if (filePath.empty()) {
        throw std::invalid_argument("GetFileSize: filePath 不能为空");
    }

    std::error_code ec;
    auto size = std::filesystem::file_size(filePath, ec);

    if (ec) {
        throw std::runtime_error("GetFileSize: 无法获取文件大小 - " + ec.message());
    }

    return static_cast<int64_t>(size);
}

std::string ArchiveUtil::NormalizeFileName(std::string_view fileName) {
    if (fileName.empty()) {
        throw std::invalid_argument("NormalizeFileName: fileName 不能为空");
    }

    std::string result(fileName);
    for (char& c : result) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }

    return NormalizePath(result);
}

std::string ArchiveUtil::NormalizePath(std::string_view fileName) {
    if (fileName.empty()) {
        throw std::invalid_argument("NormalizePath: fileName 不能为空");
    }

    std::string result(fileName);
    std::replace(result.begin(), result.end(), '\\', '/');
    return result;
}

std::vector<std::string> ArchiveUtil::ReadFileArray(std::string_view dir, const std::vector<std::string>& exts, const std::vector<std::string>& excludes) {
    std::vector<std::string> result;

    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        return result;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
        if (!std::filesystem::is_regular_file(entry.path())) {
            continue;
        }

        std::string filename = entry.path().string();
        std::string ext = entry.path().extension().string();

        bool extMatch = exts.empty();
        if (!extMatch) {
            for (const auto& e : exts) {
                if (ext == e) {
                    extMatch = true;
                    break;
                }
            }
        }

        if (!extMatch) {
            continue;
        }

        bool excluded = false;
        for (const auto& ex : excludes) {
            if (filename.find(ex) != std::string::npos) {
                excluded = true;
                break;
            }
        }

        if (!excluded) {
            result.push_back(filename);
        }
    }

    return result;
}

uint16_t ArchiveUtil::ReadU16LittleEndian(std::istream& reader) {
    uint16_t value = 0;
    reader.read(reinterpret_cast<char*>(&value), sizeof(value));
    return value;
}

uint32_t ArchiveUtil::ReadU32LittleEndian(std::istream& reader) {
    uint32_t value = 0;
    reader.read(reinterpret_cast<char*>(&value), sizeof(value));
    return value;
}

uint64_t ArchiveUtil::ReadU64LittleEndian(std::istream& reader) {
    uint64_t value = 0;
    reader.read(reinterpret_cast<char*>(&value), sizeof(value));
    return value;
}

void ArchiveUtil::XorEncryptDecrypt(std::vector<uint8_t>& data, std::string_view key, int offset, int length) {
    if (key.empty() || offset < 0 || length <= 0) {
        return;
    }

    size_t start = static_cast<size_t>(offset);
    size_t total_len = static_cast<size_t>(length);
    size_t key_size = key.size();

    if (start >= data.size()) {
        return;
    }

    size_t available = data.size() - start;
    size_t process_len = (total_len < available) ? total_len : available;

    for (size_t i = 0; i < process_len; ++i) {
        size_t data_idx = start + i;
        size_t key_idx = i % key_size;
        data[data_idx] ^= key[key_idx];
    }
}

void ArchiveUtil::UncompressDecryptStream(std::istream& inputStream, std::vector<uint8_t>& data, uint32_t originSize, uint32_t size) {
    std::vector<uint8_t> buffer(size);
    ReadExactly(inputStream, buffer, 0, size);
    XorEncryptDecrypt(buffer, get_Key(), 0, static_cast<int>(buffer.size()));
    LZ4Decode(buffer, data, originSize);
}

void ArchiveUtil::DecryptStream(std::istream& inputStream, std::vector<uint8_t>& data) {
    ReadExactly(inputStream, data, 0, static_cast<int>(data.size()));
    XorEncryptDecrypt(data, get_Key(), 0, static_cast<int>(data.size()));
}

void ArchiveUtil::UncompressStream(std::istream& inputStream, std::vector<uint8_t>& data, uint32_t originSize, uint32_t size) {
    std::vector<uint8_t> buffer(size);
    ReadExactly(inputStream, buffer, 0, size);
    LZ4Decode(buffer, data, originSize);
}

std::vector<uint8_t> ArchiveUtil::DecodeStream(std::istream& stream, long offset, bool isCompressed, bool isEncrypted, uint32_t originSize, uint32_t size) {
    std::vector<uint8_t> result(originSize);

    std::streampos originalPos = stream.tellg();

    stream.seekg(offset, std::ios::beg);

    if (isCompressed && isEncrypted)
    {
        UncompressDecryptStream(stream, result, originSize, size);
    }
    else if (isCompressed)
    {
        UncompressStream(stream, result, originSize, size);
    }
    else if (isEncrypted)
    {
        //result.resize(originSize);
        DecryptStream(stream, result);
    }
    else
    {
        //result.resize(originSize);
        ReadExactly(stream, result, 0, originSize);
    }

    stream.clear();
    stream.seekg(originalPos);

    return result;
}

ArchiveEntry ArchiveUtil::ReadEntry(std::istream& reader) {
    ArchiveEntry entry;
    entry.Hash = ReadU64LittleEndian(reader);
    entry.Offset = ReadU32LittleEndian(reader);
    entry.OriginSize = ReadU32LittleEndian(reader);
    entry.Size = ReadU32LittleEndian(reader);

    //std::cout << "Archive Entry: " << std::endl;
    //std::cout << "Hash: " << entry.Hash << std::endl;
    //std::cout << "Offset: " << entry.Offset << std::endl;
    //std::cout << "Origin Size: " << entry.OriginSize << std::endl;
    //std::cout << "Size: " << entry.Size << std::endl;
    //std::cout << std::endl;

    return entry;
}

std::vector<ArchiveEntry> ArchiveUtil::ReadEntries(std::istream& stream, const ArchiveHeader& header) {
    std::vector<ArchiveEntry> entries;

    uint32_t flag = header.mFlag;

    uint32_t originSize = header.mOriginSize;
    uint32_t size = header.mSize;

    const uint32_t HEADER_SIZE = 24;
    auto decodedData = DecodeStream(stream, HEADER_SIZE, header.IsHeaderCompressed(), header.IsHeaderEncrypted(), originSize, size);

    std::istringstream decodedStream(std::string(decodedData.begin(), decodedData.end()));

    int entryCount = header.mEntryCount;
    entries.reserve(entryCount);

    for (int i = 0; i < entryCount; ++i) {
        auto entry = ReadEntry(decodedStream);
        entries.push_back(entry);
    }

    return entries;
}

ArchiveHeader ArchiveUtil::ReadHeader(std::istream& stream) {
    uint32_t magicNumber = ReadU32LittleEndian(stream);
    if (magicNumber != MAGIC) {
        std::ostringstream errOs;
        errOs << "文件头是错误的, Archive 无法加载! MAGIC: " << magicNumber;
        throw std::runtime_error(errOs.str());
    }

    uint32_t version = ReadU32LittleEndian(stream);

    if (version != CURRENT_VERSION) {
        std::ostringstream errOs;
        errOs << "文件版本不匹配, 当前版本: " << version << ", 适配版本: " << CURRENT_VERSION;
        throw std::runtime_error(errOs.str());
    }

    uint32_t flag = ReadU32LittleEndian(stream);
    uint32_t originSize = ReadU32LittleEndian(stream);
    uint32_t size = ReadU32LittleEndian(stream);
    uint32_t entryCount = ReadU32LittleEndian(stream);

    ArchiveHeader header(version, flag, originSize, size, entryCount);

    //std::cout << "Archive 头部: " << std::endl;
    //std::cout << "Version: " << header.mVersion << std::endl;
    //std::cout << "Flag: " << header.mFlag << std::endl;
    //std::cout << "Origin Size: " << header.mOriginSize << std::endl;
    //std::cout << "Size: " << header.mSize << std::endl;
    //std::cout << "Entry Count: " << header.mEntryCount << std::endl;
    //std::cout << std::endl;

    return header;
}

void ArchiveUtil::ReadExactly(std::istream& stream, std::vector<uint8_t>& buffer, int offset, int count) {
    if (!stream) {
        throw std::invalid_argument("ReadExactly: 流无效");
    }

    if (count < 0) {
        throw std::out_of_range("ReadExactly: count 不能为负数");
    }

    if (offset < 0) {
        throw std::out_of_range("ReadExactly: offset 不能为负数");
    }

    // 检查缓冲区大小是否足够
    size_t requiredSize = static_cast<size_t>(offset) + count;
    if (buffer.size() < requiredSize) {
        throw std::out_of_range("ReadExactly: 缓冲区不足，需要 " + std::to_string(requiredSize) + " 字节，实际只有 " + std::to_string(buffer.size()) + " 字节");
    }

    int bytesRead = 0;
    while (bytesRead < count) {
        char* writePos = reinterpret_cast<char*>(buffer.data()) + offset + bytesRead;
        size_t remaining = count - bytesRead;

        stream.read(writePos, remaining);
        std::streamsize n = stream.gcount();

        if (n <= 0) {
            throw std::runtime_error("ReadExactly: 流意外终止");
        }

        bytesRead += static_cast<int>(n);
    }
}

// 扩展
void ArchiveUtil::WriteU32LittleEndian(std::ostream& os, uint32_t v)
{
    os.write(reinterpret_cast<const char*>(&v), sizeof(uint32_t));
}

void ArchiveUtil::WriteU64LittleEndian(std::ostream& os, uint64_t v)
{
    os.write(reinterpret_cast<const char*>(&v), sizeof(uint64_t));
}

std::vector<uint8_t> ArchiveUtil::ReadFile(const std::string& path)
{
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("文件不存在: " + path);
    }

    auto size = GetFileSize(path);
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("无法打开文件: " + path);
    }

    std::vector<uint8_t> buffer(static_cast<size_t>(size));
    file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

    if (!file) {
        throw std::runtime_error("读取文件失败: 只读取了 " + std::to_string(file.gcount()) + " 字节，期望 " + std::to_string(buffer.size()) + " 字节");
    }

    return buffer;
}

std::vector<uint8_t> ArchiveUtil::EncodeStream(const std::vector<uint8_t>& input, bool doCompress, bool doEncrypt)

{
    std::vector<uint8_t> buffer;

    if (doCompress && doEncrypt)
    {
        buffer = LZ4Compress(input);
        XorEncryptDecrypt(buffer, get_Key(), 0, buffer.size());
    }
    else if (doCompress)
    {
        buffer = LZ4Compress(input);
    }
    else if (doEncrypt)
    {
        buffer = input;
        XorEncryptDecrypt(buffer, get_Key(), 0, buffer.size());
    }
    else
    {
        buffer = input;
    }

    return buffer;
}

bool ArchiveUtil::WriteHeader(std::ostream& outStream, const ArchiveHeader& header)
{
    WriteU32LittleEndian(outStream, MAGIC);
    WriteU32LittleEndian(outStream, header.mVersion);
    WriteU32LittleEndian(outStream, header.mFlag);
    WriteU32LittleEndian(outStream, header.mOriginSize);
    WriteU32LittleEndian(outStream, header.mSize);
    WriteU32LittleEndian(outStream, header.mEntryCount);

    return outStream.good();
}

bool ArchiveUtil::WriteEntry(std::ostream& outStream, const ArchiveEntry& entry)
{
    WriteU64LittleEndian(outStream, entry.Hash);
    WriteU32LittleEndian(outStream, entry.Offset);
    WriteU32LittleEndian(outStream, entry.OriginSize);
    WriteU32LittleEndian(outStream, entry.Size);

    return outStream.good();
}

bool ArchiveUtil::WriteEntries(std::ostream& outStream, const std::vector<ArchiveEntry>& entries) {
    for (const auto& e : entries) {
		WriteEntry(outStream, e);
    }

    return outStream.good();
}

void LZ4Decode(const std::vector<uint8_t>& buffer, std::vector<uint8_t>& result, size_t originSize)
{
    result.resize(originSize);

    int decompressedSize = LZ4_decompress_safe(
        reinterpret_cast<const char*>(buffer.data()),
        reinterpret_cast<char*>(result.data()),
        static_cast<int>(buffer.size()),
        static_cast<int>(originSize)
    );

    if (decompressedSize < 0) {
        throw std::runtime_error("LZ4 解压失败");
    }

    return;
}

std::vector<uint8_t> LZ4Compress(const std::vector<uint8_t>& input)
{
    if (input.empty())
        return {};

    int maxDstSize = LZ4_compressBound(static_cast<int>(input.size()));
    std::vector<uint8_t> compressed(maxDstSize);

    int compressedSize = LZ4_compress_default(
        reinterpret_cast<const char*>(input.data()),
        reinterpret_cast<char*>(compressed.data()),
        static_cast<int>(input.size()),
        maxDstSize
    );

    if (compressedSize <= 0)
        throw std::runtime_error("LZ4 压缩失败");

    compressed.resize(compressedSize);
    return compressed;
}

std::vector<uint8_t> get___CK() {
    int32_t __kx = 255;
    int32_t __ky = 255;
    int32_t product = __kx * __ky;

    std::vector<uint8_t> bytes(sizeof(product));
    memcpy(bytes.data(), &product, sizeof(product));

    return C(bytes);
}

std::vector<uint8_t> DA(const std::vector<uint8_t>& input) {
    std::vector<uint8_t> CK = get___CK();
    size_t len = input.size();
    size_t keyLen = CK.size();
    size_t outLen = 0;
    void* decryptedData = xxtea_decrypt(input.data(), len, CK.data(), &outLen);
    std::vector<uint8_t> decrypted(reinterpret_cast<uint8_t*>(decryptedData), reinterpret_cast<uint8_t*>(decryptedData) + outLen);
    return decrypted;
}

std::vector<uint8_t> EA(const std::vector<uint8_t>& input) {
    std::vector<uint8_t> CK = get___CK();
    size_t len = input.size();
    size_t key_len = CK.size();
    size_t out_len = 0;
    void* encrypted_data = xxtea_encrypt(input.data(), len, CK.data(), &out_len);
    std::vector<uint8_t> encrypted(reinterpret_cast<uint8_t*>(encrypted_data), reinterpret_cast<uint8_t*>(encrypted_data) + out_len);
    return encrypted;
}

uint32_t GenerateFlags(bool isHeaderCompressed, bool isHeaderEncrypted, bool isBlockCompressed, bool isBlockEncrypted) {
    uint32_t flags = 0;
    if (isHeaderCompressed) {
        flags |= ArchiveUtil::HEADER_FLAG_COMPRESS;
    }

    if (isHeaderEncrypted) {
        flags |= ArchiveUtil::HEADER_FLAG_ENCRYPTED;
    }

    if (isBlockCompressed) {
        flags |= ArchiveUtil::ENTRY_FLAG_COMPRESS;
    }

    if (isBlockEncrypted) {
        flags |= ArchiveUtil::ENTRY_FLAG_ENCRYPTED;
    }

    return flags;
}
