#include "HttpMessage.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <unordered_map>
#include <shlwapi.h>
#include <Urlmon.h>
#include "Util.h"

namespace
{
bool HeaderNameEquals(const std::string& left, const std::string& right)
{
    if (left.size() != right.size())
    {
        return false;
    }

    for (size_t i = 0; i < left.size(); ++i)
    {
        if (std::tolower(static_cast<unsigned char>(left[i])) !=
            std::tolower(static_cast<unsigned char>(right[i])))
        {
            return false;
        }
    }
    return true;
}
}

std::string GetStatusText(int statusCode) {
    static const std::unordered_map<int, std::string> statusMap = {
        // 1xx: Informational
        {100, "Continue"},
        {101, "Switching Protocols"},
        {102, "Processing"},

        // 2xx: Success
        {200, "OK"},
        {201, "Created"},
        {202, "Accepted"},
        {203, "Non-Authoritative Information"},
        {204, "No Content"},
        {205, "Reset Content"},
        {206, "Partial Content"},
        {207, "Multi-Status"},
        {208, "Already Reported"},
        {226, "IM Used"},

        // 3xx: Redirection
        {300, "Multiple Choices"},
        {301, "Moved Permanently"},
        {302, "Found"},
        {303, "See Other"},
        {304, "Not Modified"},
        {305, "Use Proxy"},
        {307, "Temporary Redirect"},
        {308, "Permanent Redirect"},

        // 4xx: Client Error
        {400, "Bad Request"},
        {401, "Unauthorized"},
        {402, "Payment Required"},
        {403, "Forbidden"},
        {404, "Not Found"},
        {405, "Method Not Allowed"},
        {406, "Not Acceptable"},
        {407, "Proxy Authentication Required"},
        {408, "Request Timeout"},
        {409, "Conflict"},
        {410, "Gone"},
        {411, "Length Required"},
        {412, "Precondition Failed"},
        {413, "Payload Too Large"},
        {414, "URI Too Long"},
        {415, "Unsupported Media Type"},
        {416, "Range Not Satisfiable"},
        {417, "Expectation Failed"},
        {418, "I'm a teapot"},
        {421, "Misdirected Request"},
        {422, "Unprocessable Entity"},
        {423, "Locked"},
        {424, "Failed Dependency"},
        {425, "Too Early"},
        {426, "Upgrade Required"},
        {428, "Precondition Required"},
        {429, "Too Many Requests"},
        {431, "Request Header Fields Too Large"},
        {451, "Unavailable For Legal Reasons"},

        // 5xx: Server Error
        {500, "Internal Server Error"},
        {501, "Not Implemented"},
        {502, "Bad Gateway"},
        {503, "Service Unavailable"},
        {504, "Gateway Timeout"},
        {505, "HTTP Version Not Supported"},
        {506, "Variant Also Negotiates"},
        {507, "Insufficient Storage"},
        {508, "Loop Detected"},
        {510, "Not Extended"},
        {511, "Network Authentication Required"}
    };

    auto it = statusMap.find(statusCode);
    if (it != statusMap.end()) {
        return it->second;
    }

    return "Internal Server Error";
}

void HttpResponse::AddHeader(const std::string& name, const std::string& value)
{
    if (HasHeader(name))
    {
        repeatedHeaders.emplace_back(name, value);
        return;
    }

    headers.emplace(name, value);
}

void HttpResponse::RemoveHeader(const std::string& name)
{
    for (auto it = headers.begin(); it != headers.end();)
    {
        if (HeaderNameEquals(it->first, name))
        {
            it = headers.erase(it);
        }
        else
        {
            ++it;
        }
    }

    repeatedHeaders.erase(std::remove_if(repeatedHeaders.begin(), repeatedHeaders.end(),
        [&name](const auto& header)
        {
            return HeaderNameEquals(header.first, name);
        }), repeatedHeaders.end());
}

bool HttpResponse::HasHeader(const std::string& name) const
{
    for (const auto& header : headers)
    {
        if (HeaderNameEquals(header.first, name))
        {
            return true;
        }
    }

    for (const auto& header : repeatedHeaders)
    {
        if (HeaderNameEquals(header.first, name))
        {
            return true;
        }
    }

    return false;
}

