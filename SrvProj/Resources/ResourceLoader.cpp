#include "ResourceLoader.h"
#include "GameData.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <Archive.h>
#include "ResBase.h"
#include "BinClass/CharacterRes.h"
#include "BinClass/DiscRes.h"

template<typename T>
static bool Read(std::istream& inputStream, T& out) {
    static_assert(std::is_arithmetic_v<T>, "Read 仅支持基本类型");

    inputStream.read(reinterpret_cast<char*>(&out), sizeof(T));
    return inputStream.gcount() == sizeof(T);
}

static bool ReadBytes(std::istream& inputStream, std::string& out, size_t len) {
    out.resize(len);
    inputStream.read(out.data(), len);
    return inputStream.gcount() == len;
}

template<typename T>
static bool ReadBigEndian(std::istream& inputStream, T& outValue) {
    static_assert(std::is_integral_v<T>, "ReadBigEndian 仅支持整数类型");

    std::string buffer;
    if (!ReadBytes(inputStream, buffer, sizeof(T))) {
        return false;
    }

    outValue = 0;
    const unsigned char* data = reinterpret_cast<const unsigned char*>(buffer.data());

    for (size_t i = 0; i < sizeof(T); ++i) {
        outValue |= static_cast<T>(data[i]) << ((sizeof(T) - 1 - i) * 8);
    }

    return true;
}

void ParseBytesFile(std::istream& inputStream, BytesFileHeader& outHeader, std::vector<GeneralItem>& outItems) {
    if (!inputStream) {
        throw std::runtime_error("输入流无效");
    }

    if (!ReadBigEndian(inputStream, outHeader.magic)) {
        throw std::runtime_error("读取 magic 失败");
    }

    if (outHeader.magic != 0x54930300) {
        throw std::runtime_error("magic 不匹配，文件格式错误");
    }

    if (!Read(inputStream, outHeader.versionLen)) {
        throw std::runtime_error("读取 versionLen 失败");
    }

    if (!ReadBytes(inputStream, outHeader.versionText, outHeader.versionLen)) {
        throw std::runtime_error("读取 versionText 失败");
    }

    if (!Read(inputStream, outHeader.mode1)) {
        throw std::runtime_error("读取 mode1 失败");
    }

    if (!Read(inputStream, outHeader.mode2)) {
        throw std::runtime_error("读取 mode2 失败");
    }

    if (!Read(inputStream, outHeader.count)) {
        throw std::runtime_error("读取 count 失败");
    }

    outItems.clear();
    outItems.reserve(outHeader.count);

    for (uint32_t i = 0; i < outHeader.count; i++) {
        GeneralItem item{};

        try
        {
            // map
            if (outHeader.mode1 == 1) {
                if (outHeader.mode2 == 1) {
                    // int32 key
                    int32_t key;
                    if (!Read(inputStream, key)) {
                        throw std::runtime_error("读取 int32 key 失败");
                    }

                    item.key = std::to_string(key);

                    uint16_t len;
                    if (!Read(inputStream, len)) {
                        throw std::runtime_error("读取 value len 失败");
                    }

                    item.len = len;

                    if (!ReadBytes(inputStream, item.data, len)) {
                        throw std::runtime_error("读取 value 数据失败");
                    }
                }
                else if (outHeader.mode2 == 2) {
                    // int64 key
                    int64_t key;
                    if (!Read(inputStream, key)) {
                        throw std::runtime_error("读取 int64 key 失败");
                    }

                    item.key = std::to_string(key);

                    uint16_t len;
                    if (!Read(inputStream, len)) {
                        throw std::runtime_error("读取 value len 失败");
                    }

                    item.len = len;
                    if (!ReadBytes(inputStream, item.data, len)) {
                        throw std::runtime_error("读取 value 数据失败");
                    }
                }
                else {
                    // string key
                    uint16_t klen;
                    if (!Read(inputStream, klen)) {
                        throw std::runtime_error("读取 string key len 失败");
                    }

                    std::string key;
                    if (!ReadBytes(inputStream, key, klen)) {
                        throw std::runtime_error("读取 string key 失败");
                    }

                    item.key = key;

                    uint16_t vlen;
                    if (!Read(inputStream, vlen)) {
                        throw std::runtime_error("读取 value len 失败");
                    }

                    item.len = vlen;
                    if (!ReadBytes(inputStream, item.data, vlen)) {
                        throw std::runtime_error("读取 value 数据失败");
                    }
                }
            }
            else { // list
                item.key = std::to_string(i + 1);

                uint16_t len;
                if (!Read(inputStream, len)) {
                    throw std::runtime_error("读取 list len 失败");
                }

                item.len = len;
                if (!ReadBytes(inputStream, item.data, len)) {
                    throw std::runtime_error("读取 list 数据失败");
                }
            }
        }
        catch (const std::exception& e)
        {
            std::throw_with_nested(std::runtime_error("ParseBytesFile: 第 " + std::to_string(i) + " 条记录解析失败"));
        }

        outItems.emplace_back(std::move(item));
    }
}

std::string GetBytesFileNameName(const std::string& typeName) {
    auto fileName = "bin/" + typeName.substr(0, typeName.length() - 3) + ".bytes";
	return fileName;
}

template<typename T, typename Container>
void LoadRes(Archive* arc, Container& container) {
    auto resName = GetTypeName<T>();
    BytesFileHeader header;
    std::vector<GeneralItem> items;
	auto bytesFileName = GetBytesFileNameName(resName);
    auto inFile = arc->ReadFile(bytesFileName);
    std::stringstream inStream;
    inStream.write(reinterpret_cast<const char*>(inFile.data()), inFile.size());
    ParseBytesFile(inStream, header, items);

    for (const auto& item : items) {
        try {
            T res;
            if (!res.LoadFromPb(item.data)) {
                throw std::runtime_error("从 protobuf 数据加载资源失败");
            }

            container.emplace(std::stoi(item.key), std::move(res));
        }
        catch (const std::exception& e) {
            std::throw_with_nested(std::runtime_error("LoadRes: 解析 " + resName + "中 key = " + item.key + " 的记录失败"));
        }
    }
}

void LoadResources() {
    auto inputFilePath = "./data.arcx";
    auto arc = Archive::Open(inputFilePath);

    LoadRes<CharacterRes>(arc.get(), GameData::CharacterDataTable);
    LoadRes<ChatRes>(arc.get(), GameData::ChatDataTable);
    LoadRes<DiscRes>(arc.get(), GameData::DiscDataTable);
}
