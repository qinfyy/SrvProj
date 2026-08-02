#include "IOCPAwaiter.h"

#include <stdexcept>
#include <utility>

IOCPAwaiter::IoResult::operator bool() const
{
    return error == ERROR_SUCCESS;
}

IOCPAwaiter::IOCPAwaiter(size_t completionThreadCount, size_t businessThreadCount)
    : mCompletionPort(CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, 0, 0))
{
    if (mCompletionPort == nullptr)
    {
        throw std::runtime_error("CreateIoCompletionPort failed");
    }

    try
    {
        mCompletionWorkers.reserve(completionThreadCount);
        for (size_t i = 0; i < completionThreadCount; ++i)
        {
            mCompletionWorkers.emplace_back(&IOCPAwaiter::CompletionWorker, this);
        }

        mBusinessWorkers.reserve(businessThreadCount);
        for (size_t i = 0; i < businessThreadCount; ++i)
        {
            mBusinessWorkers.emplace_back(&IOCPAwaiter::BusinessWorker, this);
        }
    }
    catch (...)
    {
        Shutdown();
        throw;
    }
}

IOCPAwaiter::~IOCPAwaiter()
{
    Shutdown();
}

bool IOCPAwaiter::AssociateSocket(SOCKET socket)
{
    return CreateIoCompletionPort(reinterpret_cast<HANDLE>(socket), mCompletionPort, 0, 0) == mCompletionPort;
}

size_t IOCPAwaiter::CompletionThreadCount() const
{
    return mCompletionWorkers.size();
}

IOCPAwaiter::ReceiveAwaiter::ReceiveAwaiter(IOCPAwaiter& awaiter, SOCKET socket, char* buffer, ULONG length)
    : mAwaiter(awaiter), mSocket(socket)
{
    mBuffer.buf = buffer;
    mBuffer.len = length;
}

bool IOCPAwaiter::ReceiveAwaiter::IsCompleted() const noexcept
{
    return false;
}

bool IOCPAwaiter::ReceiveAwaiter::OnCompleted(std::coroutine_handle<> continuation)
{
    Operation* operation = &mOperation;
    if (!mAwaiter.BeginOperation(*operation, continuation))
    {
        operation->error = ERROR_OPERATION_ABORTED;
        return false;
    }

    DWORD bytes = 0;
    DWORD flags = 0;
    const int result = WSARecv(mSocket, &mBuffer, 1, &bytes, &flags, &operation->overlapped, nullptr);
    if (result == 0)
    {
        return true;
    }

    const DWORD error = WSAGetLastError();
    if (error == WSA_IO_PENDING)
    {
        return true;
    }

    mAwaiter.CompleteInline(*operation, bytes, error);
    return false;
}

IOCPAwaiter::IoResult IOCPAwaiter::ReceiveAwaiter::GetResult()
{
    return { mOperation.bytes, mOperation.error };
}

IOCPAwaiter::SendAwaiter::SendAwaiter(IOCPAwaiter& awaiter, SOCKET socket, const char* buffer, ULONG length)
    : mAwaiter(awaiter), mSocket(socket)
{
    mBuffer.buf = const_cast<char*>(buffer);
    mBuffer.len = length;
}

bool IOCPAwaiter::SendAwaiter::IsCompleted() const noexcept
{
    return false;
}

bool IOCPAwaiter::SendAwaiter::OnCompleted(std::coroutine_handle<> continuation)
{
    Operation* operation = &mOperation;
    if (!mAwaiter.BeginOperation(*operation, continuation))
    {
        operation->error = ERROR_OPERATION_ABORTED;
        return false;
    }

    DWORD bytes = 0;
    const int result = WSASend(mSocket, &mBuffer, 1, &bytes, 0, &operation->overlapped, nullptr);
    if (result == 0)
    {
        return true;
    }

    const DWORD error = WSAGetLastError();
    if (error == WSA_IO_PENDING)
    {
        return true;
    }

    mAwaiter.CompleteInline(*operation, bytes, error);
    return false;
}

IOCPAwaiter::IoResult IOCPAwaiter::SendAwaiter::GetResult()
{
    return { mOperation.bytes, mOperation.error };
}

