#include <iostream>
#include "sender.h"

using namespace network;


void Sender::send_message(sockaddr_storage *address,
                          const network::buffer_t &buffer) const {
    sockaddr *receiver = reinterpret_cast<sockaddr *>(address);
    socklen_t socklen;

    if (address->ss_family == AF_INET6) {
        socklen = sizeof(sockaddr_in6);
    } else if (address->ss_family == AF_INET) {
        socklen = sizeof(sockaddr_in);
    } else {
        throw std::invalid_argument("Family not supported");
    }

    ssize_t length;
    length = sendto(sock, buffer.data(), buffer.size(), 0, receiver, socklen);

    if (length < 0 && errno == EWOULDBLOCK) {
        throw would_block_error();
    } else if (length < 0) {
        std::cerr << "Error sending " << errno << std::endl;
    }
}
