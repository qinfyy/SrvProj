#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <vector>
#include <memory>
#include <atomic>
#include <functional>
#include <mutex>
#include <chrono>
#include "Logger.h"
#include "ThreadPool.h"

class HttpRequest;
class HttpResponse;

using RouteHandler = std::function<void(const HttpRequest&, HttpResponse&)>;

struct RouteEntry {
    std::string method;
    std::string pattern;
    RouteHandler handler;

    RouteEntry(const std::string& m, const std::string& p, RouteHandler h)
        : method(m), pattern(p), handler(std::move(h)) { }
};

class AccountServer {
public:
    AccountServer(const std::string& bindIp, uint16_t port);

    ~AccountServer();

    bool Start();

    void Stop();

    bool IsRunning() const { return mRunning.load(); }

    void EnableHttpLogging(bool enable) { mEnableHttpLogging = enable; }

    void EnableRegisteredLogging(bool enable) { mEnableRegisteredLogging = enable; }

    void EnableRequestLogging(bool enable) { mEnableRequestLogging = enable; }

    void SetLogLevel(LogLevel level) { mLogLevel = level; }

    void RegisterRoute(const std::string& method, const std::string& pattern, RouteHandler handler);

    void SetupRoutes();
private:
    void ListenThread();

    void ClientLoop(SOCKET clientSocket, sockaddr_in clientAddr);

    bool ParseHttpRequest(const std::string& buffer, size_t& consumed, HttpRequest& request);

    void SendResponse(SOCKET sock, const HttpResponse& resp);

    bool ShouldCloseConnection(int requestCount, bool connectionClose, const std::chrono::steady_clock::time_point& lastActivity) const;

    bool DispatchRoute(const HttpRequest& req, HttpResponse& resp);

    static bool MatchWildcard(const std::string& pattern, const std::string& str);

    std::string mBindIp;
    uint16_t mPort;
    SOCKET mListenSocket;
    std::atomic<bool> mRunning;
    std::atomic<bool> mEnableHttpLogging;
    std::atomic<bool> mEnableRegisteredLogging;
    std::atomic<bool> mEnableRequestLogging;
    LogLevel mLogLevel;

    std::thread mListenThread;
    ThreadPool mThreadPool;

    std::vector<RouteEntry> mRoutes;
    std::mutex mRoutesMutex;

    static constexpr int MAX_REQUESTS_PER_CONN = 100;
    static constexpr int KEEPALIVE_TIMEOUT_SEC = 15;
    static constexpr size_t RECV_BUFFER_SIZE = 65536;
};
