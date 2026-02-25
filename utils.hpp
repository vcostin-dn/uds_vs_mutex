#pragma once

#include <string>
#include <vector>
#include <thread>
#include <sbench.hpp>
#include "ThreadSafeQueue/ThreadSafeQueue.hpp"
#include "SocketQueue/SocketQueue.hpp"

#ifdef DEBUG
    constexpr int NUM_MESSAGES = 3;
    constexpr int NUM_ITERATIONS = 2;
    #define DEBUG_PRINT(x) std::cout << x << std::endl;
    #define DEBUG_BLOCK(x) do { x; } while (0)
#else
    constexpr int NUM_MESSAGES = 1000000;
    constexpr int NUM_ITERATIONS = 15;
    #define DEBUG_PRINT(x)
    #define DEBUG_BLOCK(x) do { } while (0)
#endif

void workload(std::string& message);
void workload_pointer(void* pointer);

extern std::vector<std::string> messages;
void generate_messages();


void test_string_socket_queue_packet(sbench::SBench& bench);
void test_string_thread_safe_queue(sbench::SBench& bench);
void test_string_thread_safe_queue_optimized(sbench::SBench& bench);

void test_string_socket_queue_stream(sbench::SBench& bench);

void test_pointer_socket_queue_stream(sbench::SBench& bench);
void test_pointer_thread_safe_queue(sbench::SBench& bench);
void test_pointer_thread_safe_queue_optimized(sbench::SBench& bench);