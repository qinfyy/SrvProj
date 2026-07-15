#include "HttpClient.h"

#include "IOCPAwaiter.h"
#include "Util.h"

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <algorithm>
#include <array>
#include <charconv>
#include <cctype>
#include <limits>
#include <optional>
#include <stdexcept>
#include <unordered_set>

#pragma comment(lib, "winhttp.lib")

namespace
{
constexpr int RequestTimeoutMilliseconds = 5000;
constexpr size_t BufferedResponseLimit = 1024 * 1024;
constexpr size_t StreamBufferSize = 16 * 1024;

std::string ToLowerAscii(const std::string& value)
{
    std::string result = value;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char character)
        {
            return static_cast<char>(std::tolower(character));
        });
    return result;
}

void AddConnectionTokens(const std::string& value, std::unordered_set<std::string>& names)
{
    size_t begin = 0;
    while (begin < value.size())
    {
        const size_t comma = value.find(',', begin);
        const size_t end = comma == std::string::npos ? value.size() : comma;
        const size_t tokenBegin = value.find_first_not_of(" 	", begin);
        if (tokenBegin != std::string::npos && tokenBegin < end)
        {
            const size_t tokenEnd = value.find_last_not_of(" 	", end - 1);
            names.emplace(ToLowerAscii(value.substr(tokenBegin, tokenEnd - tokenBegin + 1)));
        }
        begin = end + 1;
    }
}

std::unordered_set<std::string> GetHopByHopHeaderNames()
{
    return {
        "connection",
        "keep-alive",
        "proxy-authenticate",
        "proxy-authorization",
        "te",
        "trailer",
        "transfer-encoding",
        "upgrade",
        "host",
        "content-length"
    };
}

bool QueryHeaderString(HINTERNET request, DWORD query, std::wstring& value)
{
    DWORD bytes = 0;
    if (WinHttpQueryHeaders(request, query, WINHTTP_HEADER_NAME_BY_INDEX, nullptr, &bytes,
        WINHTTP_NO_HEADER_INDEX))
    {
        value.clear();
        return true;
    }

    if (GetLastError() != ERROR_INSUFFICIENT_BUFFER || bytes == 0)
    {
        return false;
    }

    std::vector<wchar_t> buffer(bytes / sizeof(wchar_t));
    if (!WinHttpQueryHeaders(request, query, WINHTTP_HEADER_NAME_BY_INDEX, buffer.data(), &bytes,
        WINHTTP_NO_HEADER_INDEX))
    {
        return false;
    }

    value.assign(buffer.data());
    return true;
}

bool QueryStatusCode(HINTERNET request, DWORD& statusCode)
{
    DWORD bytes = sizeof(statusCode);
    return WinHttpQueryHeaders(request, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &bytes, WINHTTP_NO_HEADER_INDEX) == TRUE;
}

std::vector<std::pair<std::string, std::string>> ParseRawHeaders(HINTERNET request)
{
    std::wstring rawHeaders;
    if (!QueryHeaderString(request, WINHTTP_QUERY_RAW_HEADERS_CRLF, rawHeaders))
    {
        return {};
    }

    std::vector<std::pair<std::string, std::string>> headers;
    size_t lineStart = rawHeaders.find(L"\r\n");
    if (lineStart == std::wstring::npos)
    {
        return headers;
    }
    lineStart += 2;

    while (lineStart < rawHeaders.size())
    {
        const size_t lineEnd = rawHeaders.find(L"\r\n", lineStart);
        if (lineEnd == std::wstring::npos || lineEnd == lineStart)
        {
            break;
        }

        const std::wstring line = rawHeaders.substr(lineStart, lineEnd - lineStart);
        const size_t colon = line.find(L':');
        if (colon != std::wstring::npos)
        {
            size_t valueStart = colon + 1;
            while (valueStart < line.size() && (line[valueStart] == L' ' || line[valueStart] == L'\t'))
            {
                ++valueStart;
            }

            headers.emplace_back(Utf16ToUtf8(line.substr(0, colon)),
                Utf16ToUtf8(line.substr(valueStart)));
        }

        lineStart = lineEnd + 2;
    }

    return headers;
}

