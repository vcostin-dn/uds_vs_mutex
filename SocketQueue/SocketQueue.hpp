#pragma once

#include <string>
#include <functional>

class SocketQueuePacket {
public:
    SocketQueuePacket(const std::string path);
    ~SocketQueuePacket();

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

class SocketQueueStream {
public:
    SocketQueueStream(const std::string path);
    ~SocketQueueStream();

    void waitForConsumer();
    void connectToProducer();

    // method used only by the producer thread
    void sendMessage(const std::string& message);

    // method used only by the consumer thread
    void receiveMessage(std::function<void(std::string&)> consumeCb);

    void sendPointer(void* pointer);
    void receivePointer(std::function<void(void*)> consumeCb);

    void cleanup();

private:
    int mListenFd = -1;
    int mConsumerFd = -1;
    int mProducerFd = -1;

    std::string mSocketPath;
};