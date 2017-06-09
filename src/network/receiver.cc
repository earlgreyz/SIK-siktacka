#include <stdexcept>
#include <iostream>
#include <cstring>
#include "receiver.h"
#include "constants.h"

using namespace network;

Receiver::Receiver(int sock) noexcept : sock(sock) {}

buffer_t Receiver::receive_message(sockaddr_storage *address) {
    memset(address, 0, sizeof(sockaddr_storage));
    sockaddr *sender = reinterpret_cast<sockaddr *>(address);
    socklen_t socklen = sizeof(sockaddr_storage);
    buffer_t buffer(MAX_MESSAGE_LEN, '\0');
    char * data = const_cast<char *>(buffer.data());

    ssize_t length;
    length = recvfrom(sock, data, buffer.size(), 0, sender, &socklen);
    if (length < 0) {
        throw std::runtime_error("Error receiving data");
    }

    buffer.resize(static_cast<std::size_t>(length));
    return buffer;
}

