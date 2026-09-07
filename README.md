# Multithreaded TCP Network Service

A concurrent TCP client-server system built in C++17 on Linux, featuring a custom binary message protocol, thread-safe producer-consumer queue, fixed worker pool, graceful shutdown, and automated unit testing with GoogleTest.

## Features

- TCP client-server communication using POSIX sockets
- Custom binary message protocol
- Message serialization and deserialization
- Reliable `sendAll()` and `recvAll()` socket operations
- Message type and payload-size validation
- Multithreaded client connection handling
- Thread-safe producer-consumer queue
- Fixed-size worker thread pool
- Mutex-protected message statistics
- Graceful server shutdown using `SIGINT`
- CMake build system
- GoogleTest unit tests

## Architecture

```text
                    TCP Server
                        |
                    accept()
                        |
              +---------+---------+
              |                   |
       Connection Thread   Connection Thread
              |                   |
         receiveMessage()   receiveMessage()
              |                   |
              +---------+---------+
                        |
                        v
                Thread-Safe Queue
                        |
             +----------+----------+
             |          |          |
             v          v          v
          Worker 1   Worker 2   Worker 3   Worker 4
             |          |          |          |
             +----------+----------+----------+
                        |
                        v
                   Send Response
```
