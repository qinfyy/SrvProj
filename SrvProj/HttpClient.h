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

class HttpResponseWriter
{
public:
    virtual ~HttpResponseWriter() = default;

    virtual AsyncTask<bool> WriteHeaders(const HttpResponse& response, HttpResponseBodyMode bodyMode) = 0;
    virtual AsyncTask<bool> WriteData(const char* data, size_t length) = 0;
    virtual AsyncTask<bool> Finish() = 0;
    virtual void Abort() noexcept = 0;
    virtual bool UsesHttp11() const noexcept = 0;
    virtual bool HasStarted() const noexcept = 0;
    virtual bool IsFinished() const noexcept = 0;
    virtual int StatusCode() const noexcept = 0;

    AsyncTask<bool> WriteResponse(const HttpResponse& response)
    {
        if (!(co_await WriteHeaders(response, HttpResponseBodyMode::ContentLength)))
        {
            co_return false;
        }

        if (!response.body.empty() && !(co_await WriteData(response.body.data(), response.body.size())))
        {
            co_return false;
        }

        co_return co_await Finish();
    }
};

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

    explicit HttpClient(IOCPAwaiter& iocpAwaiter);
    ~HttpClient();

    HttpClient(const HttpClient&) = delete;
    HttpClient& operator=(const HttpClient&) = delete;

    AsyncTask<Response> Get(const std::wstring& host, const std::wstring& path,
        const std::vector<std::pair<std::string, std::string>>& requestHeaders = {});
    AsyncTask<bool> ProxyGet(const HttpRequest& request, HttpResponseWriter& writer);

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

    struct RequestState;

    class OperationAwaiter
    {
    public:
        OperationAwaiter(std::shared_ptr<RequestState> state, PendingOperation operation,
            char* buffer = nullptr, DWORD bufferLength = 0);

        bool await_ready() const noexcept;
        bool await_suspend(std::coroutine_handle<> continuation);
        OperationResult await_resume() const noexcept;

    private:
        std::shared_ptr<RequestState> mState;
        PendingOperation mOperation;
        char* mBuffer;
        DWORD mBufferLength;
    };

    AsyncTask<std::shared_ptr<RequestState>> OpenGet(const std::wstring& host, const std::wstring& path,
        const std::vector<std::pair<std::string, std::string>>& requestHeaders, OperationResult& result);
    AsyncTask<bool> WriteGatewayError(HttpResponseWriter& writer, int statusCode);
    void CloseRequest(const std::shared_ptr<RequestState>& state);
    void RemoveRequest(uint64_t id);
    bool StartOperation(RequestState& state, PendingOperation operation, char* buffer, DWORD bufferLength);
    static void CALLBACK StatusCallback(HINTERNET handle, DWORD_PTR context, DWORD status,
        LPVOID statusInformation, DWORD statusInformationLength);

    IOCPAwaiter& mIocpAwaiter;
    HINTERNET mSession = nullptr;
    std::mutex mRequestsMutex;
    std::condition_variable mRequestsCondition;
    std::unordered_map<uint64_t, std::shared_ptr<RequestState>> mRequests;
    uint64_t mNextRequestId = 1;
};
