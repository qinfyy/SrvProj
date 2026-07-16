#include "AccountServer.h"

#include "HttpMessage.h"
#include "AccountController.h"
#include "Handlers/GatewayController.h"
#include "PaymentController.h"

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <algorithm>
#include <array>
#include <charconv>
#include <coroutine>
#include <cstring>
#include <exception>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace
{
    constexpr size_t ReceiveChunkSize = 4096;
    constexpr DWORD AcceptAddressLength = sizeof(sockaddr_in) + 16;
    constexpr DWORD AcceptBufferLength = AcceptAddressLength * 2;

    std::string GetClientAddr(const sockaddr* address)
    {
        if (address == nullptr || address->sa_family != AF_INET)
        {
            return "unknown";
        }

        const auto* ipv4 = reinterpret_cast<const sockaddr_in*>(address);
        char ip[INET_ADDRSTRLEN]{};
        if (inet_ntop(AF_INET, &ipv4->sin_addr, ip, sizeof(ip)) == nullptr)
        {
            return "unknown";
        }

        return std::string(ip) + ":" + std::to_string(ntohs(ipv4->sin_port));
    }
}

RouteHandler MakeBlockingRoute(SynchronousRouteHandler handler)
{
    return [handler = std::move(handler)](RouteContext& context, const HttpRequest& request,
        HttpResponseWriter& writer) -> AsyncTask<void>
        {
            HttpResponse response;
            co_await context.Runtime().RunBlocking([&request, &response, handler]
                {
                    handler(request, response);
                });
            co_await writer.WriteResponse(response);
        };
}

struct AccountServer::Connection
{
    Connection(SOCKET socket, std::string clientAddr)
        : mSocket(socket), mId(socket), mClientAddr(std::move(clientAddr)),
        mLastActivityMilliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count())
    {}

    void MarkActive()
    {
        mLastActivityMilliseconds.store(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
    }

    bool IsIdle(int64_t nowMilliseconds, int64_t timeoutMilliseconds) const
    {
        return nowMilliseconds - mLastActivityMilliseconds.load() >= timeoutMilliseconds;
    }

    void RequestClose()
    {
        bool expected = false;
        if (!mCloseRequested.compare_exchange_strong(expected, true))
        {
            return;
        }

        std::lock_guard<std::mutex> lock(mSocketMutex);
        if (mSocket != INVALID_SOCKET)
        {
            shutdown(mSocket, SD_BOTH);
            CancelIoEx(reinterpret_cast<HANDLE>(mSocket), nullptr);
        }
    }

    void CloseSocket()
    {
        std::lock_guard<std::mutex> lock(mSocketMutex);
        if (mSocket != INVALID_SOCKET)
        {
            closesocket(mSocket);
            mSocket = INVALID_SOCKET;
        }
    }

    SOCKET Socket() const
    {
        return mSocket;
    }

    SOCKET Id() const
    {
        return mId;
    }

    std::mutex mSocketMutex;
    SOCKET mSocket;
    SOCKET mId;
    std::string mClientAddr;
    std::atomic<bool> mCloseRequested = false;
    std::atomic<int64_t> mLastActivityMilliseconds;
    int mRequestCount = 0;
};

class AccountServer::ConnectionResponseWriter final : public HttpResponseWriter
{
public:
    ConnectionResponseWriter(AccountServer& server, std::shared_ptr<Connection> connection,
        const std::string& version, bool keepAlive)
        : mServer(server), mConnection(std::move(connection)), mVersion(version), mKeepAlive(keepAlive)
    {}

