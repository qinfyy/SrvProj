#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <future>
#include <functional>
#include <condition_variable>
#include <atomic>
#include <mutex>

class ThreadPool
{
public:
    explicit ThreadPool(size_t threadCount) : stop(false), tasksRemaining(0) {
        for (size_t i = 0; i < threadCount; ++i)
        {
            workers.emplace_back([this]
                {
                    while (true)
                    {
                        std::function<void()> task;

                        {
                            std::unique_lock lock(queueMutex);

                            condition.wait(lock, [this]
                                {
                                    return stop || !tasks.empty();
                                });

                            if (stop && tasks.empty())
                                return;

                            task = std::move(tasks.front());
                            tasks.pop();
                        }

                        task();

                        if (--tasksRemaining == 0)
                        {
                            std::unique_lock lock(waitMutex);
                            waitCondition.notify_all();
                        }
                    }
                });
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock lock(queueMutex);
            stop = true;
        }

        condition.notify_all();

        for (std::thread& worker : workers)
            worker.join();
    }

    template<class F, class... Args>
    std::future<typename std::invoke_result<F, Args...>::type> enqueue(F&& f, Args&&... args) {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>
            (
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );

        std::future<return_type> res = task->get_future();

        {
            std::unique_lock lock(queueMutex);

            if (stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");

            tasks.emplace([task]()
                {
                    (*task)();
                });

            tasksRemaining++;
        }

        condition.notify_one();

        return res;
    }

    void waitAll() {
        std::unique_lock lock(waitMutex);

        waitCondition.wait(lock, [this]
            {
                return tasksRemaining == 0;
            });
    }

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<size_t> tasksRemaining;
    std::mutex waitMutex;
    std::condition_variable waitCondition;
    bool stop;
};
