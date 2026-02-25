#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>

template<typename T>
class ThreadSafeQueue {
public:
    // method used only by the producer thread
    void addMessage(const T& message)
    {
        m_queue_mutex.lock();
        bool was_empty = false;

        was_empty = m_queue.empty();
        m_queue.push_back(message);
        if (was_empty) {
            m_queue_mutex.unlock();
            m_queue_condition.notify_one();
        } else {
            m_queue_mutex.unlock();
        }
    }

    // methods used only by the consumer thread
    void waitMessages()
    {
        std::unique_lock<std::mutex> lock(m_queue_mutex);
        m_queue_condition.wait(lock, [this] { return !m_queue.empty(); });
    }

    size_t processMessages(std::function<void(T&)> consumeCb)
    {
        T message;
        {
            std::lock_guard<std::mutex> lock(m_queue_mutex);
            message = m_queue.back();
            m_queue.pop_back();
        }
        consumeCb(message);
        return 1;
    }

private:
    std::vector<T> m_queue;

    std::mutex m_queue_mutex;
    std::condition_variable m_queue_condition;
};

// A double buffer optimization for the queue
// Only adding queue is protected by the locking mechanism
// The consumer consumes the whole queue at once, processing all messages by batch
template<typename T>
class ThreadSafeQueueOptimized {
public:
    // method used only by the producer thread
    void addMessage(const T& message)
    {
        m_adding_queue_mutex.lock();
        bool was_empty = false;

        was_empty = m_adding_queue.empty();
        m_adding_queue.push_back(message);
        if (was_empty) {
            m_adding_queue_mutex.unlock();
            m_adding_queue_condition.notify_one();
        } else {
            m_adding_queue_mutex.unlock();
        }
    }

    // methods used only by the consumer thread
    void waitMessages()
    {
        std::unique_lock<std::mutex> lock(m_adding_queue_mutex);
        m_adding_queue_condition.wait(lock, [this] { return !m_adding_queue.empty(); });
    }

    size_t processMessages(std::function<void(T&)> consumeCb)
    {
        {
            std::lock_guard<std::mutex> lock(m_adding_queue_mutex);
            std::swap(m_adding_queue, m_consuming_queue);
        }

        int ret = m_consuming_queue.size();
        for (auto& message : m_consuming_queue) {
            consumeCb(message);
        }
        m_consuming_queue.clear();
        return ret;
    }

private:
    std::vector<T> m_adding_queue;
    std::vector<T> m_consuming_queue;

    std::mutex m_adding_queue_mutex;
    std::condition_variable m_adding_queue_condition;
};
