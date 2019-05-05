#pragma once

#include <queue>
#include <mutex>

template<typename T>
class SafeQueue
{
public:
    SafeQueue();
    SafeQueue(const SafeQueue& copy);

    void Push(T value);
    bool TryPop(T& value);
    bool Empty() const;
    uint64_t Size() const;

private:
    std::queue<T> m_queue;
    mutable std::mutex m_mutex;
};

template<typename T>
SafeQueue<T>::SafeQueue()
{}

template<typename T>
SafeQueue<T>::SafeQueue(const SafeQueue<T>& copy)
{
    std::lock_guard<std::mutex> lock(copy.m_mutex);
    m_queue = copy.m_queue;
}

template<typename T>
void SafeQueue<T>::Push(T value)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_queue.push(std::move(value));
}

template<typename T>
bool SafeQueue<T>::TryPop(T& value)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_queue.empty())
        return false;

    value = m_queue.front();
    m_queue.pop();

    return true;
}

template<typename T>
bool SafeQueue<T>::Empty() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.empty();
}

template<typename T>
uint64_t SafeQueue<T>::Size() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_queue.size();
}