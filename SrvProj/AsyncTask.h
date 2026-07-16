#pragma once

#include <atomic>
#include <coroutine>
#include <exception>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

#include "Logger.h"

template <typename TResult = void>
class Awaiter
{
public:
    virtual ~Awaiter() = default;

    bool await_ready() const noexcept
    {
        return IsCompleted();
    }

    bool await_suspend(std::coroutine_handle<> continuation)
    {
        return OnCompleted(continuation);
    }

    TResult await_resume()
    {
        return GetResult();
    }

    virtual bool IsCompleted() const noexcept = 0;
    virtual bool OnCompleted(std::coroutine_handle<> continuation) = 0;
    virtual TResult GetResult() = 0;

protected:
    Awaiter() = default;
    Awaiter(const Awaiter&) = delete;
    Awaiter& operator=(const Awaiter&) = delete;
    Awaiter(Awaiter&&) = default;
    Awaiter& operator=(Awaiter&&) = default;
};

template <typename T>
class AsyncTask
{
public:
    struct promise_type;
    using Handle = std::coroutine_handle<promise_type>;

    AsyncTask() = default;

    explicit AsyncTask(Handle handle) noexcept
        : mHandle(handle)
    {
    }

    AsyncTask(const AsyncTask&) = delete;
    AsyncTask& operator=(const AsyncTask&) = delete;

    AsyncTask(AsyncTask&& other) noexcept
        : mHandle(std::exchange(other.mHandle, {}))
    {
    }

    AsyncTask& operator=(AsyncTask&& other) noexcept
    {
        if (this != &other)
        {
            if (mHandle)
            {
                mHandle.destroy();
            }
            mHandle = std::exchange(other.mHandle, {});
        }
        return *this;
    }

    ~AsyncTask()
    {
        if (mHandle)
        {
            mHandle.destroy();
        }
    }

    struct TaskAwaiter
    {
        Handle handle{};

        explicit TaskAwaiter(Handle h) noexcept
            : handle(h)
        {
        }

        bool await_ready() const noexcept
        {
            return !handle || handle.done();
        }

        std::coroutine_handle<> await_suspend(std::coroutine_handle<> continuation) noexcept
        {
            handle.promise().continuation = continuation;
            return handle;
        }

        T await_resume()
        {
            promise_type& promise = handle.promise();
            if (promise.exception)
            {
                std::rethrow_exception(promise.exception);
            }
            return std::move(*promise.value);
        }
    };

    TaskAwaiter operator co_await() const noexcept
    {
        return TaskAwaiter(mHandle);
    }

    struct promise_type
    {
        std::optional<T> value;
        std::exception_ptr exception;
        std::coroutine_handle<> continuation = std::noop_coroutine();

        AsyncTask get_return_object() noexcept
        {
            return AsyncTask(Handle::from_promise(*this));
        }

        std::suspend_always initial_suspend() noexcept
        {
            return {};
        }

        struct FinalAwaiter
        {
            bool await_ready() const noexcept
            {
                return false;
            }

            std::coroutine_handle<> await_suspend(Handle handle) noexcept
            {
                return handle.promise().continuation;
            }

            void await_resume() noexcept
            {
            }
        };

        FinalAwaiter final_suspend() noexcept
        {
            return {};
        }

        template <typename U>
        void return_value(U&& result)
        {
            value.emplace(std::forward<U>(result));
        }

        void unhandled_exception() noexcept
        {
            exception = std::current_exception();
        }
    };

private:
    Handle mHandle{};
};

template <>
class AsyncTask<void>
{
public:
    struct promise_type;
    using Handle = std::coroutine_handle<promise_type>;

    AsyncTask() = default;

    explicit AsyncTask(Handle handle) noexcept
        : mHandle(handle)
    {
    }

    AsyncTask(const AsyncTask&) = delete;
    AsyncTask& operator=(const AsyncTask&) = delete;

    AsyncTask(AsyncTask&& other) noexcept
        : mHandle(std::exchange(other.mHandle, {}))
    {
    }

    AsyncTask& operator=(AsyncTask&& other) noexcept
    {
        if (this != &other)
        {
            if (mHandle)
            {
                mHandle.destroy();
            }
            mHandle = std::exchange(other.mHandle, {});
        }
        return *this;
    }

