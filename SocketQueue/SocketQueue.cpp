#include "SocketQueue.hpp"

#ifdef DEBUG
#include <iostream>
#endif

#include <string>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <cassert>

#define STRICT_MESSAGE_SIZE 13 // "Hello, world!"

SocketQueuePacket::SocketQueuePacket(const std::string path)
{
    auto ret = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    mListenFd = ret;

    unlink(path.c_str());
    struct sockaddr_un addr = {};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);
    ret = bind(mListenFd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
    assert(ret == 0);
    ret = listen(mListenFd, 1);
    assert(ret == 0);
    mSocketPath = path;
}

SocketQueuePacket::~SocketQueuePacket()
{
    if (mListenFd >= 0) {
        close(mListenFd);
        mListenFd = -1;
    }
    if (mConsumerFd >= 0) {
        close(mConsumerFd);
        mConsumerFd = -1;
    }
    if (mProducerFd >= 0) {
        close(mProducerFd);
        mProducerFd = -1;
    }
}

void SocketQueuePacket::waitForConsumer()
{
#ifdef DEBUG
    std::cout << "Waiting for consumer" << std::endl;
#endif
    if (mConsumerFd >= 0) {
        close(mConsumerFd);
        mConsumerFd = -1;
    }

    auto ret = accept(mListenFd, nullptr, nullptr);
    assert(ret >= 0);
    mConsumerFd = ret;
#ifdef DEBUG
    std::cout << "Consumer connected" << std::endl;
#endif
}

void SocketQueuePacket::connectToProducer()
{
#ifdef DEBUG
    std::cout << "Connecting to producer" << std::endl;
#endif
    if (mProducerFd >= 0) {
        close(mProducerFd);
        mProducerFd = -1;
    }

    auto ret = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    mProducerFd = ret;

    struct sockaddr_un addr = {};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, mSocketPath.c_str(), sizeof(addr.sun_path) - 1);
    ret = connect(mProducerFd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
    assert(ret == 0);
#ifdef DEBUG
    std::cout << "Connected to producer" << std::endl;
#endif
}

void SocketQueuePacket::sendMessage(const std::string& message)
{
    #define MESSAGE_SIZE 1024
    char buf[MESSAGE_SIZE];
    strncpy(buf, message.c_str(), message.size());

    struct msghdr msg = {};
    struct iovec iov;
    iov.iov_base = buf;
    iov.iov_len = message.size();
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    ssize_t n = sendmsg(mConsumerFd, &msg, MSG_NOSIGNAL);
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return;
        }
#ifdef DEBUG
        std::cout << "sendmsg failed: " << strerror(errno) << std::endl;
#endif
        return;
    }
    assert(n == message.size());
}

void SocketQueuePacket::receiveMessage(std::function<void(std::string&)> consumeCb)
{
    #define MESSAGE_SIZE 1024
    char buf[MESSAGE_SIZE];

    struct msghdr msg = {};
    struct iovec iov;
    iov.iov_base = buf;
    iov.iov_len = MESSAGE_SIZE;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    ssize_t n = recvmsg(mProducerFd, &msg, 0);
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return;
        }
#ifdef DEBUG
        std::cout << "recvmsg failed: " << strerror(errno) << std::endl;
#endif
        return;
    }

    if (n == 0) {
        return;
    }

    if (msg.msg_flags & MSG_TRUNC) {
#ifdef DEBUG
        std::cout << "message truncated" << std::endl;
#endif
        return;
    }

    std::string message(buf, n);
    consumeCb(message);
}

void SocketQueuePacket::cleanup()
{
    if (mConsumerFd >= 0) {
        close(mConsumerFd);
        mConsumerFd = -1;
    }
    if (mProducerFd >= 0) {
        close(mProducerFd);
        mProducerFd = -1;
    }
}