    AsyncTask<bool> WriteHeaders(const HttpResponse& response, HttpResponseBodyMode bodyMode) override
    {
        if (mStarted || mFailed)
        {
            co_return false;
        }

        if (bodyMode == HttpResponseBodyMode::Chunked && mVersion == "HTTP/1.0")
        {
            bodyMode = HttpResponseBodyMode::Close;
        }

        mStarted = true;
        mBodyMode = bodyMode;
        mCloseAfterResponse = !mKeepAlive || bodyMode == HttpResponseBodyMode::Close;
        mStatusCode = response.statusCode;

        HttpResponse outgoing = response;
        outgoing.version = mVersion;
        outgoing.RemoveHeader("Connection");
        outgoing.headers["Connection"] = mCloseAfterResponse ? "close" : "keep-alive";
        if (bodyMode == HttpResponseBodyMode::Chunked)
        {
            outgoing.RemoveHeader("Content-Length");
            outgoing.RemoveHeader("Transfer-Encoding");
        }
        else if (bodyMode == HttpResponseBodyMode::Close)
        {
            outgoing.RemoveHeader("Content-Length");
            outgoing.RemoveHeader("Transfer-Encoding");
        }
        else
        {
            outgoing.RemoveHeader("Transfer-Encoding");
        }

        const std::string headerData = outgoing.ToHeadersString(bodyMode, mCloseAfterResponse);
        const bool sent = co_await mServer.SendAll(mConnection, headerData.data(), headerData.size());
        if (!sent)
        {
            Abort();
        }
        co_return sent;
    }

    AsyncTask<bool> WriteData(const char* data, size_t length) override
    {
        if (!mStarted || mFinished || mFailed)
        {
            co_return false;
        }

        if (length == 0)
        {
            co_return true;
        }

        if (mBodyMode != HttpResponseBodyMode::Chunked)
        {
            const bool sent = co_await mServer.SendAll(mConnection, data, length);
            if (!sent)
            {
                Abort();
            }
            co_return sent;
        }

        std::array<char, 32> lengthBuffer{};
        const std::to_chars_result lengthResult = std::to_chars(lengthBuffer.data(),
            lengthBuffer.data() + lengthBuffer.size(), length, 16);
        if (lengthResult.ec != std::errc())
        {
            Abort();
            co_return false;
        }

        std::string chunk;
        chunk.reserve(static_cast<size_t>(lengthResult.ptr - lengthBuffer.data()) + length + 4);
        chunk.append(lengthBuffer.data(), lengthResult.ptr);
        chunk.append("\r\n");
        chunk.append(data, length);
        chunk.append("\r\n");

        const bool sent = co_await mServer.SendAll(mConnection, chunk.data(), chunk.size());
        if (!sent)
        {
            Abort();
        }
        co_return sent;
    }

    AsyncTask<bool> Finish() override
    {
        if (mFinished)
        {
            co_return !mFailed;
        }

        if (!mStarted || mFailed)
        {
            co_return false;
        }

        if (mBodyMode == HttpResponseBodyMode::Chunked)
        {
            const char endChunk[] = "0\r\n\r\n";
            if (!(co_await mServer.SendAll(mConnection, endChunk, sizeof(endChunk) - 1)))
            {
                Abort();
                co_return false;
            }
        }

        mFinished = true;
        co_return true;
    }

    void Abort() noexcept override
    {
        mFailed = true;
        mConnection->RequestClose();
    }

    bool UsesHttp11() const noexcept override
    {
        return mVersion != "HTTP/1.0";
    }

    bool HasStarted() const noexcept override
    {
        return mStarted;
    }

    bool IsFinished() const noexcept override
    {
        return mFinished;
    }

    int StatusCode() const noexcept override
    {
        return mStatusCode;
    }

    bool ShouldClose() const noexcept
    {
        return mCloseAfterResponse || mFailed;
    }

private:
    AccountServer& mServer;
    std::shared_ptr<Connection> mConnection;
    std::string mVersion;
    bool mKeepAlive = false;
    bool mStarted = false;
    bool mFinished = false;
    bool mFailed = false;
    bool mCloseAfterResponse = false;
    int mStatusCode = 0;
    HttpResponseBodyMode mBodyMode = HttpResponseBodyMode::ContentLength;
};

AccountServer::AccountServer(const std::string& bindIp, uint16_t port)
    : mBindIp(bindIp), mPort(port), mListenSocket(INVALID_SOCKET),
    mRunning(false), mEnableHttpLogging(false),
    mEnableRegisteredLogging(true), mEnableRequestLogging(true),
    mLogLevel(LogLevel::Info)
{
    SetupRoutes();
}