    ~AsyncTask()
    {
        if (mHandle)
        {
            mHandle.destroy();
        }
    }

    struct TaskAwaiter
    {
        Handle handle{};

        explicit TaskAwaiter(Handle h) noexcept
            : handle(h)
        {
        }

        bool await_ready() const noexcept
        {
            return !handle || handle.done();
        }

        std::coroutine_handle<> await_suspend(std::coroutine_handle<> continuation) noexcept
        {
            handle.promise().continuation = continuation;
            return handle;
        }

        void await_resume()
        {
            if (handle.promise().exception)
            {
                std::rethrow_exception(handle.promise().exception);
            }
        }
    };

    TaskAwaiter operator co_await() const noexcept
    {
        return TaskAwaiter(mHandle);
    }

    struct promise_type
    {
        std::exception_ptr exception;
        std::coroutine_handle<> continuation = std::noop_coroutine();

        AsyncTask get_return_object() noexcept
        {
            return AsyncTask(Handle::from_promise(*this));
        }

        std::suspend_always initial_suspend() noexcept
        {
            return {};
        }

        struct FinalAwaiter
        {
            bool await_ready() const noexcept
            {
                return false;
            }

            std::coroutine_handle<> await_suspend(Handle handle) noexcept
            {
                return handle.promise().continuation;
            }

            void await_resume() noexcept
            {
            }
        };

        FinalAwaiter final_suspend() noexcept
        {
            return {};
        }

        void return_void() noexcept
        {
        }

        void unhandled_exception() noexcept
        {
            exception = std::current_exception();
        }
    };

private:
    Handle mHandle{};
};

class DetachedTask
{
public:
    struct promise_type
    {
        DetachedTask get_return_object() noexcept
        {
            return {};
        }

        std::suspend_never initial_suspend() noexcept
        {
            return {};
        }

        std::suspend_never final_suspend() noexcept
        {
            return {};
        }

        void return_void() noexcept
        {
        }

        void unhandled_exception() noexcept
        {
            try
            {
                throw;
            }
            catch (const std::exception& e)
            {
                LOG_ERROR("Unhandled DetachedTask exception: {}", e.what());
            }
            catch (...)
            {
                LOG_ERROR("Unhandled DetachedTask exception");
            }
        }
    };
};

template <typename T>
class TaskCompletionSource
{
    struct State
    {
        std::mutex mutex;
        bool completed = false;
        std::optional<T> value;
        std::exception_ptr exception;
        std::vector<std::coroutine_handle<>> waiters;
    };

public:
    class TcsAwaiter : public Awaiter<T>
    {
    public:
        explicit TcsAwaiter(std::shared_ptr<State> state) noexcept
            : mState(std::move(state))
        {
        }

        bool IsCompleted() const noexcept override
        {
            std::lock_guard<std::mutex> lock(mState->mutex);
            return mState->completed;
        }

        bool OnCompleted(std::coroutine_handle<> continuation) override
        {
            std::lock_guard<std::mutex> lock(mState->mutex);
            if (mState->completed)
            {
                return false;
            }
            mState->waiters.push_back(continuation);
            return true;
        }

        T GetResult() override
        {
            if (mState->exception)
            {
                std::rethrow_exception(mState->exception);
            }
            return std::move(*mState->value);
        }

    private:
        std::shared_ptr<State> mState;
    };

    TaskCompletionSource()
        : mState(std::make_shared<State>())
    {
    }

    bool TrySetResult(T value)
    {
        std::vector<std::coroutine_handle<>> waiters;
        {
            std::lock_guard<std::mutex> lock(mState->mutex);
            if (mState->completed)
            {
                return false;
            }
            mState->completed = true;
            mState->value.emplace(std::move(value));
            waiters.swap(mState->waiters);
        }
        for (std::coroutine_handle<> waiter : waiters)
        {
            waiter.resume();
        }
        return true;
    }

    void SetResult(T value)
    {
        if (!TrySetResult(std::move(value)))
        {
            throw std::logic_error("TaskCompletionSource already completed");
        }
    }

