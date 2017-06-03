#include "receiver.h"
#include "../siktacka/protocol/server/constants.h"

using namespace sik;

Receiver::Receiver(int sock) noexcept : sock(sock) {}

buffer_t Receiver::receive_message(const sockaddr_in &address) {
    socklen_t address_len = sizeof(address);
    buffer_t buffer(siktacka::MAX_MESSAGE_LEN);

    ssize_t length;
    length = recvfrom(sock, buffer.data(), buffer.size(), 0,
                      (sockaddr *) &address, &address_len);
    if (length < 0) {
        throw std::runtime_error("Error receiving data");
    }

    buffer.resize(length);
    return buffer;
}

