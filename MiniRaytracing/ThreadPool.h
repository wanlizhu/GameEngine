#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <future>
#include <string>
#include <vector>
#include <deque>
#include <mutex>
#include <functional>
#include <stdexcept>

class ThreadPool
{
public:
    ThreadPool();
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    virtual ~ThreadPool() { release(); }

    void release();

    template<typename Func, typename... Args>
    std::future<void> enqueue(Func&& func, Args&&... args);

private:
    std::vector<std::thread> _workers;
    std::deque<std::function<void()>> _tasks;
    std::mutex _mutex;
    std::condition_variable _cond;
    std::atomic_bool _quit;
};

inline ThreadPool::ThreadPool()
    : _quit(false)
{
    for (int i = 0; i < std::thread::hardware_concurrency(); i++)
    {
        _workers.emplace_back([this]() {
            while (true)
            {
                std::function<void()> task;

                if (true)
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    _cond.wait(lock, [this]() { return _quit || !_tasks.empty(); });
                    
                    if (_quit && _tasks.empty())
                    {
                        return;
                    }

                    task = std::move(_tasks.front());
                    _tasks.pop_front();
                }

                task();
            }
        });
    }
}

inline void ThreadPool::release()
{
    _quit = true;
    _cond.notify_all();

    for (auto& worker : _workers)
        worker.join();
}

template<typename Func, typename... Args>
std::future<void> ThreadPool::enqueue(Func&& func, Args&&... args)
{
    auto boundFunc = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
    auto packagedTask = std::make_shared<std::packaged_task<void()>>(boundFunc);
    auto result = packagedTask->get_future();

    if (true)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _tasks.emplace_back([packagedTask]() { (*packagedTask)(); });
    }

    _cond.notify_one();

    return result;
}