AccountServer::~AccountServer()
{
    Stop();
}

void AccountServer::RegisterRoute(const std::string& method, const std::string& pattern, RouteHandler handler)
{
    std::lock_guard<std::mutex> lock(mRoutesMutex);
    mRoutes.emplace_back(method, pattern, std::move(handler));
    if (mEnableRegisteredLogging && mLogLevel <= LogLevel::Info)
    {
        LOG_INFO("Registered route: {} {}", method, pattern);
    }
}

void AccountServer::SetupRoutes()
{
    RegisterRoute("GET", "/", [](RouteContext&, const HttpRequest&, HttpResponseWriter& writer) -> AsyncTask<void>
        {
            HttpResponse response;
            response.statusCode = 200;
            response.body = "Hello World";
            co_await writer.WriteResponse(response);
        });

    RegisterRoute("GET", "/meta/serverlist.html", ServerListHandler);
    RegisterRoute("GET", "/notice/*", NoticeListHandler);
    RegisterRoute("POST", "/user/quick-login", QuickLoginHandler);
    RegisterRoute("POST", "/user/login", LoginHandler);
    RegisterRoute("POST", "/user/detail", DetailHandler);
    RegisterRoute("POST", "/yostar/send-code", SmsHandler);
    RegisterRoute("POST", "/yostar/get-auth", AuthHandler);
    RegisterRoute("POST", "/common/version", VersionHandler);
    RegisterRoute("POST", "/common/config", CommonConfigHandler);
    RegisterRoute("POST", "/agent-zone-1/", AgentHandler);
    RegisterRoute("POST", "/order/products", MakeBlockingRoute(OrderProductsHandler));
    RegisterRoute("POST", "/order/create", MakeBlockingRoute(OrderCreateHandler));
    RegisterRoute("POST", "/order/notify", MakeBlockingRoute(OrderNotifyHandler));
    RegisterRoute("GET", "/mock-pay", MakeBlockingRoute(MockPayPageHandler));

    if (mEnableRegisteredLogging && mLogLevel <= LogLevel::Info)
    {
        LOG_INFO("Route setup completed, total routes: {}", mRoutes.size());
    }
}

AsyncTask<void> AccountServer::DispatchRoute(RouteContext& context, const HttpRequest& req, HttpResponseWriter& writer)
{
    const std::string pathWithoutQuery = req.GetPathWithoutQuery();
    const std::string method = req.method;
    RouteHandler handler;

    {
        std::lock_guard<std::mutex> lock(mRoutesMutex);
        for (const auto& route : mRoutes)
        {
            if (route.method == method && MatchWildcard(route.pattern, pathWithoutQuery))
            {
                handler = route.handler;
                break;
            }
        }
    }

    if (handler)
    {
        co_await handler(context, req, writer);
    }
    else
    {
        HttpResponse response;
        response.statusCode = 404;
        response.statusText.clear();
        response.headers["Content-Type"] = "application/json";
        response.body = R"({"code":404,"error":"Not Found"})";
        co_await writer.WriteResponse(response);
    }

    if (mEnableRequestLogging && mLogLevel <= LogLevel::Info)
    {
        LOG_INFO("Client request: {} {} - {}", method, pathWithoutQuery, writer.StatusCode());
    }

    co_return;
}