IOCPAwaiter::AcceptAwaiter::AcceptAwaiter(IOCPAwaiter& awaiter, LPFN_ACCEPTEX acceptEx, SOCKET listenSocket,
    SOCKET clientSocket, char* addressBuffer, DWORD addressBufferLength)
    : mAwaiter(awaiter), mAcceptEx(acceptEx), mListenSocket(listenSocket), mClientSocket(clientSocket),
    mAddressBuffer(addressBuffer), mAddressBufferLength(addressBufferLength)
{
}

bool IOCPAwaiter::AcceptAwaiter::IsCompleted() const noexcept
{
    return false;
}

bool IOCPAwaiter::AcceptAwaiter::OnCompleted(std::coroutine_handle<> continuation)
{
    Operation* operation = &mOperation;
    if (!mAwaiter.BeginOperation(*operation, continuation))
    {
        operation->error = ERROR_OPERATION_ABORTED;
        return false;
    }

    DWORD bytes = 0;
    const BOOL result = mAcceptEx(mListenSocket, mClientSocket, mAddressBuffer, 0,
        mAddressBufferLength / 2, mAddressBufferLength / 2, &bytes, &operation->overlapped);
    if (result == TRUE)
    {
        return true;
    }

    const DWORD error = WSAGetLastError();
    if (error == ERROR_IO_PENDING)
    {
        return true;
    }

    mAwaiter.CompleteInline(*operation, bytes, error);
    return false;
}

IOCPAwaiter::IoResult IOCPAwaiter::AcceptAwaiter::GetResult()
{
    return { mOperation.bytes, mOperation.error };
}

IOCPAwaiter::BlockingAwaiter::BlockingAwaiter(IOCPAwaiter& awaiter, std::function<void()> task)
    : mAwaiter(awaiter), mTask(std::move(task))
{
}

bool IOCPAwaiter::BlockingAwaiter::IsCompleted() const noexcept
{
    return false;
}

bool IOCPAwaiter::BlockingAwaiter::OnCompleted(std::coroutine_handle<> continuation)
{
    Operation* operation = &mOperation;
    if (!mAwaiter.BeginOperation(*operation, continuation))
    {
        operation->error = ERROR_OPERATION_ABORTED;
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(mAwaiter.mBusinessMutex);
        if (!mAwaiter.mAcceptingWork.load())
        {
            mAwaiter.CompleteInline(*operation, 0, ERROR_OPERATION_ABORTED);
            return false;
        }

        mAwaiter.mBusinessTasks.emplace([awaiter = &mAwaiter, operation, task = std::move(mTask)]() mutable
            {
                try
                {
                    task();
                }
                catch (...)
                {
                    operation->exception = std::current_exception();
                }

                awaiter->PostCompletion(*operation);
            });
    }

    mAwaiter.mBusinessCondition.notify_one();
    return true;
}

void IOCPAwaiter::BlockingAwaiter::GetResult()
{
    if (mOperation.exception)
    {
        std::rethrow_exception(mOperation.exception);
    }
}

IOCPAwaiter::ReceiveAwaiter IOCPAwaiter::Receive(SOCKET socket, char* buffer, ULONG length)
{
    return ReceiveAwaiter(*this, socket, buffer, length);
}

IOCPAwaiter::SendAwaiter IOCPAwaiter::Send(SOCKET socket, const char* buffer, ULONG length)
{
    return SendAwaiter(*this, socket, buffer, length);
}

IOCPAwaiter::AcceptAwaiter IOCPAwaiter::Accept(LPFN_ACCEPTEX acceptEx, SOCKET listenSocket, SOCKET clientSocket,
    char* addressBuffer, DWORD addressBufferLength)
{
    return AcceptAwaiter(*this, acceptEx, listenSocket, clientSocket, addressBuffer, addressBufferLength);
}

IOCPAwaiter::BlockingAwaiter IOCPAwaiter::RunBlocking(std::function<void()> task)
{
    return BlockingAwaiter(*this, std::move(task));
}

