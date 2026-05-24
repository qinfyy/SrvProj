#include "pch.h"
#include "Archive.h"
#include <filesystem>
#include <fstream>
#include "RecoveryPath.h"
#include <iostream>
#include <random>
#include "ThreadPool.h"

Archive::Archive(std::stringstream buffer, uint64_t archiveSize, const ArchiveHeader& header, std::vector<ArchiveEntry> entries) :
	mBuffer(std::move(buffer)),
	mArchiveSize(archiveSize),
	mHeader(header),
	mEntries(std::move(entries)) {}

// Methods
std::optional<ArchiveEntry> Archive::FindEntry(uint64_t hash) {
    for (const auto& entry : mEntries) {
        if (entry.Hash == hash) {
            return entry;
        }
    }

    return std::nullopt;
}

std::vector<uint8_t> Archive::ReadFile(uint64_t hash) {
    auto entry = FindEntry(hash);
    if (!entry) {
        throw std::runtime_error("找不到条目，Hash: 0x" +
            std::to_string(hash));
    }

    if (!mBuffer.good()) {
        throw std::runtime_error("流损坏");
    }

    std::vector<uint8_t> data(entry->Size);
    mBuffer.seekg(entry->Offset);
    mBuffer.read(reinterpret_cast<char*>(data.data()), entry->Size);

    if (!mBuffer.good()) {
        throw std::runtime_error("读取文件失败, 流已经损坏");
    }

	auto dataVec = ArchiveUtil::DecodeStream(mBuffer, entry->Offset, mHeader.IsEntryCompressed(), mHeader.IsEntryEncrypted(), entry->OriginSize, entry->Size);

    // DA 解密
    auto finalData = DA(dataVec);

    return finalData;
}

std::vector<uint8_t> Archive::ReadFile(std::string_view fileName) {
    std::string lower = ArchiveUtil::NormalizeFileName(fileName);
    uint64_t hash = ArchiveUtil::XXHash64(lower);
    auto data = ReadFile(hash);
    return data;
}

std::unique_ptr<Archive> Archive::Open(std::string path) {
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error(path + " 文件不存在，Archive无法加载");
    }

    uint64_t archiveSize = ArchiveUtil::GetFileSize(path);
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("无法打开文件: " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    ArchiveHeader header = ArchiveUtil::ReadHeader(buffer);
    std::vector<ArchiveEntry> entries = ArchiveUtil::ReadEntries(buffer, header);
    if (entries.empty()) {
        throw std::runtime_error("Archive Entries 为空: " + path);
    }

    return std::make_unique<Archive>(std::move(buffer), archiveSize, header, std::move(entries));
}

int Unpack(const std::string& inputFilePath, const std::string& outPath, const std::string& pathDictionary, bool displayMessage) {
    InitMap(pathDictionary);
    auto arc = Archive::Open(inputFilePath);

    if (displayMessage) {
        std::cout << "Archive 头部: " << std::endl;
        std::cout << "Version: " << arc->mHeader.mVersion << std::endl;
        std::cout << "Flag: " << arc->mHeader.mFlag << std::endl;
        std::cout << "Origin Size: " << arc->mHeader.mOriginSize << std::endl;
        std::cout << "Size: " << arc->mHeader.mSize << std::endl;
        std::cout << "Entry Count: " << arc->mHeader.mEntryCount << std::endl;
        std::cout << std::endl;
    }

	auto res = arc->mEntries;

    int count = 0;
    for (ArchiveEntry& e : res) {
        try {
            auto data = arc->ReadFile(e.Hash);

            std::string outfilePath = (std::filesystem::path(outPath) / HashGetPath(e.Hash)).string();
            std::filesystem::path dir = std::filesystem::path(outfilePath).parent_path();
            if (!dir.empty() && !std::filesystem::exists(dir)) {
                std::filesystem::create_directories(dir);
            }

			if (displayMessage) {
				std::cout << "解包: " << outfilePath << std::endl;
			}

            std::ofstream outfile(outfilePath, std::ios::binary);
            if (outfile) {
                outfile.write(reinterpret_cast<const char*>(data.data()), data.size());
                outfile.close();
                ++count;
            }
            else {
                throw std::runtime_error("打开文件进行写入时出错: " + outfilePath);
            }
        }
        catch (const std::exception& ex) {
            std::throw_with_nested(std::runtime_error("读取包错误: " + std::string(ex.what())));
            throw;
        }
    }

    if (displayMessage) {
        std::cout << "已解包: " << count << " 个文件" << std::endl;
    }

    return count;
}