bool AccountServer::Start()
{
    if (mRunning.exchange(true))
    {
        if (mLogLevel <= LogLevel::Error)
        {
            LOG_ERROR("Server is already running");
        }
        return false;
    }

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        mRunning = false;
        if (mLogLevel <= LogLevel::Error)
        {
            LOG_ERROR("WSAStartup failed");
        }
        return false;
    }

    const size_t hardwareThreads = std::thread::hardware_concurrency();
    const size_t workerCount = std::max<size_t>(hardwareThreads, hardwareThreads * 4);

    try
    {
        mIocpAwaiter = std::make_unique<IOCPAwaiter>(workerCount, workerCount);
        mHttpClient = std::make_unique<HttpClient>(*mIocpAwaiter);
    }
    catch (const std::exception& e)
    {
        mRunning = false;
        LOG_ERROR("Failed to create AccountServer IOCP runtime: {}", e.what());
        mHttpClient.reset();
        mIocpAwaiter.reset();
        WSACleanup();
        return false;
    }

    SOCKET listenSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
    if (listenSocket == INVALID_SOCKET)
    {
        LOG_ERROR("WSASocket() failed, error: {}", WSAGetLastError());
        mRunning = false;
        mHttpClient.reset();
        mIocpAwaiter.reset();
        WSACleanup();
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    if (inet_pton(AF_INET, mBindIp.c_str(), &addr.sin_addr) != 1)
    {
        LOG_ERROR("Invalid bind address: {}", mBindIp);
        closesocket(listenSocket);
        mRunning = false;
        mHttpClient.reset();
        mIocpAwaiter.reset();
        WSACleanup();
        return false;
    }
    addr.sin_port = htons(mPort);

    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) == SOCKET_ERROR)
    {
        LOG_ERROR("bind() failed, error: {}", WSAGetLastError());
        closesocket(listenSocket);
        mRunning = false;
        mHttpClient.reset();
        mIocpAwaiter.reset();
        WSACleanup();
        return false;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        LOG_ERROR("listen() failed, error: {}", WSAGetLastError());
        closesocket(listenSocket);
        mRunning = false;
        mHttpClient.reset();
        mIocpAwaiter.reset();
        WSACleanup();
        return false;
    }

    if (!mIocpAwaiter->AssociateSocket(listenSocket))
    {
        LOG_ERROR("Failed to associate listening socket with IOCP, error: {}", GetLastError());
        closesocket(listenSocket);
        mRunning = false;
        mHttpClient.reset();
        mIocpAwaiter.reset();
        WSACleanup();
        return false;
    }

    DWORD bytes = 0;
    GUID acceptExGuid = WSAID_ACCEPTEX;
    if (WSAIoctl(listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &acceptExGuid, sizeof(acceptExGuid), &mAcceptEx, sizeof(mAcceptEx), &bytes, nullptr, nullptr) == SOCKET_ERROR)
    {
        LOG_ERROR("Failed to load AcceptEx, error: {}", WSAGetLastError());
        closesocket(listenSocket);
        mRunning = false;
        mHttpClient.reset();
        mIocpAwaiter.reset();
        WSACleanup();
        return false;
    }

    GUID getAcceptExSockaddrsGuid = WSAID_GETACCEPTEXSOCKADDRS;
    if (WSAIoctl(listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &getAcceptExSockaddrsGuid,
        sizeof(getAcceptExSockaddrsGuid), &mGetAcceptExSockaddrs, sizeof(mGetAcceptExSockaddrs),
        &bytes, nullptr, nullptr) == SOCKET_ERROR)
    {
        LOG_ERROR("Failed to load GetAcceptExSockaddrs, error: {}", WSAGetLastError());
        closesocket(listenSocket);
        mRunning = false;
        mHttpClient.reset();
        mIocpAwaiter.reset();
        WSACleanup();
        return false;
    }

    mListenSocket = listenSocket;
    const size_t acceptDepth = workerCount * 2;
    size_t acceptedOperations = 0;
    for (size_t i = 0; i < acceptDepth; ++i)
    {
        if (StartAccept())
        {
            ++acceptedOperations;
        }
    }

    if (acceptedOperations == 0)
    {
        LOG_ERROR("Failed to post any AcceptEx operation");
        Stop();
        return false;
    }

    if (!StartIdleTimer())
    {
        Stop();
        return false;
    }

    if (mLogLevel <= LogLevel::Info)
    {
        LOG_INFO("AccountServer listening on {}:{} with {} IOCP workers", mBindIp, mPort, workerCount);
    }

    return true;
}

