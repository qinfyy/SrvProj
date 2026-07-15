#pragma once

#include <winsock2.h>
#include <mswsock.h>
#include <windows.h>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <atomic>
#include <condition_variable>
#include <coroutine>
#include <cstddef>
#include <exception>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class IOCPAwaiter
{
private:
    struct Operation
    {
        OVERLAPPED overlapped{};
        std::coroutine_handle<> continuation{};
        DWORD bytes = 0;
        DWORD error = ERROR_SUCCESS;
        std::exception_ptr exception;
    };

    static constexpr ULONG_PTR ShutdownCompletionKey = 1;

public:
    struct IoResult
    {
        DWORD bytes = 0;
        DWORD error = ERROR_SUCCESS;

        explicit operator bool() const;
    };

    explicit IOCPAwaiter(size_t completionThreadCount, size_t businessThreadCount);
    ~IOCPAwaiter();

    IOCPAwaiter(const IOCPAwaiter&) = delete;
    IOCPAwaiter& operator=(const IOCPAwaiter&) = delete;

    bool AssociateSocket(SOCKET socket);
    size_t CompletionThreadCount() const;

    class ReceiveAwaiter
    {
    public:
        ReceiveAwaiter(IOCPAwaiter& awaiter, SOCKET socket, char* buffer, ULONG length);

        bool await_ready() const noexcept;
        bool await_suspend(std::coroutine_handle<> continuation);
        IoResult await_resume() const noexcept;

    private:
        IOCPAwaiter& mAwaiter;
        SOCKET mSocket;
        WSABUF mBuffer{};
        Operation mOperation;
    };

    class SendAwaiter
    {
    public:
        SendAwaiter(IOCPAwaiter& awaiter, SOCKET socket, const char* buffer, ULONG length);

        bool await_ready() const noexcept;
        bool await_suspend(std::coroutine_handle<> continuation);
        IoResult await_resume() const noexcept;

    private:
        IOCPAwaiter& mAwaiter;
        SOCKET mSocket;
        WSABUF mBuffer{};
        Operation mOperation;
    };

    class AcceptAwaiter
    {
    public:
        AcceptAwaiter(IOCPAwaiter& awaiter, LPFN_ACCEPTEX acceptEx, SOCKET listenSocket, SOCKET clientSocket,
            char* addressBuffer, DWORD addressBufferLength);

        bool await_ready() const noexcept;
        bool await_suspend(std::coroutine_handle<> continuation);
        IoResult await_resume() const noexcept;

    private:
        IOCPAwaiter& mAwaiter;
        LPFN_ACCEPTEX mAcceptEx;
        SOCKET mListenSocket;
        SOCKET mClientSocket;
        char* mAddressBuffer;
        DWORD mAddressBufferLength;
        Operation mOperation;
    };

    class BlockingAwaiter
    {
    public:
        BlockingAwaiter(IOCPAwaiter& awaiter, std::function<void()> task);

        bool await_ready() const noexcept;
        bool await_suspend(std::coroutine_handle<> continuation);
        void await_resume();

    private:
        IOCPAwaiter& mAwaiter;
        std::function<void()> mTask;
        Operation mOperation;
    };

    ReceiveAwaiter Receive(SOCKET socket, char* buffer, ULONG length);
    SendAwaiter Send(SOCKET socket, const char* buffer, ULONG length);
    AcceptAwaiter Accept(LPFN_ACCEPTEX acceptEx, SOCKET listenSocket, SOCKET clientSocket,
        char* addressBuffer, DWORD addressBufferLength);
    BlockingAwaiter RunBlocking(std::function<void()> task);

    void StopAcceptingWork();
    void WaitForIdle();
    void Shutdown();

private:
    bool BeginOperation(Operation& operation, std::coroutine_handle<> continuation);
    void CompleteInline(Operation& operation, DWORD bytes, DWORD error);
    void PostCompletion(Operation& operation);
    void FinishOperation();
    void CompletionWorker();
    void BusinessWorker();

    HANDLE mCompletionPort = nullptr;
    std::vector<std::thread> mCompletionWorkers;
    std::vector<std::thread> mBusinessWorkers;

    std::queue<std::function<void()>> mBusinessTasks;
    std::mutex mBusinessMutex;
    std::condition_variable mBusinessCondition;
    bool mBusinessStopping = false;

    std::atomic<bool> mAcceptingWork = true;
    std::atomic<bool> mShutdown = false;
    std::atomic<size_t> mOutstandingOperations = 0;
    std::mutex mIdleMutex;
    std::condition_variable mIdleCondition;
};