bool IOCPAwaiter::PostContinuation(std::coroutine_handle<> continuation)
{
    if (!continuation || mShutdown.load() || mCompletionPort == nullptr)
    {
        return false;
    }

    auto* operation = new ExternalOperation();
    operation->continuation = continuation;
    mOutstandingOperations.fetch_add(1);
    if (PostQueuedCompletionStatus(mCompletionPort, 0, ExternalCompletionKey, &operation->overlapped))
    {
        return true;
    }

    delete operation;
    FinishOperation();
    return false;
}

void IOCPAwaiter::StopAcceptingWork()
{
    mAcceptingWork = false;
}

void IOCPAwaiter::WaitForIdle()
{
    std::unique_lock<std::mutex> lock(mIdleMutex);
    mIdleCondition.wait(lock, [this]
        {
            return mOutstandingOperations.load() == 0;
        });
}

void IOCPAwaiter::Shutdown()
{
    bool expected = false;
    if (!mShutdown.compare_exchange_strong(expected, true))
    {
        return;
    }

    StopAcceptingWork();

    {
        std::lock_guard<std::mutex> lock(mBusinessMutex);
        mBusinessStopping = true;
    }
    mBusinessCondition.notify_all();

    for (std::thread& worker : mBusinessWorkers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
    mBusinessWorkers.clear();

    for (size_t i = 0; i < mCompletionWorkers.size(); ++i)
    {
        PostQueuedCompletionStatus(mCompletionPort, 0, ShutdownCompletionKey, nullptr);
    }

    for (std::thread& worker : mCompletionWorkers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
    mCompletionWorkers.clear();

    if (mCompletionPort != nullptr)
    {
        CloseHandle(mCompletionPort);
        mCompletionPort = nullptr;
    }
}

bool IOCPAwaiter::BeginOperation(Operation& operation, std::coroutine_handle<> continuation)
{
    if (!mAcceptingWork.load())
    {
        return false;
    }

    operation.continuation = continuation;
    operation.bytes = 0;
    operation.error = ERROR_SUCCESS;
    operation.exception = nullptr;
    ZeroMemory(&operation.overlapped, sizeof(operation.overlapped));
    mOutstandingOperations.fetch_add(1);
    return true;
}

void IOCPAwaiter::CompleteInline(Operation& operation, DWORD bytes, DWORD error)
{
    operation.bytes = bytes;
    operation.error = error;
    FinishOperation();
}

void IOCPAwaiter::PostCompletion(Operation& operation)
{
    if (!PostQueuedCompletionStatus(mCompletionPort, 0, 0, &operation.overlapped))
    {
        operation.error = GetLastError();
        FinishOperation();
        operation.continuation.resume();
    }
}

void IOCPAwaiter::FinishOperation()
{
    if (mOutstandingOperations.fetch_sub(1) == 1)
    {
        std::lock_guard<std::mutex> lock(mIdleMutex);
        mIdleCondition.notify_all();
    }
}

void IOCPAwaiter::CompletionWorker()
{
    while (true)
    {
        DWORD bytes = 0;
        ULONG_PTR completionKey = 0;
        LPOVERLAPPED overlapped = nullptr;
        const BOOL result = GetQueuedCompletionStatus(mCompletionPort, &bytes, &completionKey, &overlapped, INFINITE);

        if (overlapped == nullptr)
        {
            if (completionKey == ShutdownCompletionKey)
            {
                return;
            }
            continue;
        }

        if (completionKey == ExternalCompletionKey)
        {
            ExternalOperation* operation = CONTAINING_RECORD(overlapped, ExternalOperation, overlapped);
            std::coroutine_handle<> continuation = operation->continuation;
            delete operation;
            FinishOperation();
            continuation.resume();
            continue;
        }

        Operation* operation = CONTAINING_RECORD(overlapped, Operation, overlapped);
        operation->bytes = bytes;
        operation->error = result == TRUE ? ERROR_SUCCESS : GetLastError();
        FinishOperation();
        operation->continuation.resume();
    }
}

void IOCPAwaiter::BusinessWorker()
{
    while (true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mBusinessMutex);
            mBusinessCondition.wait(lock, [this]
                {
                    return mBusinessStopping || !mBusinessTasks.empty();
                });

            if (mBusinessStopping && mBusinessTasks.empty())
            {
                return;
            }

            task = std::move(mBusinessTasks.front());
            mBusinessTasks.pop();
        }

        task();
    }
}
