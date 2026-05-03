#include "AccountServer.h"
#include "HttpMessage.h"
#include "AccountController.h"
#include "Handlers/GatewayController.h"

static std::string GetClientAddr(sockaddr_in addr) {
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ip, sizeof(ip));
    uint16_t port = ntohs(addr.sin_port);
    return std::string(ip) + ":" + std::to_string(port);
}

AccountServer::AccountServer(const std::string& bindIp, uint16_t port)
    : mBindIp(bindIp), mPort(port), mListenSocket(INVALID_SOCKET),
    mRunning(false), mEnableHttpLogging(false),
    mEnableRegisteredLogging(true), mEnableRequestLogging(true),
    mLogLevel(LogLevel::Info),
    mThreadPool(std::thread::hardware_concurrency() * 16) 
{
    SetupRoutes();
}

AccountServer::~AccountServer() {
    Stop();
}

void AccountServer::RegisterRoute(const std::string& method, const std::string& pattern, RouteHandler handler) {
    std::lock_guard<std::mutex> lock(mRoutesMutex);
    mRoutes.emplace_back(method, pattern, std::move(handler));
    if (mEnableRegisteredLogging && mLogLevel <= LogLevel::Info) {
        LOG_INFO("Registered route: {} {}", method, pattern);
    }
}

