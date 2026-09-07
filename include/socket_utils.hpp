#pragma once

#include <cstddef>

bool sendAll(int socket, const char* data, std::size_t length);

bool recvAll(int socket, char* data, std::size_t length);