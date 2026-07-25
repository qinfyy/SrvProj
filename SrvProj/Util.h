#pragma once
#include <span>
#include <string>
#include <windows.h>

#define U8(str) reinterpret_cast<const char*>(u8##str)

double RandomDouble();
int RandomInt(int minIncl, int maxIncl);
bool RandomChance(double chance);

std::string Utf16ToUtf8(const std::wstring& wstr);

std::wstring Utf8ToUtf16(const std::string& str);

std::string Utf16ToAnsi(const std::wstring& wstr);

std::wstring AnsiToUtf16(const std::string& str);

std::string AnsiToUtf8(const std::string& str);

std::string Utf8ToAnsi(const std::string& str);

bool ContainsIgnoreCaseA(PCSTR haystack, PCSTR needle);

std::string ToHex(std::string_view bin, bool lowerCase = false, bool addSpace = false);

std::string ToHex(std::span<uint8_t> bin, bool lowerCase = false, bool addSpace = false);

std::string Base64Encode(std::string_view input);

std::string Base64Decode(const std::string& input);

bool GenerateToken(std::string& outToken, bool lowerCase);
