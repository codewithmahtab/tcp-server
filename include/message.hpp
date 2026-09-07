#pragma once

#include <cstdint>
#include <string>

enum class MessageType : std::uint32_t {
    TEXT = 1,
    PING = 2,
    RESPONSE = 3
};

struct Message {
    MessageType type;
    std::string payload;
};

bool sendMessage(int socket, const Message& message);

bool receiveMessage(int socket, Message& message);