#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include "IOCPAwaiter.h"
#include "HttpClient.h"
#include "Logger.h"

class HttpRequest;
class HttpResponse;
class HttpClient;

class RouteContext
{
public:
    RouteContext(IOCPAwaiter& iocpAwaiter, HttpClient& httpClient)
        : mIocpAwaiter(iocpAwaiter), mHttpClient(httpClient)
    {
    }

    IOCPAwaiter& Runtime() const
    {
        return mIocpAwaiter;
    }

    HttpClient& Http() const
    {
        return mHttpClient;
    }

private:
    IOCPAwaiter& mIocpAwaiter;
    HttpClient& mHttpClient;
};

using RouteHandler = std::function<AsyncTask<void>(RouteContext&, const HttpRequest&, HttpResponseWriter&)>;
using SynchronousRouteHandler = std::function<void(const HttpRequest&, HttpResponse&)>;

RouteHandler MakeBlockingRoute(SynchronousRouteHandler handler);

class RouteEntry {
public:
    std::string method;
    std::string pattern;
    RouteHandler handler;

    RouteEntry(const std::string& m, const std::string& p, RouteHandler h)
        : method(m), pattern(p), handler(std::move(h)) { }
};

class AccountServer {
public:
    // workerCount 为 0 时按 "auto" 公式（hardware * 4）计算
    AccountServer(const std::string& bindIp, uint16_t port, size_t workerCount = 0);

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
    struct Connection;
    class ConnectionResponseWriter;

    bool StartAccept();
    DetachedTask AcceptLoop(SOCKET listenSocket, SOCKET clientSocket);
    DetachedTask ConnectionLoop(std::shared_ptr<Connection> connection);

    bool AddConnection(const std::shared_ptr<Connection>& connection);
    void RemoveConnection(SOCKET socket);
    void FinishAccept();
    void WaitForConnectionsToClose();

    bool StartIdleTimer();
    void StopIdleTimer();
    void SweepIdleConnections();
    static VOID CALLBACK IdleTimerCallback(PVOID context, BOOLEAN timerOrWaitFired);

    bool ParseHttpRequest(const std::string& buffer, size_t& consumed, HttpRequest& request);
    bool ShouldCloseConnection(int requestCount, bool connectionClose) const;
    AsyncTask<void> DispatchRoute(RouteContext& context, const HttpRequest& req, HttpResponseWriter& writer);
    AsyncTask<bool> SendAll(const std::shared_ptr<Connection>& connection, const char* data, size_t length);

    static bool MatchWildcard(const std::string& pattern, const std::string& str);

    std::string mBindIp;
    uint16_t mPort;
    size_t mWorkerCount = 0;
    std::atomic<SOCKET> mListenSocket;
    std::atomic<bool> mRunning;
    std::atomic<bool> mEnableHttpLogging;
    std::atomic<bool> mEnableRegisteredLogging;
    std::atomic<bool> mEnableRequestLogging;
    LogLevel mLogLevel;

    std::unique_ptr<IOCPAwaiter> mIocpAwaiter;
    std::unique_ptr<HttpClient> mHttpClient;
    LPFN_ACCEPTEX mAcceptEx = nullptr;
    LPFN_GETACCEPTEXSOCKADDRS mGetAcceptExSockaddrs = nullptr;
    HANDLE mIdleTimer = nullptr;

    std::unordered_map<SOCKET, std::shared_ptr<Connection>> mConnections;
    std::mutex mConnectionsMutex;
    std::condition_variable mConnectionsCondition;
    std::atomic<size_t> mPendingAccepts = 0;

    std::vector<RouteEntry> mRoutes;
    std::mutex mRoutesMutex;

    static constexpr int MAX_REQUESTS_PER_CONN = 100;
    static constexpr int KEEPALIVE_TIMEOUT_SEC = 15;
    static constexpr size_t RECV_BUFFER_SIZE = 65536;
};
