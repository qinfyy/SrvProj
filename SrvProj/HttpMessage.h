#pragma once
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

enum class HttpResponseBodyMode
{
    ContentLength,
    Chunked,
    Close
};

class HttpRequest {
public:
    std::string method;
    std::string path;
    std::string version;
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    std::string GetPathWithoutQuery() const;
    std::unordered_map<std::string, std::string> GetQueryParams() const;
    std::string GetQueryParam(const std::string& key) const;
};

class HttpResponse {
public:
    std::string version = "HTTP/1.1";
    int statusCode = 200;
    std::string statusText = "OK";
    std::unordered_map<std::string, std::string> headers;
    std::vector<std::pair<std::string, std::string>> repeatedHeaders;
    std::string body;

    void AddHeader(const std::string& name, const std::string& value);
    void RemoveHeader(const std::string& name);
    bool HasHeader(const std::string& name) const;
    std::string GetHeader(const std::string& name) const;
    std::string ToHeadersString(HttpResponseBodyMode bodyMode, bool closeConnection) const;
    std::string ToString() const;
};

std::string URLDecodeA(const std::string& encodedUrl);
