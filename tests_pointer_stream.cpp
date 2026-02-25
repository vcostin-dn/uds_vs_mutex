#include "utils.hpp"

void test_pointer_socket_queue_stream(sbench::SBench& bench)
{
    DEBUG_PRINT("test_pointer_socket_queue_stream");
    SocketQueueStream socket_queue("/tmp/mvi_socket_queue");
    generate_messages();

    std::thread producer([&socket_queue]() {
        socket_queue.waitForConsumer();
        for (int i = 0; i < NUM_MESSAGES; ++i) {
            socket_queue.sendPointer(&messages[i]);
        }

    });

    std::thread consumer([&socket_queue]() {
        socket_queue.connectToProducer();
        for (int i = 0; i < NUM_MESSAGES; ++i) {
            socket_queue.receivePointer([](void* pointer) {
                workload_pointer(pointer);
            });
        }
    });

    bench.run("socket queue stream", [&]() {
        DEBUG_PRINT("Waiting for threads to join");
        producer.join();
        consumer.join();
    });

    DEBUG_PRINT("Done");
    DEBUG_PRINT("--------------------------------\n\n");
}

void test_pointer_thread_safe_queue(sbench::SBench& bench)
{
    DEBUG_PRINT("test_pointer_thread_safe_queue");
    ThreadSafeQueue<void*> thread_safe_queue;
    generate_messages();

    std::thread producer([&thread_safe_queue]() {
        for (int i = 0; i < NUM_MESSAGES; ++i) {
            thread_safe_queue.addMessage(&messages[i]);
        }
    });

    std::thread consumer([&thread_safe_queue]() {
        for (int i = 0; i < NUM_MESSAGES; ++i) {
            thread_safe_queue.waitMessages();
            thread_safe_queue.processMessages([](void* pointer) {
                workload_pointer(pointer);
            });
        }
    });

    bench.run("thread safe queue", [&]() {
        DEBUG_PRINT("Waiting for threads to join");
        producer.join();
        consumer.join();
    });

    DEBUG_PRINT("Done");
    DEBUG_PRINT("--------------------------------\n\n");
}

void test_pointer_thread_safe_queue_optimized(sbench::SBench& bench)
{
    DEBUG_PRINT("test_pointer_thread_safe_queue_optimized");
    ThreadSafeQueueOptimized<void*> thread_safe_queue_optimized;
    generate_messages();

    std::thread producer([&thread_safe_queue_optimized]() {
        for (int i = 0; i < NUM_MESSAGES; i++) {
            thread_safe_queue_optimized.addMessage(&messages[i]);
        }
    });

    int processed_messages = 0;
    std::thread consumer([&thread_safe_queue_optimized, &processed_messages]() {
        while (processed_messages != NUM_MESSAGES) {
            thread_safe_queue_optimized.waitMessages();
            size_t num_messages = thread_safe_queue_optimized.processMessages([](void* pointer) {
                workload_pointer(pointer);
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