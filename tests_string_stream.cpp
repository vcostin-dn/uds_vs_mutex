#include "utils.hpp"


void test_string_socket_queue_stream(sbench::SBench& bench)
{
    DEBUG_PRINT("test_string_socket_queue_stream");
    SocketQueueStream socket_queue("/tmp/mvi_socket_queue");

    std::thread producer([&socket_queue]() {
        socket_queue.waitForConsumer();
        for (int i = 0; i < NUM_MESSAGES; i++) {
            socket_queue.sendMessage("Hello, world!");
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

    bench.run("socket queue stream", [&]() {
        DEBUG_PRINT("Waiting for threads to join");
        producer.join();
        consumer.join();
    });

    DEBUG_PRINT("Done");
    DEBUG_PRINT("--------------------------------\n\n");
}