std::string HttpResponse::GetHeader(const std::string& name) const
{
    for (const auto& header : headers)
    {
        if (HeaderNameEquals(header.first, name))
        {
            return header.second;
        }
    }

    for (const auto& header : repeatedHeaders)
    {
        if (HeaderNameEquals(header.first, name))
        {
            return header.second;
        }
    }

    return "";
}

std::string HttpResponse::ToHeadersString(HttpResponseBodyMode bodyMode, bool closeConnection) const {
    std::stringstream ss;

    std::string finalStatusText = statusText;
    if (finalStatusText.empty() || (finalStatusText == "OK" && statusCode != 200)) {
        finalStatusText = GetStatusText(statusCode);
    }

    ss << version << " " << statusCode << " " << finalStatusText << "\r\n";

    if (bodyMode == HttpResponseBodyMode::ContentLength && !HasHeader("Content-Length")) {
        ss << "Content-Length: " << body.size() << "\r\n";
    }
    else if (bodyMode == HttpResponseBodyMode::Chunked && !HasHeader("Transfer-Encoding")) {
        ss << "Transfer-Encoding: chunked\r\n";
    }

    if (!HasHeader("Connection")) {
        if (closeConnection || version == "HTTP/1.0") {
            ss << "Connection: close\r\n";
        }
        else {
            ss << "Connection: keep-alive\r\n";
        }
    }

    if (!HasHeader("Content-Type") && !body.empty())
    {
        LPWSTR pwzMimeOut = NULL;
        HRESULT hr = FindMimeFromData(NULL, NULL, (void*)body.data(), body.size(), NULL, 0, &pwzMimeOut, 0);
        std::string mimeType = "text/html";
        if (SUCCEEDED(hr) && pwzMimeOut != NULL)
        {
            mimeType = Utf16ToUtf8(pwzMimeOut);
            CoTaskMemFree(pwzMimeOut);
        }

        ss << "Content-Type: " << mimeType << "\r\n";
    }

    for (const auto& header : headers) {
        ss << header.first << ": " << header.second << "\r\n";
    }
    for (const auto& header : repeatedHeaders) {
        ss << header.first << ": " << header.second << "\r\n";
    }

    ss << "\r\n";
    return ss.str();
}

std::string HttpResponse::ToString() const {
    const bool closeConnection = version == "HTTP/1.0" || statusCode >= 400;
    return ToHeadersString(HttpResponseBodyMode::ContentLength, closeConnection) + body;
}

std::string HttpRequest::GetPathWithoutQuery() const {
    size_t queryPos = path.find('?');
    if (queryPos != std::string::npos) {
        return path.substr(0, queryPos);
    }
    return path;
}

std::unordered_map<std::string, std::string> HttpRequest::GetQueryParams() const {
    std::unordered_map<std::string, std::string> params;

    auto queryPos = path.find('?');
    if (queryPos == std::string::npos) {
        return params;
    }

    std::string queryString = path.substr(queryPos + 1);
    std::istringstream iss(queryString);
    std::string pair;

    while (std::getline(iss, pair, '&')) {
        size_t equalPos = pair.find('=');
        if (equalPos != std::string::npos) {
            std::string key = pair.substr(0, equalPos);
            std::string value = pair.substr(equalPos + 1);
            params[key] = value;
        }
        else {
            params[pair] = "";
        }
    }

    return params;
}

std::string HttpRequest::GetQueryParam(const std::string& key) const {
    auto params = GetQueryParams();
    auto it = params.find(key);
    if (it != params.end()) {
        return it->second;
    }
    return "";
}

std::string URLDecodeA(const std::string& encodedUrl) {
    if (encodedUrl.empty()) {
        return "";
    }

    DWORD bufferSize = static_cast<DWORD>(encodedUrl.length()) + 1;
    std::string result(bufferSize, '\0');
    memcpy(result.data(), encodedUrl.c_str(), encodedUrl.length());
    HRESULT hr = UrlUnescapeA(result.data(), nullptr, &bufferSize, URL_UNESCAPE_INPLACE);
    if (FAILED(hr)) {
        return "URL 解码失败";
    }

    result.resize(strlen(result.c_str()));
    return result;
}
