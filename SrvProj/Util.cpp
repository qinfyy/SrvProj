#include "Util.h"
#include <string>
#include <iomanip>
#include <unordered_map>
#include <windows.h>

std::string Utf16ToUtf8(const std::wstring& wstr)
{
    if (wstr.empty())
        return {};

    auto size = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(), NULL, 0, NULL, NULL);

    std::string result(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int)wstr.size(),result.data(), size, NULL, NULL);

    return result;
}

std::wstring Utf8ToUtf16(const std::string& str)
{
    if (str.empty())
        return {};

    auto size = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), NULL, 0);

    std::wstring result(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.data(), (int)str.size(), result.data(), size);

    return result;
}

std::string Utf16ToAnsi(const std::wstring& wstr) {
    if (wstr.empty())
        return std::string();

    auto codePage = GetACP();
    auto sizeNeeded = WideCharToMultiByte(codePage, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
    if (sizeNeeded <= 0)
        return std::string();

    std::string result(sizeNeeded, 0);
    WideCharToMultiByte(codePage, 0, wstr.c_str(), (int)wstr.size(), &result[0], sizeNeeded, NULL, NULL);

    return result;
}

std::wstring AnsiToUtf16(const std::string& str) {
    if (str.empty())
        return std::wstring();

    auto codePage = GetACP();
    auto sizeNeeded = MultiByteToWideChar(codePage, 0, str.c_str(), (int)str.size(), NULL, 0);
    if (sizeNeeded <= 0)
        return std::wstring();

    std::wstring result(sizeNeeded, 0);
    MultiByteToWideChar(codePage, 0, str.c_str(), (int)str.size(), &result[0], sizeNeeded);

    return result;
}

std::string AnsiToUtf8(const std::string& str) {
	auto utf16 = AnsiToUtf16(str);
	auto utf8 = Utf16ToUtf8(utf16);
    return utf8;
}

std::string Utf8ToAnsi(const std::string& str) {
    auto utf16 = Utf8ToUtf16(str);
    auto ansi = Utf16ToAnsi(utf16);
    return ansi;
}

bool ContainsIgnoreCaseA(PCSTR haystack, PCSTR needle)
{
    int hlen = (int)strlen(haystack);
    int nlen = (int)strlen(needle);

    for (int i = 0; i <= hlen - nlen; ++i)
    {
        if (CompareStringA(LOCALE_INVARIANT, NORM_IGNORECASE, haystack + i, nlen, needle, nlen) == CSTR_EQUAL)
        {
            return true;
        }
    }
    return false;
}

