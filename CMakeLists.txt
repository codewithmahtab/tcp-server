cmake_minimum_required(VERSION 3.16)

project(TCPNetworkProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# ----------------------------------------
# Common source files
# ----------------------------------------

set(COMMON_SOURCES
    src/message.cpp
    src/socket_utils.cpp
)


# ----------------------------------------
# Server
# ----------------------------------------

add_executable(server
    src/server.cpp
    ${COMMON_SOURCES}
)

target_include_directories(server
    PRIVATE
    include
)

# client

add_executable(client
    src/client.cpp
    ${COMMON_SOURCES}
)

target_include_directories(client
    PRIVATE
    include
)


# ----------------------------------------
# Testing
# ----------------------------------------

enable_testing()

find_package(GTest REQUIRED)


add_executable(message_test
    tests/message_test.cpp
    ${COMMON_SOURCES}
)


target_include_directories(message_test
    PRIVATE
    include
)


target_link_libraries(message_test
    GTest::gtest_main
)


include(GoogleTest)

gtest_discover_tests(message_test)