std::optional<uint64_t> ParseContentLength(const std::string& value)
{
    uint64_t result = 0;
    const char* begin = value.data();
    const char* end = begin + value.size();
    const std::from_chars_result parsed = std::from_chars(begin, end, result);
    if (parsed.ec != std::errc() || parsed.ptr != end)
    {
        return std::nullopt;
    }
    return result;
}

std::vector<std::pair<std::string, std::string>> BuildForwardHeaders(const HttpRequest& request)
{
    std::unordered_set<std::string> excluded = GetHopByHopHeaderNames();
    for (const auto& [name, value] : request.headers)
    {
        if (ToLowerAscii(name) == "connection")
        {
            AddConnectionTokens(value, excluded);
        }
    }

    std::vector<std::pair<std::string, std::string>> headers;
    bool hasUserAgent = false;
    for (const auto& [name, value] : request.headers)
    {
        const std::string lowerName = ToLowerAscii(name);
        if (excluded.contains(lowerName))
        {
            continue;
        }

        hasUserAgent = hasUserAgent || lowerName == "user-agent";
        headers.emplace_back(name, value);
    }

    if (!hasUserAgent)
    {
        headers.emplace_back("User-Agent", "UnityPlayer/2022.3.62f2 (UnityWebRequest/1.0, libcurl/8.10.1-DEV)");
    }

    return headers;
}

std::optional<uint64_t> PopulateResponseHeaders(HINTERNET request, HttpResponse& response)
{
    DWORD statusCode = 502;
    QueryStatusCode(request, statusCode);
    response.statusCode = static_cast<int>(statusCode);
    response.statusText.clear();

    std::wstring statusText;
    if (QueryHeaderString(request, WINHTTP_QUERY_STATUS_TEXT, statusText))
    {
        response.statusText = Utf16ToUtf8(statusText);
    }

    const std::vector<std::pair<std::string, std::string>> headers = ParseRawHeaders(request);
    std::unordered_set<std::string> excluded = GetHopByHopHeaderNames();
    for (const auto& [name, value] : headers)
    {
        if (ToLowerAscii(name) == "connection")
        {
            AddConnectionTokens(value, excluded);
        }
    }

    std::optional<uint64_t> contentLength;
    for (const auto& [name, value] : headers)
    {
        const std::string lowerName = ToLowerAscii(name);
        if (lowerName == "content-length")
        {
            contentLength = ParseContentLength(value);
            continue;
        }

        if (!excluded.contains(lowerName))
        {
            response.AddHeader(name, value);
        }
    }

    return contentLength;
}
}

struct HttpClient::RequestState
{
    RequestState(HttpClient& owner, uint64_t requestId)
        : client(owner), id(requestId)
    {
    }

    bool Begin(PendingOperation operation, std::coroutine_handle<> continuation)
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (closed || request == nullptr || pending != PendingOperation::None)
        {
            result = { ERROR_OPERATION_ABORTED, 0 };
            return false;
        }

