#include "socket_utils.hpp"
#include <cstdio>

#include <sys/socket.h>

bool sendAll(int socket, const char* data, std::size_t length) {

    std::size_t totalSent = 0;

    while (totalSent < length) {

        ssize_t bytesSent = send(
            socket,
            data + totalSent,
            length - totalSent,
            0
        );

        if (bytesSent <= 0) {
            return false;
        }

        totalSent += bytesSent;
    }

    return true;
}


bool recvAll(int socket, char* data, std::size_t length) {

    std::size_t totalReceived = 0;

    while (totalReceived < length) {

        ssize_t bytesReceived = recv(
            socket,
            data + totalReceived,
            length - totalReceived,
            0
        );

        if (bytesReceived == 0) {
            return false;
        }

        if (bytesReceived < 0) {
            std::perror("recv");
            return false;
        }

        totalReceived += bytesReceived;
    }

    return true;
}