bool AccountServer::StartAccept()
{
    if (!mRunning.load())
    {
        return false;
    }

    const SOCKET listenSocket = mListenSocket.load();
    if (listenSocket == INVALID_SOCKET || mIocpAwaiter == nullptr || mAcceptEx == nullptr)
    {
        return false;
    }

    SOCKET clientSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
    if (clientSocket == INVALID_SOCKET)
    {
        if (mRunning && mLogLevel <= LogLevel::Error)
        {
            LOG_ERROR("WSASocket() for AcceptEx failed, error: {}", WSAGetLastError());
        }
        return false;
    }

    mPendingAccepts.fetch_add(1);
    AcceptLoop(listenSocket, clientSocket);
    return true;
}

DetachedTask AccountServer::AcceptLoop(SOCKET listenSocket, SOCKET clientSocket)
{
    std::array<char, AcceptBufferLength> addressBuffer{};

    try
    {
        const IOCPAwaiter::IoResult result = co_await mIocpAwaiter->Accept(mAcceptEx, listenSocket, clientSocket, addressBuffer.data(), static_cast<DWORD>(addressBuffer.size()));

        if (result && mRunning.load())
        {
            if (setsockopt(clientSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT,
                reinterpret_cast<const char*>(&listenSocket), sizeof(listenSocket)) == SOCKET_ERROR)
            {
                LOG_ERROR("SO_UPDATE_ACCEPT_CONTEXT failed, error: {}", WSAGetLastError());
                closesocket(clientSocket);
            }
            else if (!mIocpAwaiter->AssociateSocket(clientSocket))
            {
                LOG_ERROR("Failed to associate accepted socket with IOCP, error: {}", GetLastError());
                closesocket(clientSocket);
            }
            else
            {
                sockaddr* localAddress = nullptr;
                sockaddr* remoteAddress = nullptr;
                int localAddressLength = 0;
                int remoteAddressLength = 0;
                mGetAcceptExSockaddrs(addressBuffer.data(), 0, AcceptAddressLength, AcceptAddressLength,
                    &localAddress, &localAddressLength, &remoteAddress, &remoteAddressLength);

                auto connection = std::make_shared<Connection>(clientSocket, GetClientAddr(remoteAddress));
                if (AddConnection(connection))
                {
                    if (mEnableHttpLogging && mLogLevel <= LogLevel::Debug)
                    {
                        LOG_DEBUG("New connection from {}", connection->mClientAddr);
                    }
                    ConnectionLoop(connection);
                }
                else
                {
                    closesocket(clientSocket);
                }
            }
        }
        else
        {
            if (mRunning.load() && result.error != ERROR_OPERATION_ABORTED && mLogLevel <= LogLevel::Error)
            {
                LOG_ERROR("AcceptEx failed, error: {}", result.error);
            }
            closesocket(clientSocket);
        }
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Accept coroutine failed: {}", e.what());
        closesocket(clientSocket);
    }

    if (mRunning.load())
    {
        StartAccept();
    }
    FinishAccept();
    co_return;
}

bool AccountServer::AddConnection(const std::shared_ptr<Connection>& connection)
{
    std::lock_guard<std::mutex> lock(mConnectionsMutex);
    if (!mRunning.load())
    {
        return false;
    }

    return mConnections.emplace(connection->Id(), connection).second;
}

void AccountServer::RemoveConnection(SOCKET socket)
{
    {
        std::lock_guard<std::mutex> lock(mConnectionsMutex);
        mConnections.erase(socket);
    }
    mConnectionsCondition.notify_all();
}

void AccountServer::FinishAccept()
{
    mPendingAccepts.fetch_sub(1);
    mConnectionsCondition.notify_all();
}