bool PackSingleThread(const std::string& inputPath, const std::string& outPath, bool rePack, uint32_t flags, bool displayMessage)
{
    std::ostringstream outStream;
    std::vector<std::filesystem::path> filePaths;
    std::vector<std::string> relativePaths;

    std::filesystem::path basePath = std::filesystem::absolute(inputPath);

    bool IsHeaderCompressed = (flags & HEADER_COMPRESSED) != 0;
    bool IsHeaderEncrypted = (flags & HEADER_ENCRYPTED) != 0;
    bool IsBlockCompressed = (flags & BLOCK_COMPRESSED) != 0;
    bool IsBlockEncrypted = (flags & BLOCK_ENCRYPTED) != 0;


    if (displayMessage) {
        std::cout << "扫描文件..." << std::endl;
    }

    // 扫描文件
    for (auto& p : std::filesystem::recursive_directory_iterator(basePath, std::filesystem::directory_options::skip_permission_denied)) {
        if (!p.is_regular_file()) continue;

        std::filesystem::path fullPath = p.path();
        auto fileSize = std::filesystem::file_size(fullPath);
        if (fileSize == 0) {
            continue;
        }

        std::filesystem::path relativePath = std::filesystem::relative(fullPath, basePath);
        std::string relativeStr = relativePath.generic_string();

        filePaths.push_back(fullPath);
        relativePaths.push_back(relativeStr);
    }

    if (displayMessage) {
        std::cout << "扫描完成: " << filePaths.size() << " 个文件" << std::endl;
    }

    if (filePaths.empty()) {
        if (displayMessage) {
            std::cout << "没有文件需要打包" << std::endl;
        }

        return false;
    }

    std::vector<std::pair<std::filesystem::path, std::string>> paired(filePaths.size());
    for (size_t i = 0; i < filePaths.size(); ++i) {
        paired[i] = { filePaths[i], relativePaths[i] };
    }

    std::sort(paired.begin(), paired.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
        });

    for (size_t i = 0; i < paired.size(); ++i) {
        filePaths[i] = paired[i].first;
        relativePaths[i] = paired[i].second;
    }

    // 计算头表大小
    const uint32_t MAGIC_SIZE = 4;
    const uint32_t HEADER_FIELD_COUNT = 5;   // 不含MAGIC
    const uint32_t HEADER_SIZE = MAGIC_SIZE + HEADER_FIELD_COUNT * 4;  // = 4 + 5 * 4 = 24
    const uint32_t ENTRY_SIZE = 8 + 4 + 4 + 4;  // 20字节
    const uint32_t ENTRY_TABLE_SIZE = static_cast<uint32_t>(filePaths.size()) * ENTRY_SIZE;
    const uint32_t DATA_START_OFFSET = HEADER_SIZE + ENTRY_TABLE_SIZE;

    if (displayMessage) {
        std::cout << "Header 大小: " << HEADER_SIZE << " 字节" << std::endl;
        std::cout << "Entry Table 大小: " << ENTRY_TABLE_SIZE << " 字节" << std::endl;
        std::cout << "数据起始偏移: " << DATA_START_OFFSET << std::endl;
    }

    // 写入占位数据
    ArchiveUtil::WriteU32LittleEndian(outStream, 0);  // MAGIC 占位
    for (int i = 0; i < HEADER_FIELD_COUNT; ++i) {
        ArchiveUtil::WriteU32LittleEndian(outStream, 0);  // Header 字段占位
    }

    std::vector<uint8_t> entryTablePlaceholder(ENTRY_TABLE_SIZE, 0);
    outStream.write(reinterpret_cast<const char*>(entryTablePlaceholder.data()), ENTRY_TABLE_SIZE);

    // 写入数据
    if (displayMessage) {
        std::cout << "写入文件数据..." << std::endl;
    }

    auto dataStart = std::chrono::steady_clock::now();

    std::vector<ArchiveEntry> entries(filePaths.size());
    uint32_t currentOffset = DATA_START_OFFSET;

    for (size_t i = 0; i < filePaths.size(); ++i) {
        const std::filesystem::path& fullPath = filePaths[i];
        const std::string& relativeStr = relativePaths[i];

        // 计算哈希
        uint64_t hash = 0;
        if (rePack) {
            hash = stoull(relativeStr);
        }
        else {
            std::string normalized = ArchiveUtil::NormalizeFileName(relativeStr);
			hash = ArchiveUtil::XXHash64(normalized);
        }

        // 读取并处理文件
        std::vector<uint8_t> original = ArchiveUtil::ReadFile(fullPath.string());
        auto eadata = EA(original);
        std::vector<uint8_t> compressed = ArchiveUtil::EncodeStream(eadata, IsBlockCompressed, IsBlockEncrypted);

        // 填充 entry
        entries[i].Hash = hash;
        entries[i].OriginSize = static_cast<uint32_t>(eadata.size());
        entries[i].Size = static_cast<uint32_t>(compressed.size());
        entries[i].Offset = currentOffset;

        // 直接写入压缩数据
        outStream.write(reinterpret_cast<const char*>(compressed.data()), compressed.size());
        currentOffset += compressed.size();

        if (displayMessage && ((i + 1) % 50 == 0 || (i + 1) == filePaths.size())) {
            std::cout << "\r进度: " << (i + 1) << "/" << filePaths.size() << " ("
                << ((i + 1) * 100 / filePaths.size()) << "%)" << std::flush;
        }
    }

    if (displayMessage) {
        std::cout << std::endl;
        std::cout << "数据写入完成" << std::endl;

        // 构建 Entry Table
        std::cout << "构建 Entry Table..." << std::endl;
    }

    // 构建 Entry Table
    std::stringstream entryTable;
	ArchiveUtil::WriteEntries(entryTable, entries);

    entryTable.seekg(0, std::ios::end);
    size_t entryTableLen = entryTable.tellg();
    entryTable.seekg(0, std::ios::beg);

    std::vector<uint8_t> entryTableVec(entryTableLen);
    entryTable.read(reinterpret_cast<char*>(entryTableVec.data()), entryTableLen);

    std::vector<uint8_t> compressedEntryTable = ArchiveUtil::EncodeStream(entryTableVec, IsHeaderCompressed, IsHeaderEncrypted);

    if (compressedEntryTable.size() >= entryTableVec.size()) {
        IsHeaderCompressed = false;
        compressedEntryTable = ArchiveUtil::EncodeStream(entryTableVec, IsHeaderCompressed, IsHeaderEncrypted);
    }

    // 计算填充大小
    uint32_t padding = 0;
    if (compressedEntryTable.size() < ENTRY_TABLE_SIZE) {
        padding = ENTRY_TABLE_SIZE - static_cast<uint32_t>(compressedEntryTable.size());
    }

    else if (compressedEntryTable.size() > ENTRY_TABLE_SIZE) {
        throw std::runtime_error("Entry Table 压缩后超出预留空间，打包失败");
    }

    // 写回文件头
    ArchiveHeader header(ArchiveUtil::CURRENT_VERSION, 0, entryTableVec.size(), compressedEntryTable.size(), entries.size());
    header.SetEntryCompressed(IsBlockCompressed);
    header.SetEntryEncrypted(IsBlockEncrypted);
    header.SetHeaderCompressed(IsHeaderCompressed);
    header.SetHeaderEncrypted(IsHeaderEncrypted);

    outStream.seekp(0, std::ios::beg);
    ArchiveUtil::WriteHeader(outStream, header);

    if (displayMessage) {
        std::cout << "写入文件头..." << std::endl;
        std::cout << std::endl;

        std::cout << "Archive 头部: " << std::endl;
        std::cout << "Version: " << header.mVersion << std::endl;
        std::cout << "Flags: " << std::endl;
        std::cout << "IsHeaderCompressed: " << header.IsHeaderCompressed() << std::endl;
        std::cout << "IsHeaderEncrypted: " << header.IsHeaderEncrypted() << std::endl;
        std::cout << "IsBlockCompressed: " << header.IsEntryCompressed() << std::endl;
        std::cout << "IsBlockEncrypted: " << header.IsEntryEncrypted() << std::endl;
        std::cout << "Origin Size: " << header.mOriginSize << std::endl;
        std::cout << "Size: " << header.mSize << std::endl;
        std::cout << "Entry Count: " << header.mEntryCount << std::endl;

        std::cout << std::endl;
        std::cout << "写入目录表..." << std::endl;
    }

    // 写入压缩后的 Entry Table
    outStream.write(reinterpret_cast<const char*>(compressedEntryTable.data()), compressedEntryTable.size());

    // 写入填充字节
    if (padding > 0) {
        std::random_device rd;
        std::minstd_rand0 gen(rd());
        std::uniform_int_distribution<unsigned int> dis(0, 255);
        for (size_t i = 0; i < padding; ++i) {
            uint8_t byte = static_cast<uint8_t>(dis(gen));
            outStream.write(reinterpret_cast<const char*>(&byte), 1);
        }
    }

    outStream.seekp(0, std::ios::end);

    std::ofstream file(outPath, std::ios::binary);
    if (file) {
        auto view = outStream.view();
        file.write(view.data(), view.size());
    }

    if (displayMessage) {
        std::cout << "打包完成!" << std::endl;
    }

	return true;
}