SocketQueueStream::SocketQueueStream(const std::string path)
{
    auto ret = socket(AF_UNIX, SOCK_STREAM, 0);
    mListenFd = ret;

    unlink(path.c_str());
    struct sockaddr_un addr = {};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path.c_str(), sizeof(addr.sun_path) - 1);
    ret = bind(mListenFd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
    assert(ret == 0);
    ret = listen(mListenFd, 1);
    assert(ret == 0);
    mSocketPath = path;
}

SocketQueueStream::~SocketQueueStream()
{
    if (mListenFd >= 0) {
        close(mListenFd);
        mListenFd = -1;
    }
    if (mConsumerFd >= 0) {
        close(mConsumerFd);
        mConsumerFd = -1;
    }
    if (mProducerFd >= 0) {
        close(mProducerFd);
        mProducerFd = -1;
    }
}

void SocketQueueStream::waitForConsumer()
{
#ifdef DEBUG
    std::cout << "Waiting for consumer" << std::endl;
#endif
    if (mConsumerFd >= 0) {
        close(mConsumerFd);
        mConsumerFd = -1;
    }

    auto ret = accept(mListenFd, nullptr, nullptr);
    assert(ret >= 0);
    mConsumerFd = ret;
#ifdef DEBUG
    std::cout << "Consumer connected" << std::endl;
#endif
}

void SocketQueueStream::connectToProducer()
{
#ifdef DEBUG
    std::cout << "Connecting to producer" << std::endl;
#endif
    if (mProducerFd >= 0) {
        close(mProducerFd);
        mProducerFd = -1;
    }

    auto ret = socket(AF_UNIX, SOCK_STREAM, 0);
    mProducerFd = ret;

    struct sockaddr_un addr = {};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, mSocketPath.c_str(), sizeof(addr.sun_path) - 1);
    ret = connect(mProducerFd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
    assert(ret == 0);
#ifdef DEBUG
    std::cout << "Connected to producer" << std::endl;
#endif
}

void SocketQueueStream::sendMessage(const std::string& message)
{
    ssize_t n = write(mConsumerFd, message.c_str(), message.size());
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return;
        }
#ifdef DEBUG
        std::cout << "sendmsg failed: " << strerror(errno) << std::endl;
#endif
        return;
    }
    assert(n == message.size());
}

void SocketQueueStream::receiveMessage(std::function<void(std::string&)> consumeCb)
{
    #define MESSAGE_SIZE 1024
    char buf[MESSAGE_SIZE];

    ssize_t n = read(mProducerFd, buf, STRICT_MESSAGE_SIZE);
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return;
        }
#ifdef DEBUG
        std::cout << "recvmsg failed: " << strerror(errno) << std::endl;
#endif
        return;
    }

    if (n == 0) {
        return;
    }

    assert(n == STRICT_MESSAGE_SIZE);

    buf[n] = '\0';
    std::string message(buf, n);
    consumeCb(message);
}

void SocketQueueStream::sendPointer(void* pointer)
{
    ssize_t n = write(mConsumerFd, &pointer, sizeof(pointer));
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return;
        }
#ifdef DEBUG
        std::cout << "sendmsg failed: " << strerror(errno) << std::endl;
#endif
        return;
    }
    assert(n == sizeof(pointer));
}

void SocketQueueStream::receivePointer(std::function<void(void*)> consumeCb)
{
    void* pointer = nullptr;

    ssize_t n = read(mProducerFd, &pointer, sizeof(pointer));
    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return;
        }
#ifdef DEBUG
        std::cout << "recvmsg failed: " << strerror(errno) << std::endl;
#endif
        return;
    }

    if (n == 0) {
        return;
    }

    assert(n == sizeof(pointer));
    consumeCb(pointer);
}

void SocketQueueStream::cleanup()
{
    if (mConsumerFd >= 0) {
        close(mConsumerFd);
        mConsumerFd = -1;
    }
    if (mProducerFd >= 0) {
        close(mProducerFd);
        mProducerFd = -1;
    }
}