DetachedTask AccountServer::ConnectionLoop(std::shared_ptr<Connection> connection)
{
    std::array<char, ReceiveChunkSize> receiveBuffer{};
    std::string requestBuffer;
    requestBuffer.reserve(RECV_BUFFER_SIZE);
    RouteContext routeContext(*mIocpAwaiter, *mHttpClient);

    try
    {
        while (mRunning.load() && !connection->mCloseRequested.load())
        {
            while (!connection->mCloseRequested.load() && !requestBuffer.empty())
            {
                HttpRequest request;
                size_t consumed = 0;
                bool parsed = false;
                try
                {
                    parsed = ParseHttpRequest(requestBuffer, consumed, request);
                }
                catch (const std::exception& e)
                {
                    LOG_ERROR("Invalid HTTP request from {}: {}", connection->mClientAddr, e.what());
                    connection->RequestClose();
                    break;
                }

                if (!parsed)
                {
                    break;
                }

                requestBuffer.erase(0, consumed);
                ++connection->mRequestCount;
                connection->MarkActive();

                const auto connectionHeader = request.headers.find("Connection");
                bool connectionClose = false;
                if (request.version == "HTTP/1.0")
                {
                    connectionClose = connectionHeader == request.headers.end() || connectionHeader->second != "keep-alive";
                }
                else
                {
                    connectionClose = connectionHeader != request.headers.end() && connectionHeader->second == "close";
                }

                const bool keepAlive = !ShouldCloseConnection(connection->mRequestCount, connectionClose);
                ConnectionResponseWriter writer(*this, connection, request.version, keepAlive);
                bool writeServerError = false;
                try
                {
                    co_await DispatchRoute(routeContext, request, writer);
                }
                catch (const std::exception& e)
                {
                    if (!mRunning.load() || connection->mCloseRequested.load())
                    {
                        break;
                    }

                    LOG_ERROR("Route handler failed for {}: {}", connection->mClientAddr, e.what());
                    if (writer.HasStarted())
                    {
                        writer.Abort();
                    }
                    else
                    {
                        writeServerError = true;
                    }
                }

                if (writeServerError)
                {
                    HttpResponse response;
                    response.version = request.version;
                    response.statusCode = 500;
                    response.statusText.clear();
                    response.headers["Content-Type"] = "application/json";
                    response.body = R"({"code":500,"error":"Internal Server Error"})";
                    co_await writer.WriteResponse(response);
                }

                if (!mRunning.load() || connection->mCloseRequested.load())
                {
                    break;
                }

                if (!writer.HasStarted())
                {
                    HttpResponse response;
                    response.version = request.version;
                    response.statusCode = 500;
                    response.statusText.clear();
                    response.headers["Content-Type"] = "application/json";
                    response.body = R"({"code":500,"error":"Route did not produce a response"})";
                    co_await writer.WriteResponse(response);
                }

                if (writer.HasStarted() && !writer.IsFinished())
                {
                    if (!(co_await writer.Finish()))
                    {
                        writer.Abort();
                        break;
                    }
                }

                if (!keepAlive || writer.ShouldClose() || connection->mCloseRequested.load())
                {
                    connection->RequestClose();
                    break;
                }
            }

            if (!mRunning.load() || connection->mCloseRequested.load())
            {
                break;
            }

            if (requestBuffer.size() >= RECV_BUFFER_SIZE)
            {
                if (mLogLevel <= LogLevel::Error)
                {
                    LOG_ERROR("Buffer full for {}, closing", connection->mClientAddr);
                }
                connection->RequestClose();
                break;
            }

            const size_t available = RECV_BUFFER_SIZE - requestBuffer.size();
            const ULONG receiveLength = static_cast<ULONG>(std::min<size_t>(available, receiveBuffer.size()));
            const IOCPAwaiter::IoResult receiveResult = co_await mIocpAwaiter->Receive(
                connection->Socket(), receiveBuffer.data(), receiveLength);
            if (!receiveResult || receiveResult.bytes == 0)
            {
                if (mEnableHttpLogging && mLogLevel <= LogLevel::Debug && !connection->mCloseRequested.load())
                {
                    LOG_DEBUG("Client {} disconnected", connection->mClientAddr);
                }
                break;
            }

            requestBuffer.append(receiveBuffer.data(), receiveResult.bytes);
        }
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Connection coroutine failed for {}: {}", connection->mClientAddr, e.what());
    }

    connection->CloseSocket();
    RemoveConnection(connection->Id());
    if (mEnableHttpLogging && mLogLevel <= LogLevel::Debug)
    {
        LOG_DEBUG("Closed connection {}", connection->mClientAddr);
    }
    co_return;
}

