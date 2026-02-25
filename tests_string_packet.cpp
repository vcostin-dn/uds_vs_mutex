#include "utils.hpp"

void test_string_socket_queue_packet(sbench::SBench& bench)
{
    DEBUG_PRINT("test_string_socket_queue_packet");
    SocketQueuePacket socket_queue("/tmp/mvi_socket_queue");

    std::thread producer([&socket_queue]() {
        socket_queue.waitForConsumer();
        for (int i = 0; i < NUM_MESSAGES; i++) {
            socket_queue.sendMessage("Hello, world! " + std::to_string(i));
        }
    });

    std::thread consumer([&socket_queue]() {
        socket_queue.connectToProducer();
        for (int i = 0; i < NUM_MESSAGES; i++) {
            socket_queue.receiveMessage([](std::string& message) {
                workload(message);
            });
        }
    });

    bench.run("socket queue packet", [&]() {
        DEBUG_PRINT("Waiting for threads to join");
        producer.join();
        consumer.join();
    });

    DEBUG_PRINT("Done");
    DEBUG_PRINT("--------------------------------\n\n");
}

void test_string_thread_safe_queue(sbench::SBench& bench)
{
    DEBUG_PRINT("test_string_thread_safe_queue");
    ThreadSafeQueue<std::string> thread_safe_queue;

    std::thread producer([&thread_safe_queue]() {
        for (int i = 0; i < NUM_MESSAGES; i++) {
            thread_safe_queue.addMessage("Hello, world! " + std::to_string(i));
        }
    });

    std::thread consumer([&thread_safe_queue]() {
        for (int i = 0; i < NUM_MESSAGES; ++i) {
            thread_safe_queue.waitMessages();
            thread_safe_queue.processMessages([](std::string& message) {
                workload(message);
            });
        }
    });

    bench.run("thread safe queue", [&]() {
        producer.join();
        consumer.join();
    });

    DEBUG_PRINT("Done");
    DEBUG_PRINT("--------------------------------\n\n");
}

void test_string_thread_safe_queue_optimized(sbench::SBench& bench)
{
    DEBUG_PRINT("test_string_thread_safe_queue_optimized");
    ThreadSafeQueueOptimized<std::string> thread_safe_queue_optimized;

    std::thread producer([&thread_safe_queue_optimized]() {
        for (int i = 0; i < NUM_MESSAGES; i++) {
            thread_safe_queue_optimized.addMessage("Hello, world! " + std::to_string(i));
        }
    });

    int processed_messages = 0;
    std::thread consumer([&thread_safe_queue_optimized, &processed_messages]() {
        while (processed_messages != NUM_MESSAGES) {
            thread_safe_queue_optimized.waitMessages();
            size_t num_messages = thread_safe_queue_optimized.processMessages([](std::string& message) {
                workload(message);
            });

            processed_messages += num_messages;

            DEBUG_BLOCK(
                if (num_messages != 0)
                    std::cout << "Processed " << num_messages << " messages\n";
            );
        }
    });

    bench.run("thread safe queue optimized", [&]() {
        producer.join();
        consumer.join();
    });

    DEBUG_PRINT("Done");
    DEBUG_PRINT("--------------------------------\n\n");
}
