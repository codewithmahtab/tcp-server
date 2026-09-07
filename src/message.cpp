#include "message.hpp"
#include "socket_utils.hpp"

#include <arpa/inet.h>
#include <cstdint>
#include <vector>
#include <iostream>

constexpr std::uint32_t MAX_MESSAGE_SIZE = 4096;

bool sendMessage(int socket, const Message& message) {

    std::uint32_t type =
        htonl(static_cast<std::uint32_t>(message.type));

    std::uint32_t length =
        htonl(static_cast<std::uint32_t>(message.payload.size()));

    if (!sendAll(
            socket,
            reinterpret_cast<const char*>(&type),
            sizeof(type)
        )) {
        return false;
    }

    if (!sendAll(
            socket,
            reinterpret_cast<const char*>(&length),
            sizeof(length)
        )) {
        return false;
    }

    if (!message.payload.empty()) {

        if (!sendAll(
                socket,
                message.payload.data(),
                message.payload.size()
            )) {
            return false;
        }
    }

    return true;
}

bool receiveMessage(int socket, Message& message) {

    std::uint32_t networkType;
    std::uint32_t networkLength;

    if (!recvAll(
            socket,
            reinterpret_cast<char*>(&networkType),
            sizeof(networkType)
        )) {
        return false;
    }

    if (!recvAll(
            socket,
            reinterpret_cast<char*>(&networkLength),
            sizeof(networkLength)
        )) {
        return false;
    }

    std::uint32_t type = ntohl(networkType);
    std::uint32_t length = ntohl(networkLength);

    if (type < 1 || type > 3) {
        std::cerr << "Invalid message type\n";
        return false;
    }

    if (length > MAX_MESSAGE_SIZE) {
        std::cerr << "Message too large\n";
        return false;
    }

    message.type = static_cast<MessageType>(type);

    std::string payload(length, '\0');

    if (length > 0) {

        if (!recvAll(
                socket,
                payload.data(),
                length
            )) {
            return false;
        }
    }

    message.payload = std::move(payload);

    return true;
}