#include "ThreadSafeQueue.hpp"

void ThreadSafeQueue::addMessage(const std::string& message)
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

void ThreadSafeQueue::waitMessages()
{
    std::unique_lock<std::mutex> lock(m_queue_mutex);
    m_queue_condition.wait(lock, [this] { return !m_queue.empty(); });
}

size_t ThreadSafeQueue::processMessages(std::function<void(std::string&)> consumeCb)
{
    std::string message;
    {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        message = m_queue.back();
        m_queue.pop_back();
    }
    consumeCb(message);
    return 1;
}

void ThreadSafeQueueOptimized::addMessage(const std::string& message)
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

void ThreadSafeQueueOptimized::waitMessages()
{
    std::unique_lock<std::mutex> lock(m_adding_queue_mutex);
    m_adding_queue_condition.wait(lock, [this] { return !m_adding_queue.empty(); });
}

size_t ThreadSafeQueueOptimized::processMessages(std::function<void(std::string&)> consumeCb)
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