AsyncTask<bool> AccountServer::SendAll(const std::shared_ptr<Connection>& connection, const char* data, size_t length)
{
    size_t sent = 0;
    while (sent < length)
    {
        const size_t remaining = length - sent;
        const ULONG sendLength = static_cast<ULONG>(std::min<size_t>(
            remaining, static_cast<size_t>(std::numeric_limits<ULONG>::max())));
        const IOCPAwaiter::IoResult sendResult = co_await mIocpAwaiter->Send(
            connection->Socket(), data + sent, sendLength);
        if (!sendResult || sendResult.bytes == 0)
        {
            if (mRunning.load() && !connection->mCloseRequested.load() && mLogLevel <= LogLevel::Error)
            {
                LOG_ERROR("WSASend failed for {}, error: {}", connection->mClientAddr, sendResult.error);
            }
            co_return false;
        }

        sent += sendResult.bytes;
        connection->MarkActive();
    }

    co_return true;
}

bool AccountServer::StartIdleTimer()
{
    if (CreateTimerQueueTimer(&mIdleTimer, nullptr, IdleTimerCallback, this, 1000, 1000, WT_EXECUTEDEFAULT))
    {
        return true;
    }

    LOG_ERROR("CreateTimerQueueTimer failed, error: {}", GetLastError());
    return false;
}

void AccountServer::StopIdleTimer()
{
    HANDLE timer = mIdleTimer;
    if (timer != nullptr)
    {
        mIdleTimer = nullptr;
        DeleteTimerQueueTimer(nullptr, timer, INVALID_HANDLE_VALUE);
    }
}

VOID CALLBACK AccountServer::IdleTimerCallback(PVOID context, BOOLEAN)
{
    static_cast<AccountServer*>(context)->SweepIdleConnections();
}

void AccountServer::SweepIdleConnections()
{
    if (!mRunning.load())
    {
        return;
    }

    const int64_t nowMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    const int64_t timeoutMilliseconds = static_cast<int64_t>(KEEPALIVE_TIMEOUT_SEC) * 1000;
    std::vector<std::shared_ptr<Connection>> idleConnections;

    {
        std::lock_guard<std::mutex> lock(mConnectionsMutex);
        for (const auto& [socket, connection] : mConnections)
        {
            if (connection->IsIdle(nowMilliseconds, timeoutMilliseconds))
            {
                idleConnections.push_back(connection);
            }
        }
    }

    for (const std::shared_ptr<Connection>& connection : idleConnections)
    {
        connection->RequestClose();
    }
}

void AccountServer::WaitForConnectionsToClose()
{
    std::unique_lock<std::mutex> lock(mConnectionsMutex);
    mConnectionsCondition.wait(lock, [this]
        {
            return mConnections.empty() && mPendingAccepts.load() == 0;
        });
}

void AccountServer::Stop()
{
    if (!mRunning.exchange(false))
    {
        return;
    }

    if (mLogLevel <= LogLevel::Info)
    {
        LOG_INFO("Stopping AccountServer...");
    }

    StopIdleTimer();

    const SOCKET listenSocket = mListenSocket.exchange(INVALID_SOCKET);
    if (listenSocket != INVALID_SOCKET)
    {
        CancelIoEx(reinterpret_cast<HANDLE>(listenSocket), nullptr);
        closesocket(listenSocket);
    }

    std::vector<std::shared_ptr<Connection>> connections;
    {
        std::lock_guard<std::mutex> lock(mConnectionsMutex);
        connections.reserve(mConnections.size());
        for (const auto& [socket, connection] : mConnections)
        {
            connections.push_back(connection);
        }
    }

    for (const std::shared_ptr<Connection>& connection : connections)
    {
        connection->RequestClose();
    }

    if (mHttpClient != nullptr)
    {
        mHttpClient->CancelAll();
    }

    WaitForConnectionsToClose();

    if (mHttpClient != nullptr)
    {
        mHttpClient->WaitForIdle();
        mHttpClient.reset();
    }

    if (mIocpAwaiter != nullptr)
    {
        mIocpAwaiter->StopAcceptingWork();
        mIocpAwaiter->WaitForIdle();
        mIocpAwaiter->Shutdown();
        mIocpAwaiter.reset();
    }

    mAcceptEx = nullptr;
    mGetAcceptExSockaddrs = nullptr;
    WSACleanup();

    if (mLogLevel <= LogLevel::Info)
    {
        LOG_INFO("AccountServer stopped");
    }
}

