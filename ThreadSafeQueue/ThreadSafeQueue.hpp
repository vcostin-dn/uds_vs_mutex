#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <functional>

class ThreadSafeQueue {
public:
    // method used only by the producer thread
    void addMessage(const std::string& message);

    // methods used only by the consumer thread
    void waitMessages();
    size_t processMessages(std::function<void(std::string&)> consumeCb);

private:
    std::vector<std::string> m_adding_queue;
    std::vector<std::string> m_consuming_queue;

    std::mutex m_adding_queue_mutex;
    std::condition_variable m_adding_queue_condition;
};