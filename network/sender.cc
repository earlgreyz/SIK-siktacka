#include <iostream>
#include "sender.h"
#include <arpa/inet.h>

using namespace network;


void Sender::send_message(const sockaddr_in &address,
                          const network::buffer_t &buffer) const {
    socklen_t address_len = sizeof(address);
    ssize_t length = sendto(sock, buffer.data(), buffer.size(), 0,
                            (sockaddr *) &address, address_len);

    if (length < 0 && errno == EWOULDBLOCK) {
        throw WouldBlockException();
    }

    if (length < 0) {
        std::cerr << "Error sending " << errno << std::endl;
    }
}