void AccountServer::SetupRoutes() {
    RegisterRoute("GET", "/", [](const HttpRequest&, HttpResponse& rsp) {
        rsp.statusCode = 200;
        rsp.body = "Hello World";
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
    RegisterRoute("POST", "/agent-zone-1", AgentHandler);

    if (mEnableRegisteredLogging && mLogLevel <= LogLevel::Info) {
        LOG_INFO("Route setup completed, total routes: {}", mRoutes.size());
    }
}

bool AccountServer::DispatchRoute(const HttpRequest& req, HttpResponse& resp) {
    std::string pathWithoutQuery = req.GetPathWithoutQuery();
    std::string method = req.method;
    int statusCode = 500;
    bool found = false;

    {
        std::lock_guard<std::mutex> lock(mRoutesMutex);
        for (const auto& route : mRoutes) {
            if (route.method != method) {
                continue;
            }
            if (MatchWildcard(route.pattern, pathWithoutQuery)) {
                route.handler(req, resp);
                statusCode = resp.statusCode;
                found = true;
                break;
            }
        }
    }

    if (!found) {
        resp.statusCode = 404;
        resp.headers["Content-Type"] = "application/json";
        resp.body = R"({"code":404,"error":"Not Found"})";
        statusCode = 404;
    }

    if (mEnableRequestLogging  && mLogLevel <= LogLevel::Info) {
        LOG_INFO("Client request: {} {} - {}", method, pathWithoutQuery, statusCode);
    }

    return found;
}

bool AccountServer::Start() {
    if (mRunning) {
        if (mLogLevel <= LogLevel::Error) {
            LOG_ERROR("Server is already running");
        }
        return false;
    }

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        if (mLogLevel <= LogLevel::Error) {
            LOG_ERROR("WSAStartup failed");
        }
        return false;
    }

    mListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mListenSocket == INVALID_SOCKET) {
        if (mLogLevel <= LogLevel::Error) {
            LOG_ERROR("socket() failed, error: {}", WSAGetLastError());
        }
        WSACleanup();
        return false;
    }

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, mBindIp.c_str(), &addr.sin_addr);
    addr.sin_port = htons(mPort);

    if (bind(mListenSocket, (sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        if (mLogLevel <= LogLevel::Error) {
            LOG_ERROR("bind() failed, error: {}", WSAGetLastError());
        }
        closesocket(mListenSocket);
        WSACleanup();
        return false;
    }

    if (listen(mListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        if (mLogLevel <= LogLevel::Error) {
            LOG_ERROR("listen() failed, error: {}", WSAGetLastError());
        }
        closesocket(mListenSocket);
        WSACleanup();
        return false;
    }

    if (mLogLevel <= LogLevel::Info) {
        LOG_INFO("AccountServer listening on {}:{}", mBindIp, mPort);
    }

    mRunning = true;
    mListenThread = std::thread(&AccountServer::ListenThread, this);

    return true;
}

void AccountServer::ListenThread() {
    while (mRunning) {
        sockaddr_in clientAddr;
        int addrLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(mListenSocket, (sockaddr*)&clientAddr, &addrLen);

        if (clientSocket == INVALID_SOCKET) {
            if (mRunning) {
                if (mLogLevel <= LogLevel::Error) {
                    LOG_ERROR("accept() failed, error: {}", WSAGetLastError());
                }
            }
            continue;
        }

        if (mEnableHttpLogging && mLogLevel <= LogLevel::Debug) {
            LOG_DEBUG("New connection from {}", GetClientAddr(clientAddr));
        }

        mThreadPool.enqueue([this, clientSocket, clientAddr]() {
                ClientLoop(clientSocket, clientAddr);
            });
    }
}

void AccountServer::Stop() {
    if (!mRunning) {
        return;
    }

    if (mLogLevel <= LogLevel::Info) {
        LOG_INFO("Stopping AccountServer...");
    }
    mRunning = false;

    if (mListenSocket != INVALID_SOCKET) {
        shutdown(mListenSocket, SD_BOTH);
        closesocket(mListenSocket);
        mListenSocket = INVALID_SOCKET;
    }

    if (mListenThread.joinable()) {
        mListenThread.join();
    }

    mThreadPool.waitAll();
    WSACleanup();

    if (mLogLevel <= LogLevel::Info) {
        LOG_INFO("AccountServer stopped");
    }
}

void AccountServer::ClientLoop(SOCKET clientSocket, sockaddr_in clientAddr) {
    std::string clientAddrStr = GetClientAddr(clientAddr);
    int requestCount = 0;
    bool connectionClose = false;
    auto lastActivity = std::chrono::steady_clock::now();

    std::string recvBuffer;
    recvBuffer.reserve(RECV_BUFFER_SIZE);
    size_t bufLen = 0;

    while (mRunning && !ShouldCloseConnection(requestCount, connectionClose, lastActivity)) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(clientSocket, &readfds);
        timeval tv = { 1, 0 };
        int selRet = select(0, &readfds, nullptr, nullptr, &tv);

        if (selRet == SOCKET_ERROR) {
            if (mLogLevel <= LogLevel::Error) {
                LOG_ERROR("select error on {}", clientAddrStr);
            }
            break;
        }
        if (selRet == 0) {
            continue;
        }

        if (bufLen >= RECV_BUFFER_SIZE) {
            if (mLogLevel <= LogLevel::Error) {
                LOG_ERROR("Buffer full for {}, closing", clientAddrStr);
            }
            break;
        }

        int addBuf = bufLen + 4096;
        size_t newSize = max(recvBuffer.size(), static_cast<size_t>(addBuf));
        recvBuffer.resize(newSize);
        int bytes = recv(clientSocket, recvBuffer.data() + bufLen, static_cast<int>(recvBuffer.size() - bufLen), 0);
        if (bytes <= 0) {
            if (mEnableHttpLogging && mLogLevel <= LogLevel::Debug) {
                LOG_DEBUG("Client {} disconnected", clientAddrStr);
            }
            break;
        }

        bufLen += bytes;

        size_t processed = 0;
        while (processed < bufLen) {
            HttpRequest req;
            size_t consumed = 0;
            std::string tempBuffer(recvBuffer.data() + processed, recvBuffer.data() + bufLen);
            if (!ParseHttpRequest(tempBuffer, consumed, req)) {
                break;
            }

            requestCount++;
            lastActivity = std::chrono::steady_clock::now();

            bool shouldCloseAfterThis = false;
            if (req.version == "HTTP/1.0") {
                auto it = req.headers.find("Connection");
                shouldCloseAfterThis = !(it != req.headers.end() && it->second == "keep-alive");
            }
            else {
                auto it = req.headers.find("Connection");
                shouldCloseAfterThis = (it != req.headers.end() && it->second == "close");
            }
            if (shouldCloseAfterThis) {
                connectionClose = true;
            }

            HttpResponse resp;
            resp.version = req.version;
            DispatchRoute(req, resp);

            bool keepAlive = !connectionClose && !ShouldCloseConnection(requestCount, connectionClose, lastActivity);
            resp.headers["Connection"] = keepAlive ? "keep-alive" : "close";
            SendResponse(clientSocket, resp);

            if (!keepAlive) {
                break;
            }

            processed += consumed;
        }

        if (processed > 0) {
            memmove(recvBuffer.data(), recvBuffer.data() + processed, bufLen - processed);
            bufLen -= processed;
        }
    }

    closesocket(clientSocket);
    if (mEnableHttpLogging && mLogLevel <= LogLevel::Debug) {
        LOG_DEBUG("Closed connection {}", clientAddrStr);
    }
}

bool AccountServer::ParseHttpRequest(const std::string& buffer, size_t& consumed, HttpRequest& request) {
    size_t headerEnd = buffer.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        return false;
    }

    size_t lineEnd = buffer.find("\r\n");
    if (lineEnd == std::string::npos || lineEnd > headerEnd) {
        return false;
    }

    std::string requestLine = buffer.substr(0, lineEnd);
    std::istringstream lineStream(requestLine);
    lineStream >> request.method >> request.path >> request.version;
    if (request.method.empty() || request.path.empty()) {
        return false;
    }

    size_t headerStart = lineEnd + 2;
    while (headerStart < headerEnd) {
        size_t eol = buffer.find("\r\n", headerStart);
        if (eol == std::string::npos || eol > headerEnd) {
            break;
        }

        std::string headerLine = buffer.substr(headerStart, eol - headerStart);
        size_t colon = headerLine.find(':');
        if (colon != std::string::npos) {
            std::string key = headerLine.substr(0, colon);
            std::string value = headerLine.substr(colon + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            request.headers[key] = value;
        }

        headerStart = eol + 2;
    }

    size_t headerTotalLen = headerEnd + 4;
    int contentLength = 0;
    auto it = request.headers.find("Content-Length");
    if (it != request.headers.end()) {
        contentLength = std::stoi(it->second);
    }

    if (buffer.size() < headerTotalLen + contentLength) {
        return false;
    }

    if (contentLength > 0) {
        request.body = buffer.substr(headerTotalLen, contentLength);
    }

    consumed = headerTotalLen + contentLength;
    return true;
}

void AccountServer::SendResponse(SOCKET sock, const HttpResponse& resp) {
    std::string responseStr = resp.ToString();
    const char* data = responseStr.data();
    int len = static_cast<int>(responseStr.size());
    int sent = 0;
    while (sent < len) {
        int ret = send(sock, data + sent, len - sent, 0);
        if (ret <= 0) {
            if (mLogLevel <= LogLevel::Error) {
                LOG_ERROR("send() failed, error: {}", WSAGetLastError());
            }
            break;
        }
        sent += ret;
    }
}

bool AccountServer::ShouldCloseConnection(int requestCount, bool connectionClose,
    const std::chrono::steady_clock::time_point& lastActivity) const {
    if (connectionClose) {
        return true;
    }

    if (requestCount >= MAX_REQUESTS_PER_CONN) {
        return true;
    }

    auto now = std::chrono::steady_clock::now();
    auto idleSeconds = std::chrono::duration_cast<std::chrono::seconds>(now - lastActivity).count();
    if (idleSeconds >= KEEPALIVE_TIMEOUT_SEC) {
        return true;
    }

    return false;
}

bool AccountServer::MatchWildcard(const std::string& pattern, const std::string& str) {
    auto patternPos = 0;
    auto strPos = 0;
    auto patternLen = pattern.length();
    auto strLen = str.length();

    auto starPos = std::string::npos;
    auto matchPos = 0;

    while (strPos < strLen) {
        if (patternPos < patternLen &&
            (pattern[patternPos] == '?' || pattern[patternPos] == str[strPos])) {
            ++patternPos;
            ++strPos;
        }
        else if (patternPos < patternLen && pattern[patternPos] == '*') {
            starPos = patternPos;
            matchPos = strPos;
            ++patternPos;
        }
        else if (starPos != std::string::npos) {
            patternPos = starPos + 1;
            strPos = ++matchPos;
        }
        else {
            return false;
        }
    }

    while (patternPos < patternLen && pattern[patternPos] == '*') {
        ++patternPos;
    }

    return patternPos == patternLen;
}
