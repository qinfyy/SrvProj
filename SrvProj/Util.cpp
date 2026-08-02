#include "Util.h"
#include <string>
#include <iomanip>
#include <sstream>
#include <span>
#include <array>
#include <random>
#include <windows.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/rand.h>

double RandomDouble()
{
    static thread_local std::mt19937 rng{ std::random_device{}() };
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}

int RandomInt(int minIncl, int maxIncl)
{
    if (maxIncl <= minIncl)
    {
        return minIncl;
    }

    static thread_local std::mt19937 rng{ std::random_device{}() };
    std::uniform_int_distribution<int> dist(minIncl, maxIncl);
    return dist(rng);
}

bool RandomChance(double chance)
{
    return chance > 0.0 && RandomDouble() < chance;
}

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

std::string ToHex(std::string_view bin, bool lowerCase, bool addSpace) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');

    if (lowerCase) {
        oss << std::nouppercase;
    }
    else {
        oss << std::uppercase;
    }

    for (size_t i = 0; i < bin.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(bin[i]);
        oss << std::setw(2) << static_cast<int>(c);
        if (addSpace && i != bin.size() - 1) {
            oss << ' ';
        }
    }

    return oss.str();
}

std::string ToHex(std::span<uint8_t> bin, bool lowerCase, bool addSpace) {
    return ToHex(std::string_view(reinterpret_cast<const char*>(bin.data()), bin.size()), lowerCase, addSpace);
}

std::string Base64Encode(std::string_view input)
{
    BIO* bio = BIO_new(BIO_f_base64());
    BIO* mem = BIO_new(BIO_s_mem());
    bio = BIO_push(bio, mem);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, input.data(), input.size());
    BIO_flush(bio);

    BUF_MEM* bufferPtr;
    BIO_get_mem_ptr(bio, &bufferPtr);

    std::string result(bufferPtr->data, bufferPtr->length);

    BIO_free_all(bio);
    return result;
}

std::string Base64Decode(const std::string& input)
{
    BIO* bio = BIO_new(BIO_f_base64());
    BIO* mem = BIO_new_mem_buf(input.data(), input.size());
    bio = BIO_push(bio, mem);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    std::string output(input.size(), '\0');
    int decodedLen = BIO_read(bio, output.data(), output.size());

    BIO_free_all(bio);

    if (decodedLen > 0) {
        output.resize(decodedLen);
        return output;
    }

    return "";
}

bool GenerateToken(std::string& outToken, bool lowerCase) {
    std::array<uint8_t, 16> buf;

    if (RAND_bytes(buf.data(), buf.size()) != 1) {
        outToken.clear();
        return false;
    }

    outToken = ToHex(buf, lowerCase);
    return true;
}

std::string ToLower(std::string text)
{
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
        });
    return text;
}