    bool TrySetException(std::exception_ptr exception)
    {
        if (!exception)
        {
            return false;
        }

        std::vector<std::coroutine_handle<>> waiters;
        {
            std::lock_guard<std::mutex> lock(mState->mutex);
            if (mState->completed)
            {
                return false;
            }
            mState->completed = true;
            mState->exception = std::move(exception);
            waiters.swap(mState->waiters);
        }
        for (std::coroutine_handle<> waiter : waiters)
        {
            waiter.resume();
        }
        return true;
    }

    void SetException(std::exception_ptr exception)
    {
        if (!TrySetException(std::move(exception)))
        {
            throw std::logic_error("TaskCompletionSource already completed");
        }
    }

    TcsAwaiter GetAwaiter() const noexcept
    {
        return TcsAwaiter(mState);
    }

private:
    std::shared_ptr<State> mState;
};

template <>
class TaskCompletionSource<void>
{
    struct State
    {
        std::mutex mutex;
        bool completed = false;
        std::exception_ptr exception;
        std::vector<std::coroutine_handle<>> waiters;
    };

public:
    class TcsAwaiter : public Awaiter<void>
    {
    public:
        explicit TcsAwaiter(std::shared_ptr<State> state) noexcept
            : mState(std::move(state))
        {
        }

        bool IsCompleted() const noexcept override
        {
            std::lock_guard<std::mutex> lock(mState->mutex);
            return mState->completed;
        }

        bool OnCompleted(std::coroutine_handle<> continuation) override
        {
            std::lock_guard<std::mutex> lock(mState->mutex);
            if (mState->completed)
            {
                return false;
            }
            mState->waiters.push_back(continuation);
            return true;
        }

        void GetResult() override
        {
            if (mState->exception)
            {
                std::rethrow_exception(mState->exception);
            }
        }

    private:
        std::shared_ptr<State> mState;
    };

    TaskCompletionSource()
        : mState(std::make_shared<State>())
    {
    }

    bool TrySetResult()
    {
        std::vector<std::coroutine_handle<>> waiters;
        {
            std::lock_guard<std::mutex> lock(mState->mutex);
            if (mState->completed)
            {
                return false;
            }
            mState->completed = true;
            waiters.swap(mState->waiters);
        }
        for (std::coroutine_handle<> waiter : waiters)
        {
            waiter.resume();
        }
        return true;
    }

    void SetResult()
    {
        if (!TrySetResult())
        {
            throw std::logic_error("TaskCompletionSource already completed");
        }
    }

    bool TrySetException(std::exception_ptr exception)
    {
        if (!exception)
        {
            return false;
        }

        std::vector<std::coroutine_handle<>> waiters;
        {
            std::lock_guard<std::mutex> lock(mState->mutex);
            if (mState->completed)
            {
                return false;
            }
            mState->completed = true;
            mState->exception = std::move(exception);
            waiters.swap(mState->waiters);
        }
        for (std::coroutine_handle<> waiter : waiters)
        {
            waiter.resume();
        }
        return true;
    }

    void SetException(std::exception_ptr exception)
    {
        if (!TrySetException(std::move(exception)))
        {
            throw std::logic_error("TaskCompletionSource already completed");
        }
    }

    TcsAwaiter GetAwaiter() const noexcept
    {
        return TcsAwaiter(mState);
    }

private:
    std::shared_ptr<State> mState;
};

template <typename T>
struct WhenAnyResult
{
    size_t index = 0;
    T value{};
};

inline AsyncTask<void> WhenAll(std::vector<AsyncTask<void>> tasks)
{
    if (tasks.empty())
    {
        co_return;
    }

    struct Shared
    {
        std::mutex mutex;
        size_t remaining = 0;
        std::exception_ptr exception;
        TaskCompletionSource<void> gate;
    };

    auto shared = std::make_shared<Shared>();
    shared->remaining = tasks.size();

    for (size_t i = 0; i < tasks.size(); ++i)
    {
        [](AsyncTask<void> task, std::shared_ptr<Shared> shared) -> DetachedTask
        {
            try
            {
                co_await std::move(task);
            }
            catch (...)
            {
                std::lock_guard<std::mutex> lock(shared->mutex);
                if (!shared->exception)
                {
                    shared->exception = std::current_exception();
                }
            }

            bool done = false;
            {
                std::lock_guard<std::mutex> lock(shared->mutex);
                done = (--shared->remaining == 0);
            }
            if (done)
            {
                shared->gate.TrySetResult();
            }
        }(std::move(tasks[i]), shared);
    }

    co_await shared->gate.GetAwaiter();
    if (shared->exception)
    {
        std::rethrow_exception(shared->exception);
    }
}