        pending = operation;
        result = {};
        this->continuation = continuation;
        return true;
    }

    void Complete(PendingOperation operation, DWORD error, DWORD value)
    {
        std::coroutine_handle<> resume;
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (pending != operation)
            {
                return;
            }

            pending = PendingOperation::None;
            result = { error, value };
            resume = continuation;
            continuation = {};
        }

        client.mIocpAwaiter.PostContinuation(resume);
    }

    void FailCurrent(DWORD error)
    {
        std::coroutine_handle<> resume;
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (pending == PendingOperation::None)
            {
                return;
            }

            pending = PendingOperation::None;
            result = { error, 0 };
            resume = continuation;
            continuation = {};
        }

        client.mIocpAwaiter.PostContinuation(resume);
    }

    void CompleteInline(DWORD error)
    {
        std::lock_guard<std::mutex> lock(mutex);
        pending = PendingOperation::None;
        result = { error, 0 };
        continuation = {};
    }

    OperationResult Consume() const
    {
        std::lock_guard<std::mutex> lock(mutex);
        return result;
    }

    HINTERNET RequestHandle() const
    {
        std::lock_guard<std::mutex> lock(mutex);
        return request;
    }

    bool SetHandles(HINTERNET connectionHandle, HINTERNET requestHandle)
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (closed)
        {
            return false;
        }

        connection = connectionHandle;
        request = requestHandle;
        return true;
    }

    void BeginCallbackRegistration()
    {
        std::lock_guard<std::mutex> lock(mutex);
        callbackRegistrationInProgress = true;
    }

    bool FinishCallbackRegistration(bool registered)
    {
        std::lock_guard<std::mutex> lock(mutex);
        callbackRegistered = registered;
        callbackRegistrationInProgress = false;
        callbackCondition.notify_all();
        return !closed;
    }

    bool IsClosed() const
    {
        std::lock_guard<std::mutex> lock(mutex);
        return closed;
    }

    bool Cancel()
    {
        HINTERNET requestHandle = nullptr;
        HINTERNET connectionHandle = nullptr;
        bool awaitHandleClosing = false;
        {
            std::unique_lock<std::mutex> lock(mutex);
            callbackCondition.wait(lock, [this]
                {
                    return !callbackRegistrationInProgress;
                });
            if (closed)
            {
                return waitingForHandleClose;
            }

            closed = true;
            requestHandle = request;
            connectionHandle = connection;
            awaitHandleClosing = callbackRegistered && requestHandle != nullptr;
            waitingForHandleClose = awaitHandleClosing;
            request = nullptr;
            connection = nullptr;
        }

        if (requestHandle != nullptr)
        {
            WinHttpCloseHandle(requestHandle);
        }
        if (connectionHandle != nullptr)
        {
            WinHttpCloseHandle(connectionHandle);
        }
        return awaitHandleClosing;
    }

    void HandleClosing()
    {
        std::coroutine_handle<> resume;
        {
            std::lock_guard<std::mutex> lock(mutex);
            waitingForHandleClose = false;
            request = nullptr;
            connection = nullptr;
            if (pending != PendingOperation::None)
            {
                pending = PendingOperation::None;
                result = { ERROR_OPERATION_ABORTED, 0 };
                resume = continuation;
                continuation = {};
            }
        }

        if (resume)
        {
            client.mIocpAwaiter.PostContinuation(resume);
        }
        client.RemoveRequest(id);
    }

    HttpClient& client;
    const uint64_t id;
    mutable std::mutex mutex;
    std::condition_variable callbackCondition;
    HINTERNET connection = nullptr;
    HINTERNET request = nullptr;
    PendingOperation pending = PendingOperation::None;
    std::coroutine_handle<> continuation{};
    OperationResult result{};
    bool closed = false;
    bool callbackRegistered = false;
    bool callbackRegistrationInProgress = false;
    bool waitingForHandleClose = false;
};

HttpClient::OperationAwaiter::OperationAwaiter(std::shared_ptr<RequestState> state, PendingOperation operation,
    char* buffer, DWORD bufferLength)
    : mState(std::move(state)), mOperation(operation), mBuffer(buffer), mBufferLength(bufferLength)
{
}

bool HttpClient::OperationAwaiter::await_ready() const noexcept
{
    return false;
}

bool HttpClient::OperationAwaiter::await_suspend(std::coroutine_handle<> continuation)
{
    if (!mState->Begin(mOperation, continuation))
    {
        return false;
    }

    if (mState->client.StartOperation(*mState, mOperation, mBuffer, mBufferLength))
    {
        return true;
    }

    const DWORD error = GetLastError();
    if (error == ERROR_IO_PENDING)
    {
        return true;
    }

    mState->CompleteInline(error);
    return false;
}

HttpClient::OperationResult HttpClient::OperationAwaiter::await_resume() const noexcept
{
    return mState->Consume();
}

