#include <iostream>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "message.hpp"

int main() {

    int clientSocket = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (clientSocket == -1) {
        perror("socket");
        return 1;
    }

    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);

    if (inet_pton(
            AF_INET,
            "127.0.0.1",
            &serverAddress.sin_addr
        ) <= 0) {

        std::cerr << "Invalid server address\n";
        close(clientSocket);
        return 1;
    }

    if (connect(
            clientSocket,
            reinterpret_cast<sockaddr*>(&serverAddress),
            sizeof(serverAddress)
        ) == -1) {

        perror("connect");
        close(clientSocket);
        return 1;
    }

    std::cout << "Connected to server!\n";

    Message message{
        MessageType::TEXT,
        "Hello from C++ client!"
    };

    sendMessage(clientSocket, message);

    Message response;

    if (receiveMessage(clientSocket, response)) {

        std::cout << "Server response: "
                  << response.payload
                  << '\n';
    }

    close(clientSocket);

    return 0;
}