template <typename T>
inline AsyncTask<std::vector<T>> WhenAll(std::vector<AsyncTask<T>> tasks)
{
    if (tasks.empty())
    {
        co_return std::vector<T>{};
    }

    struct Shared
    {
        std::mutex mutex;
        size_t remaining = 0;
        std::exception_ptr exception;
        std::vector<T> results;
        TaskCompletionSource<void> gate;
    };

    auto shared = std::make_shared<Shared>();
    shared->remaining = tasks.size();
    shared->results.resize(tasks.size());

    for (size_t i = 0; i < tasks.size(); ++i)
    {
        [](size_t index, AsyncTask<T> task, std::shared_ptr<Shared> shared) -> DetachedTask
        {
            try
            {
                T value = co_await std::move(task);
                std::lock_guard<std::mutex> lock(shared->mutex);
                shared->results[index] = std::move(value);
            }
            catch (...)
            {
                std::lock_guard<std::mutex> lock(shared->mutex);
                if (!shared->exception)
                {
                    shared->exception = std::current_exception();
                }
            }

            bool done = false;
            {
                std::lock_guard<std::mutex> lock(shared->mutex);
                done = (--shared->remaining == 0);
            }
            if (done)
            {
                shared->gate.TrySetResult();
            }
        }(i, std::move(tasks[i]), shared);
    }

    co_await shared->gate.GetAwaiter();
    if (shared->exception)
    {
        std::rethrow_exception(shared->exception);
    }
    co_return std::move(shared->results);
}

inline AsyncTask<size_t> WhenAny(std::vector<AsyncTask<void>> tasks)
{
    if (tasks.empty())
    {
        throw std::invalid_argument("WhenAny requires at least one task");
    }

    struct Shared
    {
        std::atomic<bool> completed{ false };
        size_t index = 0;
        std::exception_ptr exception;
        TaskCompletionSource<void> gate;
    };

    auto shared = std::make_shared<Shared>();

    for (size_t i = 0; i < tasks.size(); ++i)
    {
        [](size_t index, AsyncTask<void> task, std::shared_ptr<Shared> shared) -> DetachedTask
        {
            std::exception_ptr exception;
            try
            {
                co_await std::move(task);
            }
            catch (...)
            {
                exception = std::current_exception();
            }

            if (!shared->completed.exchange(true))
            {
                shared->index = index;
                shared->exception = exception;
                shared->gate.TrySetResult();
            }
        }(i, std::move(tasks[i]), shared);
    }

    co_await shared->gate.GetAwaiter();
    if (shared->exception)
    {
        std::rethrow_exception(shared->exception);
    }
    co_return shared->index;
}

template <typename T>
inline AsyncTask<WhenAnyResult<T>> WhenAny(std::vector<AsyncTask<T>> tasks)
{
    if (tasks.empty())
    {
        throw std::invalid_argument("WhenAny requires at least one task");
    }

    struct Shared
    {
        std::atomic<bool> completed{ false };
        size_t index = 0;
        std::optional<T> value;
        std::exception_ptr exception;
        TaskCompletionSource<void> gate;
    };

    auto shared = std::make_shared<Shared>();

    for (size_t i = 0; i < tasks.size(); ++i)
    {
        [](size_t index, AsyncTask<T> task, std::shared_ptr<Shared> shared) -> DetachedTask
        {
            std::optional<T> value;
            std::exception_ptr exception;
            try
            {
                value = co_await std::move(task);
            }
            catch (...)
            {
                exception = std::current_exception();
            }

            if (!shared->completed.exchange(true))
            {
                shared->index = index;
                shared->value = std::move(value);
                shared->exception = exception;
                shared->gate.TrySetResult();
            }
        }(i, std::move(tasks[i]), shared);
    }

    co_await shared->gate.GetAwaiter();
    if (shared->exception)
    {
        std::rethrow_exception(shared->exception);
    }
    co_return WhenAnyResult<T>{ shared->index, std::move(*shared->value) };
}
