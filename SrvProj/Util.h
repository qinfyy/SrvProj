#pragma once
#include <string>
#include <windows.h>

#define U8(str) reinterpret_cast<const char*>(u8##str)

std::string Utf16ToUtf8(const std::wstring& wstr);

std::wstring Utf8ToUtf16(const std::string& str);

std::string Utf16ToAnsi(const std::wstring& wstr);

std::wstring AnsiToUtf16(const std::string& str);

std::string AnsiToUtf8(const std::string& str);

std::string Utf8ToAnsi(const std::string& str);

bool ContainsIgnoreCaseA(PCSTR haystack, PCSTR needle);
