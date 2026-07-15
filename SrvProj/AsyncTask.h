#pragma once

#include <coroutine>
#include <exception>
#include <optional>
#include <utility>

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

    struct Awaiter
    {
        Handle handle;

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

    Awaiter operator co_await() const noexcept
    {
        return { mHandle };
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

    struct Awaiter
    {
        Handle handle;

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

    Awaiter operator co_await() const noexcept
    {
        return { mHandle };
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