bool AccountServer::ParseHttpRequest(const std::string& buffer, size_t& consumed, HttpRequest& request)
{
    const size_t headerEnd = buffer.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
    {
        return false;
    }

    const size_t lineEnd = buffer.find("\r\n");
    if (lineEnd == std::string::npos || lineEnd > headerEnd)
    {
        return false;
    }

    const std::string requestLine = buffer.substr(0, lineEnd);
    std::istringstream lineStream(requestLine);
    lineStream >> request.method >> request.path >> request.version;
    if (request.method.empty() || request.path.empty())
    {
        return false;
    }

    size_t headerStart = lineEnd + 2;
    while (headerStart < headerEnd)
    {
        const size_t eol = buffer.find("\r\n", headerStart);
        if (eol == std::string::npos || eol > headerEnd)
        {
            break;
        }

        const std::string headerLine = buffer.substr(headerStart, eol - headerStart);
        const size_t colon = headerLine.find(':');
        if (colon != std::string::npos)
        {
            const std::string key = headerLine.substr(0, colon);
            std::string value = headerLine.substr(colon + 1);
            const size_t valueStart = value.find_first_not_of(" \t");
            if (valueStart == std::string::npos)
            {
                value.clear();
            }
            else
            {
                const size_t valueEnd = value.find_last_not_of(" \t");
                value = value.substr(valueStart, valueEnd - valueStart + 1);
            }
            request.headers[key] = std::move(value);
        }

        headerStart = eol + 2;
    }

    const size_t headerTotalLength = headerEnd + 4;
    size_t contentLength = 0;
    const auto contentLengthIt = request.headers.find("Content-Length");
    if (contentLengthIt != request.headers.end())
    {
        const unsigned long long parsedContentLength = std::stoull(contentLengthIt->second);
        if (parsedContentLength > RECV_BUFFER_SIZE)
        {
            return false;
        }
        contentLength = static_cast<size_t>(parsedContentLength);
    }

    if (buffer.size() < headerTotalLength + contentLength)
    {
        return false;
    }

    if (contentLength > 0)
    {
        request.body = buffer.substr(headerTotalLength, contentLength);
    }

    consumed = headerTotalLength + contentLength;
    return true;
}

bool AccountServer::ShouldCloseConnection(int requestCount, bool connectionClose) const
{
    return connectionClose || requestCount >= MAX_REQUESTS_PER_CONN;
}

bool AccountServer::MatchWildcard(const std::string& pattern, const std::string& str)
{
    size_t patternPos = 0;
    size_t strPos = 0;
    const size_t patternLength = pattern.length();
    const size_t strLength = str.length();

    size_t starPos = std::string::npos;
    size_t matchPos = 0;

    while (strPos < strLength)
    {
        if (patternPos < patternLength &&
            (pattern[patternPos] == '?' || pattern[patternPos] == str[strPos]))
        {
            ++patternPos;
            ++strPos;
        }
        else if (patternPos < patternLength && pattern[patternPos] == '*')
        {
            starPos = patternPos;
            matchPos = strPos;
            ++patternPos;
        }
        else if (starPos != std::string::npos)
        {
            patternPos = starPos + 1;
            strPos = ++matchPos;
        }
        else
        {
            return false;
        }
    }

    while (patternPos < patternLength && pattern[patternPos] == '*')
    {
        ++patternPos;
    }

    return patternPos == patternLength;
}
