#include "server.h"

#include <iostream>
#include <unistd.h>
#include <fcntl.h>

using namespace siktacka;

Server::Server(sik::port_t port, const GameOptions &game_options)
        : Server::Server(port, std::move(GameOptions(game_options))) {

}

Server::Server(sik::port_t port, GameOptions &&game_options) {
    open_socket();
    bind_socket(port);

    try {
        poll = std::make_unique<sik::Poll<1>>();
        poll->add_descriptor(sock, POLLIN | POLLOUT);
    } catch (const sik::PollException &e) {
        throw ServerException(e.what());
    }

    game = std::make_unique<Game>(std::move(game_options));
}

void Server::bind_socket(sik::port_t port) {
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
    int binding_result = bind(sock, (sockaddr *) &address,
    (socklen_t) sizeof(address));
    if (binding_result < 0) {
        throw ServerException("Error binding to socket");
    }
}

void Server::open_socket() {
    sock = socket(AF_INET, SOCK_DGRAM | O_NONBLOCK, IPPROTO_UDP);
    if (sock < 0) {
        throw ServerException("Error opening socket");
    }
}

Server::~Server() {
    if (close(sock) < 0) {
        std::cerr << "Error closing server socket" << std::endl;
    }
}

void Server::run() noexcept {
    stopping = false;
    while (!stopping) {
        try {
            poll->wait(-1);
        } catch (const std::runtime_error &) {
            continue;
        }

        if (stopping) {
            return;
        }
    }
}

void Server::stop() noexcept {
    stopping = true;
}