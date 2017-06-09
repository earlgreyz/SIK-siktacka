#include <sys/socket.h>
#include <zconf.h>
#include <iostream>
#include <netinet/in.h>
#include <cstring>
#include "socket.h"

using namespace network;

Socket::Socket(int domain, int type, int protocol) {
    if ((socket_descriptor = socket(domain, type, protocol)) < 0) {
        throw std::runtime_error("Error creating socket");
    }
}

Socket::~Socket() {
    if (close(socket_descriptor) < 0) {
        std::cerr << "Error closing socket " << socket_descriptor << std::endl;
    }
}

int Socket::get_descriptor() const noexcept {
    return socket_descriptor;
}

void Socket::bind_address(sockaddr *address, socklen_t address_len) const {
    if (bind(socket_descriptor, address, address_len) < 0) {
        throw std::runtime_error("Error binding to socket");
    }
}

void Socket::bind_address(port_t port) const {
    sockaddr_in6 address;

    memset(&address, 0, sizeof(address));
    address.sin6_family = AF_INET6;
    address.sin6_port = htons(port);

    bind_address(reinterpret_cast<sockaddr *>(&address), sizeof(address));
}