HttpClient::HttpClient(IOCPAwaiter& iocpAwaiter)
    : mIocpAwaiter(iocpAwaiter)
{
    mSession = WinHttpOpen(L"SrvProj/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_ASYNC);
    if (mSession == nullptr)
    {
        throw std::runtime_error("WinHttpOpen failed");
    }

    WinHttpSetTimeouts(mSession, RequestTimeoutMilliseconds, RequestTimeoutMilliseconds,
        RequestTimeoutMilliseconds, RequestTimeoutMilliseconds);
}

HttpClient::~HttpClient()
{
    CancelAll();
    WaitForIdle();
    if (mSession != nullptr)
    {
        WinHttpCloseHandle(mSession);
        mSession = nullptr;
    }
}

AsyncTask<std::shared_ptr<HttpClient::RequestState>> HttpClient::OpenGet(const std::wstring& host,
    const std::wstring& path, const std::vector<std::pair<std::string, std::string>>& requestHeaders,
    OperationResult& result)
{
    std::shared_ptr<RequestState> state;
    {
        std::lock_guard<std::mutex> lock(mRequestsMutex);
        if (mSession == nullptr)
        {
            result = { ERROR_OPERATION_ABORTED, 0 };
            co_return nullptr;
        }

        state = std::make_shared<RequestState>(*this, mNextRequestId++);
        mRequests.emplace(state->id, state);
    }

    HINTERNET connection = WinHttpConnect(mSession, host.c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (connection == nullptr)
    {
        result = { GetLastError(), 0 };
        CloseRequest(state);
        co_return nullptr;
    }

    if (state->IsClosed())
    {
        result = { ERROR_OPERATION_ABORTED, 0 };
        WinHttpCloseHandle(connection);
        CloseRequest(state);
        co_return nullptr;
    }

    HINTERNET request = WinHttpOpenRequest(connection, L"GET", path.c_str(), nullptr, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
    if (request == nullptr)
    {
        result = { GetLastError(), 0 };
        WinHttpCloseHandle(connection);
        CloseRequest(state);
        co_return nullptr;
    }

    if (!state->SetHandles(connection, request))
    {
        result = { ERROR_OPERATION_ABORTED, 0 };
        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connection);
        CloseRequest(state);
        co_return nullptr;
    }

    state->BeginCallbackRegistration();
    const bool callbackRegistered = WinHttpSetStatusCallback(request, StatusCallback,
        WINHTTP_CALLBACK_FLAG_SEND_REQUEST | WINHTTP_CALLBACK_FLAG_HEADERS_AVAILABLE |
        WINHTTP_CALLBACK_FLAG_DATA_AVAILABLE | WINHTTP_CALLBACK_FLAG_READ_COMPLETE |
        WINHTTP_CALLBACK_FLAG_REQUEST_ERROR | WINHTTP_CALLBACK_FLAG_HANDLES,
        0) != WINHTTP_INVALID_STATUS_CALLBACK;
    const bool requestStillOpen = state->FinishCallbackRegistration(callbackRegistered);
    if (!callbackRegistered)
    {
        result = { GetLastError(), 0 };
        CloseRequest(state);
        co_return nullptr;
    }

    if (!requestStillOpen)
    {
        result = { ERROR_OPERATION_ABORTED, 0 };
        CloseRequest(state);
        co_return nullptr;
    }

    WinHttpSetTimeouts(request, RequestTimeoutMilliseconds, RequestTimeoutMilliseconds,
        RequestTimeoutMilliseconds, RequestTimeoutMilliseconds);

    for (const auto& [name, value] : requestHeaders)
    {
        const std::wstring header = Utf8ToUtf16(name + ": " + value);
        if (!header.empty() && !WinHttpAddRequestHeaders(request, header.c_str(),
            static_cast<DWORD>(header.size()), WINHTTP_ADDREQ_FLAG_ADD))
        {
            result = { GetLastError(), 0 };
            CloseRequest(state);
            co_return nullptr;
        }
    }

    result = co_await OperationAwaiter(state, PendingOperation::Send);
    if (!result.Succeeded())
    {
        CloseRequest(state);
        co_return nullptr;
    }

    result = co_await OperationAwaiter(state, PendingOperation::Receive);
    if (!result.Succeeded())
    {
        CloseRequest(state);
        co_return nullptr;
    }

    co_return state;
}

AsyncTask<HttpClient::Response> HttpClient::Get(const std::wstring& host, const std::wstring& path,
    const std::vector<std::pair<std::string, std::string>>& requestHeaders)
{
    Response response;
    OperationResult result;
    const std::shared_ptr<RequestState> state = co_await OpenGet(host, path, requestHeaders, result);
    if (state == nullptr)
    {
        response.error = result.error;
        response.timedOut = result.error == ERROR_WINHTTP_TIMEOUT;
        co_return response;
    }

    PopulateResponseHeaders(state->RequestHandle(), response.response);
    std::array<char, StreamBufferSize> buffer{};
    while (true)
    {
        result = co_await OperationAwaiter(state, PendingOperation::QueryData);
        if (!result.Succeeded())
        {
            response.error = result.error;
            break;
        }

        if (result.value == 0)
        {
            break;
        }

        const size_t readLength = std::min<size_t>(result.value, buffer.size());
        result = co_await OperationAwaiter(state, PendingOperation::Read, buffer.data(),
            static_cast<DWORD>(readLength));
        if (!result.Succeeded() || result.value == 0)
        {
            response.error = result.Succeeded() ? ERROR_WINHTTP_CONNECTION_ERROR : result.error;
            break;
        }

        if (response.response.body.size() > BufferedResponseLimit - result.value)
        {
            response.error = ERROR_FILE_TOO_LARGE;
            break;
        }

        response.response.body.append(buffer.data(), result.value);
    }

    CloseRequest(state);
    response.timedOut = response.error == ERROR_WINHTTP_TIMEOUT;
    co_return response;
}

AsyncTask<bool> HttpClient::WriteGatewayError(HttpResponseWriter& writer, int statusCode)
{
    HttpResponse response;
    response.statusCode = statusCode;
    response.statusText.clear();
    response.headers["Content-Type"] = "text/plain";
    response.body = statusCode == 504 ? "Gateway Timeout" : "Bad Gateway";
    co_return co_await writer.WriteResponse(response);
}

AsyncTask<bool> HttpClient::ProxyGet(const HttpRequest& request, HttpResponseWriter& writer)
{
    if (request.method != "GET")
    {
        HttpResponse response;
        response.statusCode = 405;
        response.statusText.clear();
        response.headers["Content-Type"] = "text/plain";
        response.body = "Method Not Allowed";
        co_return co_await writer.WriteResponse(response);
    }

    std::wstring path = Utf8ToUtf16(request.path);
    if (path.empty() || path.front() != L'/')
    {
        path = L"/";
    }

    OperationResult result;
    const std::shared_ptr<RequestState> state = co_await OpenGet(
        L"nova-static.stargazer-games.com", path, BuildForwardHeaders(request), result);
    if (state == nullptr)
    {
        co_await WriteGatewayError(writer, result.error == ERROR_WINHTTP_TIMEOUT ? 504 : 502);
        co_return false;
    }

    HttpResponse upstreamResponse;
    const std::optional<uint64_t> contentLength =
        PopulateResponseHeaders(state->RequestHandle(), upstreamResponse);
    HttpResponseBodyMode bodyMode = HttpResponseBodyMode::ContentLength;
    if (contentLength)
    {
        upstreamResponse.headers["Content-Length"] = std::to_string(*contentLength);
    }
    else if (writer.UsesHttp11())
    {
        bodyMode = HttpResponseBodyMode::Chunked;
    }
    else
    {
        bodyMode = HttpResponseBodyMode::Close;
    }

    if (!(co_await writer.WriteHeaders(upstreamResponse, bodyMode)))
    {
        CloseRequest(state);
        co_return false;
    }

    std::array<char, StreamBufferSize> buffer{};
    uint64_t transferred = 0;
    while (true)
    {
        result = co_await OperationAwaiter(state, PendingOperation::QueryData);
        if (!result.Succeeded())
        {
            writer.Abort();
            CloseRequest(state);
            co_return false;
        }

        if (result.value == 0)
        {
            break;
        }

        const size_t readLength = std::min<size_t>(result.value, buffer.size());
        result = co_await OperationAwaiter(state, PendingOperation::Read, buffer.data(),
            static_cast<DWORD>(readLength));
        if (!result.Succeeded() || result.value == 0)
        {
            writer.Abort();
            CloseRequest(state);
            co_return false;
        }

        if (!(co_await writer.WriteData(buffer.data(), result.value)))
        {
            writer.Abort();
            CloseRequest(state);
            co_return false;
        }

        transferred += result.value;
    }

    if (contentLength && transferred != *contentLength)
    {
        writer.Abort();
        CloseRequest(state);
        co_return false;
    }

    const bool finished = co_await writer.Finish();
    if (!finished)
    {
        writer.Abort();
    }
    CloseRequest(state);
    co_return finished;
}

void HttpClient::CancelAll()
{
    std::vector<std::shared_ptr<RequestState>> requests;
    {
        std::lock_guard<std::mutex> lock(mRequestsMutex);
        requests.reserve(mRequests.size());
        for (const auto& [id, request] : mRequests)
        {
            requests.push_back(request);
        }
    }

    for (const std::shared_ptr<RequestState>& request : requests)
    {
        if (!request->Cancel())
        {
            RemoveRequest(request->id);
        }
    }
}

void HttpClient::WaitForIdle()
{
    std::unique_lock<std::mutex> lock(mRequestsMutex);
    mRequestsCondition.wait(lock, [this]
        {
            return mRequests.empty();
        });
}

void HttpClient::CloseRequest(const std::shared_ptr<RequestState>& state)
{
    if (state == nullptr)
    {
        return;
    }

    if (!state->Cancel())
    {
        RemoveRequest(state->id);
    }
}

void HttpClient::RemoveRequest(uint64_t id)
{
    {
        std::lock_guard<std::mutex> lock(mRequestsMutex);
        mRequests.erase(id);
    }
    mRequestsCondition.notify_all();
}

bool HttpClient::StartOperation(RequestState& state, PendingOperation operation, char* buffer, DWORD bufferLength)
{
    const HINTERNET request = state.RequestHandle();
    if (request == nullptr)
    {
        SetLastError(ERROR_OPERATION_ABORTED);
        return false;
    }

    switch (operation)
    {
    case PendingOperation::Send:
        return WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0,
            reinterpret_cast<DWORD_PTR>(&state)) == TRUE;
    case PendingOperation::Receive:
        return WinHttpReceiveResponse(request, nullptr) == TRUE;
    case PendingOperation::QueryData:
        return WinHttpQueryDataAvailable(request, nullptr) == TRUE;
    case PendingOperation::Read:
        return WinHttpReadData(request, buffer, bufferLength, nullptr) == TRUE;
    default:
        SetLastError(ERROR_INVALID_PARAMETER);
        return false;
    }
}

void CALLBACK HttpClient::StatusCallback(HINTERNET, DWORD_PTR context, DWORD status,
    LPVOID statusInformation, DWORD statusInformationLength)
{
    auto* state = reinterpret_cast<RequestState*>(context);
    if (state == nullptr)
    {
        return;
    }

    switch (status)
    {
    case WINHTTP_CALLBACK_STATUS_SENDREQUEST_COMPLETE:
        state->Complete(PendingOperation::Send, ERROR_SUCCESS, 0);
        break;
    case WINHTTP_CALLBACK_STATUS_HEADERS_AVAILABLE:
        state->Complete(PendingOperation::Receive, ERROR_SUCCESS, 0);
        break;
    case WINHTTP_CALLBACK_STATUS_DATA_AVAILABLE:
        state->Complete(PendingOperation::QueryData, ERROR_SUCCESS,
            statusInformation == nullptr ? 0 : *static_cast<DWORD*>(statusInformation));
        break;
    case WINHTTP_CALLBACK_STATUS_READ_COMPLETE:
        state->Complete(PendingOperation::Read, ERROR_SUCCESS, statusInformationLength);
        break;
    case WINHTTP_CALLBACK_STATUS_REQUEST_ERROR:
        if (statusInformation != nullptr)
        {
            const auto* asyncResult = static_cast<WINHTTP_ASYNC_RESULT*>(statusInformation);
            state->FailCurrent(asyncResult->dwError);
        }
        else
        {
            state->FailCurrent(ERROR_WINHTTP_CONNECTION_ERROR);
        }
        break;
    case WINHTTP_CALLBACK_STATUS_HANDLE_CLOSING:
        state->HandleClosing();
        break;
    default:
        break;
    }
}
