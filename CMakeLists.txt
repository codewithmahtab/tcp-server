cmake_minimum_required(VERSION 3.16)

project(TCPNetworkProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(server
    src/server.cpp
    src/message.cpp
    src/socket_utils.cpp
)

add_executable(client
    src/client.cpp
    src/message.cpp
    src/socket_utils.cpp
)