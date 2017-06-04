#include <stdexcept>
#include "receiver.h"
#include "constants.h"

using namespace network;

Receiver::Receiver(int sock) noexcept : sock(sock) {}

buffer_t Receiver::receive_message(const sockaddr_in &address) {
    socklen_t address_len = sizeof(address);
    buffer_t buffer(MAX_MESSAGE_LEN);

    ssize_t length;
    length = recvfrom(sock, buffer.data(), buffer.size(), 0,
                      (sockaddr *) &address, &address_len);
    if (length < 0) {
        throw std::runtime_error("Error receiving data");
    }

    buffer.resize(static_cast<std::size_t>(length));
    return buffer;
}