struct FileProcessResult {
    size_t index;
    std::vector<uint8_t> compressed;
    uint32_t originSize;
    uint32_t compressedSize;
    uint64_t hash;
    bool success = true;
    std::string errorMsg;
};

// 多线程 Pack 函数
bool PackMultiThread(const std::string& inputPath, const std::string& outPath, bool rePack, uint32_t flags, size_t threadCount, bool displayMessage)
{
    std::ostringstream outStream;
    if (threadCount == 0) {
        threadCount = std::max<size_t>(1, std::thread::hardware_concurrency());
    }

    if (displayMessage) {
        std::cout << "使用线程数：" << threadCount << std::endl;
    }

    std::vector<std::filesystem::path> filePaths;
    std::vector<std::string> relativePaths;

    std::filesystem::path basePath = std::filesystem::absolute(inputPath);

    bool IsHeaderCompressed = (flags & HEADER_COMPRESSED) != 0;
    bool IsHeaderEncrypted = (flags & HEADER_ENCRYPTED) != 0;
    bool IsBlockCompressed = (flags & BLOCK_COMPRESSED) != 0;
    bool IsBlockEncrypted = (flags & BLOCK_ENCRYPTED) != 0;

    // 扫描文件
    if (displayMessage) {
        std::cout << "扫描文件..." << std::endl;
    }

    std::vector<std::filesystem::path> allDirectories;
    allDirectories.push_back(basePath);

    for (auto& p : std::filesystem::recursive_directory_iterator(basePath, std::filesystem::directory_options::skip_permission_denied)) {
        if (p.is_directory()) {
            allDirectories.push_back(p.path());
        }
    }

    if (displayMessage) {
        std::cout << "找到 " << allDirectories.size() << " 个目录" << std::endl;
    }

    std::mutex pathsMutex;
    {
        ThreadPool pool(threadCount);
        std::vector<std::future<void>> futures;
        futures.reserve(allDirectories.size());

        for (const auto& dir : allDirectories) {
            futures.push_back(pool.enqueue([&pathsMutex, &filePaths, &relativePaths, &basePath, dir]() {
                for (auto& p : std::filesystem::directory_iterator(dir, std::filesystem::directory_options::skip_permission_denied)) {
                    if (!p.is_regular_file()) continue;

                    std::filesystem::path fullPath = p.path();
                    auto fileSize = std::filesystem::file_size(fullPath);
                    if (fileSize == 0) {
                        continue;
                    }

                    std::filesystem::path relativePath = std::filesystem::relative(fullPath, basePath);
                    std::string relativeStr = relativePath.generic_string();

                    {
                        std::lock_guard<std::mutex> lock(pathsMutex);
                        filePaths.push_back(fullPath);
                        relativePaths.push_back(relativeStr);
                    }
                }
                }));
        }

        pool.waitAll();
    }

    // 排序保证确定性
    if (!filePaths.empty()) {
        std::vector<std::pair<std::filesystem::path, std::string>> paired(filePaths.size());
        for (size_t i = 0; i < filePaths.size(); ++i) {
            paired[i] = { filePaths[i], relativePaths[i] };
        }
        std::sort(paired.begin(), paired.end(), [](const auto& a, const auto& b) {
            return a.second < b.second;
            });

        for (size_t i = 0; i < paired.size(); ++i) {
            filePaths[i] = paired[i].first;
            relativePaths[i] = paired[i].second;
        }
    }

    if (displayMessage) {
        std::cout << "扫描完成: " << filePaths.size() << " 个文件" << std::endl;
    }

    if (filePaths.empty()) {
        if (displayMessage) {
            std::cout << "没有文件需要打包" << std::endl;
        }

        return false;
    }

    // 计算头表大小
    const uint32_t MAGIC_SIZE = 4;
    const uint32_t HEADER_FIELD_COUNT = 5;
    const uint32_t HEADER_SIZE = MAGIC_SIZE + HEADER_FIELD_COUNT * 4;
    const uint32_t ENTRY_SIZE = 20;
    const uint32_t ENTRY_TABLE_SIZE = static_cast<uint32_t>(filePaths.size()) * ENTRY_SIZE;

    const uint32_t DATA_START_OFFSET = HEADER_SIZE + ENTRY_TABLE_SIZE;

    if (displayMessage) {
        std::cout << "Header 大小: " << HEADER_SIZE << " 字节" << std::endl;
        std::cout << "Entry Table 大小: " << ENTRY_TABLE_SIZE << " 字节" << std::endl;
        std::cout << "数据起始偏移: " << DATA_START_OFFSET << std::endl;
    }

    // 写入 MAGIC 占位
    ArchiveUtil::WriteU32LittleEndian(outStream, 0);

    // 写入 Header 字段占位
    for (int i = 0; i < HEADER_FIELD_COUNT; ++i) {
        ArchiveUtil::WriteU32LittleEndian(outStream, 0);
    }

    std::vector<uint8_t> entryTablePlaceholder(ENTRY_TABLE_SIZE, 0);
    outStream.write(reinterpret_cast<const char*>(entryTablePlaceholder.data()), ENTRY_TABLE_SIZE);

    // 并行处理文件并直接写入数据
    if (displayMessage) {
        std::cout << "写入文件数据..." << std::endl;
    }

    auto dataStart = std::chrono::steady_clock::now();

    std::vector<ArchiveEntry> entries(filePaths.size());
    std::atomic<size_t> processedCount(0);
    std::atomic<size_t> errorCount(0);
    std::mutex coutMutex;
    std::atomic<uint32_t> currentOffset(DATA_START_OFFSET);

    {
        ThreadPool pool(threadCount);
        size_t BATCH_SIZE = threadCount * 2;
        if (BATCH_SIZE < 1) BATCH_SIZE = 1;

        for (size_t batchStart = 0; batchStart < filePaths.size(); batchStart += BATCH_SIZE) {
            size_t batchEnd = min(batchStart + BATCH_SIZE, filePaths.size());
            std::vector<std::future<std::pair<size_t, FileProcessResult>>> batchFutures;
            batchFutures.reserve(batchEnd - batchStart);

            // 提交批次任务
            for (size_t i = batchStart; i < batchEnd; ++i) {
                batchFutures.push_back(pool.enqueue([&, i]() -> std::pair<size_t, FileProcessResult> {
                    FileProcessResult result{};
                    result.index = i;

                    try {
                        const std::filesystem::path& fullPath = filePaths[i];
                        const std::string& relativeStr = relativePaths[i];

                        // 计算哈希
                        uint64_t hash = 0;
                        if (rePack) {
                            hash = stoull(relativeStr);
                        }
                        else {
                            std::string normalized = ArchiveUtil::NormalizeFileName(relativeStr);
                            hash = ArchiveUtil::XXHash64(normalized);
                        }

                        std::vector<uint8_t> original = ArchiveUtil::ReadFile(fullPath.string());
                        auto eadata = EA(original);
                        std::vector<uint8_t> compressed = ArchiveUtil::EncodeStream(eadata, IsBlockCompressed, IsBlockEncrypted);

                        result.hash = hash;
                        result.originSize = static_cast<uint32_t>(eadata.size());
                        result.compressedSize = static_cast<uint32_t>(compressed.size());
                        result.compressed = std::move(compressed);
                        result.success = true;

                    }
                    catch (const std::exception& e) {
                        result.success = false;
                        result.errorMsg = e.what();
                    }

                    return { i, result };
                    }));
            }

            // 等待批次完成并顺序写入
            for (auto& fut : batchFutures) {
                auto [idx, result] = fut.get();

                if (!result.success) {
                    ++errorCount;
                    throw std::runtime_error("文件处理失败: " + filePaths[idx].string() + " - " + result.errorMsg);
                }

                entries[idx].Hash = result.hash;
                entries[idx].OriginSize = result.originSize;
                entries[idx].Size = result.compressedSize;
                entries[idx].Offset = currentOffset.load();

                currentOffset += result.compressedSize;

                outStream.write(reinterpret_cast<const char*>(result.compressed.data()), result.compressed.size());

                ++processedCount;
                if (displayMessage && (processedCount % 50 == 0 || processedCount == filePaths.size())) {
                    std::lock_guard<std::mutex> lock(coutMutex);
                    std::cout << "\r进度: " << processedCount << "/" << filePaths.size() << " ("
                        << (processedCount * 100 / filePaths.size()) << "%)" << std::flush;
                }
            }
        }
    }

    std::cout << std::endl;
    if (displayMessage) {
        std::cout << "数据写入完成" << std::endl;
    }

    if (errorCount > 0) {
        throw std::runtime_error("部分文件处理失败");
    }

    // 构建 Entry Table

    if (displayMessage) {
        std::cout << "构建 Entry Table..." << std::endl;
    }

    std::stringstream entryTable;
	ArchiveUtil::WriteEntries(entryTable, entries);

    entryTable.seekg(0, std::ios::end);
    size_t entryTableLen = entryTable.tellg();
    entryTable.seekg(0, std::ios::beg);

    std::vector<uint8_t> entryTableVec(entryTableLen);
    entryTable.read(reinterpret_cast<char*>(entryTableVec.data()), entryTableLen);

    std::vector<uint8_t> compressedEntryTable = ArchiveUtil::EncodeStream(entryTableVec, IsHeaderCompressed, IsHeaderEncrypted);

    if (compressedEntryTable.size() >= entryTableVec.size()) {
        IsHeaderCompressed = false;
        compressedEntryTable = ArchiveUtil::EncodeStream(entryTableVec, IsHeaderCompressed, IsHeaderEncrypted);
    }

    // 计算填充大小
    uint32_t padding = 0;
    if (compressedEntryTable.size() < ENTRY_TABLE_SIZE) {
        padding = ENTRY_TABLE_SIZE - static_cast<uint32_t>(compressedEntryTable.size());
    }
    else if (compressedEntryTable.size() > ENTRY_TABLE_SIZE) {
        throw std::runtime_error("Entry Table 压缩后超出预留空间，打包失败");
    }

    // 跳回文件头，写入头部数据
    ArchiveHeader header(ArchiveUtil::CURRENT_VERSION, 0, entryTableVec.size(), compressedEntryTable.size(), entries.size());
    header.SetEntryCompressed(IsBlockCompressed);
    header.SetEntryEncrypted(IsBlockEncrypted);
    header.SetHeaderCompressed(IsHeaderCompressed);
    header.SetHeaderEncrypted(IsHeaderEncrypted);
    //header._Version_k__BackingField = VERSION;
    //header._Flag_k__BackingField = GenerateFlag(IsHeaderCompressed, IsHeaderEncrypted, IsBlockCompressed, IsBlockEncrypted);
    //header._OriginSize_k__BackingField = static_cast<uint32_t>(entryTableVec.size());
    //header._Size_k__BackingField = static_cast<uint32_t>(compressedEntryTable.size());
    //header._EntryCount_k__BackingField = static_cast<uint32_t>(entries.size());

    // 写头
    outStream.seekp(0, std::ios::beg);
    ArchiveUtil::WriteHeader(outStream, header);

    if (displayMessage) {
        std::cout << "写入文件头..." << std::endl;
        std::cout << std::endl;

        std::cout << "Archive 头部: " << std::endl;
        std::cout << "Version: " << header.mVersion << std::endl;
        std::cout << "Flags: " << std::endl;
        std::cout << "IsHeaderCompressed: " << header.IsHeaderCompressed() << std::endl;
        std::cout << "IsHeaderEncrypted: " << header.IsHeaderEncrypted() << std::endl;
        std::cout << "IsBlockCompressed: " << header.IsEntryCompressed() << std::endl;
        std::cout << "IsBlockEncrypted: " << header.IsEntryEncrypted() << std::endl;
        std::cout << "Origin Size: " << header.mOriginSize << std::endl;
        std::cout << "Size: " << header.mSize << std::endl;
        std::cout << "Entry Count: " << header.mEntryCount << std::endl;

        std::cout << std::endl;
        std::cout << "写入目录表..." << std::endl;
    }

    // 写目录表
    outStream.write(reinterpret_cast<const char*>(compressedEntryTable.data()), compressedEntryTable.size());

    if (padding > 0) {
        std::random_device rd;
        std::minstd_rand0 gen(rd());
        std::uniform_int_distribution<unsigned int> dis(0, 255);

        for (size_t i = 0; i < padding; ++i) {
            uint8_t byte = static_cast<uint8_t>(dis(gen));
            outStream.write(reinterpret_cast<const char*>(&byte), 1);
        }
    }

    outStream.seekp(0, std::ios::end);

    std::ofstream file(outPath, std::ios::binary);
    if (file) {
        auto view = outStream.view();
        file.write(view.data(), view.size());  // 完整写入
    }

    if (displayMessage) {
        std::cout << "打包完成!" << std::endl;
    }

    return true;
}
