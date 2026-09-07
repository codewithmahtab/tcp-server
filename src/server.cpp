#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "message.hpp"

int main() {

    int serverSocket = socket(
        AF_INET,
        SOCK_STREAM,
        0
    );

    if (serverSocket == -1) {
        perror("socket");
        return 1;
    }

    sockaddr_in serverAddress{};

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    if (bind(
            serverSocket,
            reinterpret_cast<sockaddr*>(&serverAddress),
            sizeof(serverAddress)
        ) == -1) {

        perror("bind");
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) == -1) {
        perror("listen");
        close(serverSocket);
        return 1;
    }

    std::cout << "Server listening on port 8080...\n";

    int clientSocket = accept(
        serverSocket,
        nullptr,
        nullptr
    );

    if (clientSocket == -1) {
        perror("accept");
        close(serverSocket);
        return 1;
    }

    std::cout << "Client connected!\n";

    Message message;

    if (receiveMessage(clientSocket, message)) {

        std::cout << "Received message: "
                  << message.payload
                  << '\n';

        Message response{
            MessageType::RESPONSE,
            "Message received successfully!"
        };

        sendMessage(clientSocket, response);
    }

    close(clientSocket);
    close(serverSocket);

    return 0;
}