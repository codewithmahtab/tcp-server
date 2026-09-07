#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include <utility>
#include <atomic>
#include <csignal>
#include <cerrno>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "message.hpp"
#include "thread_safe_queue.hpp"


struct ClientMessage {

    int clientSocket;

    Message message;
};


ThreadSafeQueue<ClientMessage> messageQueue;


std::mutex statsMutex;

int totalMessages = 0;


std::atomic<bool> running{true};

int serverSocket = -1;


void handleSignal(int signal) {

    if (signal == SIGINT) {

        running.store(false);

        // Wake up accept().
        if (serverSocket != -1) {

            shutdown(
                serverSocket,
                SHUT_RDWR
            );
        }
    }
}


void handleClient(int clientSocket) {

    std::cout << "Client connected!\n";

    Message message;


    if (!receiveMessage(clientSocket, message)) {

        std::cerr << "Failed to receive message.\n";

        close(clientSocket);

        return;
    }


    std::cout
        << "Received: "
        << message.payload
        << '\n';


    {
        std::lock_guard<std::mutex> lock(statsMutex);

        totalMessages++;
    }


    ClientMessage task{
        clientSocket,
        std::move(message)
    };


    if (!messageQueue.push(std::move(task))) {

        std::cerr << "Queue is stopped.\n";

        close(clientSocket);

        return;
    }


    std::cout << "Message added to queue.\n";
}


void workerFunction(int workerId) {

    while (true) {

        ClientMessage task;


        if (!messageQueue.waitAndPop(task)) {

            std::cout
                << "Worker "
                << workerId
                << " shutting down.\n";

            break;
        }


        std::cout
            << "Worker "
            << workerId
            << " processing: "
            << task.message.payload
            << '\n';


        std::this_thread::sleep_for(
            std::chrono::seconds(5)
        );


        Message response{
            MessageType::RESPONSE,
            "Processed by worker " +
            std::to_string(workerId)
        };


        sendMessage(
            task.clientSocket,
            response
        );


        close(task.clientSocket);


        std::cout
            << "Worker "
            << workerId
            << " finished.\n";
    }
}


int main() {

    // Register Ctrl+C handler.
    std::signal(
        SIGINT,
        handleSignal
    );


    serverSocket = socket(
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


    if (listen(serverSocket, 10) == -1) {

        perror("listen");

        close(serverSocket);

        return 1;
    }


    std::cout
        << "Server listening on port 8080...\n";

    std::cout
        << "Press Ctrl+C to stop the server.\n";


    const int WORKER_COUNT = 4;

    std::vector<std::thread> workers;


    for (int i = 0; i < WORKER_COUNT; ++i) {

        workers.emplace_back(
            workerFunction,
            i + 1
        );
    }


    while (running.load()) {

        int clientSocket = accept(
            serverSocket,
            nullptr,
            nullptr
        );


        if (clientSocket == -1) {

            if (!running.load()) {
                break;
            }


            if (errno == EINTR) {
                continue;
            }


            perror("accept");

            continue;
        }


        if (!running.load()) {

            close(clientSocket);

            break;
        }


        std::thread clientThread(
            handleClient,
            clientSocket
        );


        clientThread.detach();
    }


    std::cout
        << "\nStopping server...\n";


    close(serverSocket);

    serverSocket = -1;


    // Tell workers that no more work
    // will be added.
    messageQueue.shutdown();


    // Wait for all workers to exit.
    for (auto& worker : workers) {

        if (worker.joinable()) {

            worker.join();
        }
    }


    std::cout
        << "Total messages processed: "
        << totalMessages
        << '\n';


    std::cout
        << "Server stopped cleanly.\n";


    return 0;
}