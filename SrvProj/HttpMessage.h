#pragma once
#include <string>
#include <unordered_map>

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
    std::string body;

    std::string ToString() const;
};

std::string URLDecodeA(const std::string& encodedUrl);
