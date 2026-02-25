#pragma once

#include <vector>
#include <atomic>
#include <cstddef>

template<typename T>
class RingBuffer {
public:
    RingBuffer(size_t size) : m_buffer(size), m_read(0), m_write(0) {}

    bool push(const T& value) {
        size_t current_write = m_write.load(std::memory_order_relaxed);
        size_t next_write = (current_write + 1) % m_buffer.size();
        if (next_write == m_read.load(std::memory_order_acquire)) {
            return false;
        }
        m_buffer[current_write] = value;
        m_write.store(next_write, std::memory_order_release);
        return true;
    }

    bool pop(T& value) {
        auto current_read = m_read.load(std::memory_order_relaxed);
        if (current_read == m_write.load(std::memory_order_acquire)) {
            return false;
        }
        value = m_buffer[current_read];
        m_read.store((current_read + 1) % m_buffer.size(), std::memory_order_release);
        return true;
    }

private:
    std::vector<T> m_buffer;
    alignas(64) std::atomic<size_t> m_read;
    alignas(64) std::atomic<size_t> m_write;
};
