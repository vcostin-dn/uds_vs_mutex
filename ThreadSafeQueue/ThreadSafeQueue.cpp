#include "ThreadSafeQueue.hpp"

void ThreadSafeQueue::addMessage(const std::string& message)
{
    bool was_empty = false;
    {
        std::lock_guard<std::mutex> lock(m_adding_queue_mutex);

        was_empty = m_adding_queue.empty();
        m_adding_queue.push_back(message);
    }

    if (was_empty)
        m_adding_queue_condition.notify_one();
}

void ThreadSafeQueue::waitMessages()
{
    std::unique_lock<std::mutex> lock(m_adding_queue_mutex);
    m_adding_queue_condition.wait(lock, [this] { return !m_adding_queue.empty(); });
}

size_t ThreadSafeQueue::processMessages(std::function<void(std::string&)> consumeCb)
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