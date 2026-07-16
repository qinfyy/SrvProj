#pragma once

#include <winsock2.h>
#include <windows.h>
#include <winhttp.h>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <condition_variable>
#include <coroutine>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "AsyncTask.h"
#include "HttpMessage.h"

class IOCPAwaiter;

class HttpClient
{
public:
    struct Response
    {
        HttpResponse response;
        DWORD error = ERROR_SUCCESS;
        bool timedOut = false;

        bool Succeeded() const noexcept
        {
            return error == ERROR_SUCCESS;
        }
    };

    struct RequestOptions
    {
        std::string method = "GET";
        std::string url;
        std::vector<std::pair<std::string, std::string>> headers;
        std::string body;
    };

    explicit HttpClient(IOCPAwaiter& iocpAwaiter);
    ~HttpClient();

    HttpClient(const HttpClient&) = delete;
    HttpClient& operator=(const HttpClient&) = delete;

    AsyncTask<Response> Request(const RequestOptions& options);
    AsyncTask<Response> Get(const std::string& url, const std::vector<std::pair<std::string, std::string>>& requestHeaders = {});
    AsyncTask<Response> Post(const std::string& url, const std::string& body, const std::vector<std::pair<std::string, std::string>>& requestHeaders = {});
    AsyncTask<Response> Put(const std::string& url, const std::string& body, const std::vector<std::pair<std::string, std::string>>& requestHeaders = {});
    AsyncTask<Response> Delete(const std::string& url, const std::vector<std::pair<std::string, std::string>>& requestHeaders = {});
    AsyncTask<bool> Proxy(const HttpRequest& request, HttpResponseWriter& writer, const std::string& baseUrl);

    void CancelAll();
    void WaitForIdle();

private:
    enum class PendingOperation
    {
        None,
        Send,
        Receive,
        QueryData,
        Read
    };

    struct OperationResult
    {
        DWORD error = ERROR_SUCCESS;
        DWORD value = 0;

        bool Succeeded() const noexcept
        {
            return error == ERROR_SUCCESS;
        }
    };

    struct Target
    {
        std::wstring host;
        std::wstring path;
        INTERNET_PORT port = 0;
        bool secure = false;
    };

    struct RequestState;

    class OperationAwaiter : public Awaiter<OperationAwaiter, OperationResult>
    {
    public:
        OperationAwaiter(std::shared_ptr<RequestState> state, PendingOperation operation, char* buffer = nullptr, DWORD bufferLength = 0);

        bool IsCompleted() const noexcept;
        bool OnCompleted(std::coroutine_handle<> continuation);
        OperationResult GetResult() const noexcept;

    private:
        std::shared_ptr<RequestState> mState;
        PendingOperation mOperation;
        char* mBuffer;
        DWORD mBufferLength;
    };

    static bool ParseUrl(const std::string& url, Target& out);
    static bool JoinProxyTarget(const std::string& baseUrl, const HttpRequest& request, Target& out, std::string& method);
    AsyncTask<std::shared_ptr<RequestState>> OpenRequest(const Target& target, const std::wstring& method, const std::vector<std::pair<std::string, std::string>>& requestHeaders, std::string body, OperationResult& result);
    AsyncTask<bool> WriteGatewayError(HttpResponseWriter& writer, int statusCode);
    void CloseRequest(const std::shared_ptr<RequestState>& state);
    void RemoveRequest(uint64_t id);
    bool StartOperation(RequestState& state, PendingOperation operation, char* buffer, DWORD bufferLength);
    static void CALLBACK StatusCallback(HINTERNET handle, DWORD_PTR context, DWORD status, LPVOID statusInformation, DWORD statusInformationLength);

    IOCPAwaiter& mIocpAwaiter;
    HINTERNET mSession = nullptr;
    std::mutex mRequestsMutex;
    std::condition_variable mRequestsCondition;
    std::unordered_map<uint64_t, std::shared_ptr<RequestState>> mRequests;
    uint64_t mNextRequestId = 1;
};
