#pragma once

#include <string>
#include <functional>

class SocketQueue {
public:
    SocketQueue(const std::string path);
    ~SocketQueue();

    void waitForConsumer();
    void connectToProducer();

    // method used only by the producer thread
    void sendMessage(const std::string& message);

    // method used only by the consumer thread
    void receiveMessage(std::function<void(std::string&)> consumeCb);

    void cleanup();

private:
    int mListenFd = -1;
    int mConsumerFd = -1;
    int mProducerFd = -1;

    std::string mSocketPath;
};