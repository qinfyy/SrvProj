#pragma once
#include <sstream>
#include <vector>
#include <typeinfo>
#include <string>

class Archive;

struct GeneralItem {
	std::string key;
	int32_t len;
	std::string data;
};

struct BytesFileHeader {
	uint32_t magic;
	uint16_t versionLen;
	std::string versionText;
	uint8_t mode1;
	uint8_t mode2;
	uint32_t count;
};

void LoadResources();
void ParseBytesFile(std::istream& inputStream, BytesFileHeader& outHeader, std::vector<GeneralItem>& outItems);

inline static bool loaded;

template<typename T>
std::string GetTypeName() {
	const char* name = typeid(T).name();
	std::string result = name;
	if (result.find("class ") == 0) {
		result = result.substr(6);
	}
	else if (result.find("struct ") == 0) {
		result = result.substr(7);
	}
	return result;
}

