#include <cstring>
#include <netinet/in.h>
#include <netdb.h>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include "gui_client.h"

using namespace sikclient;


GuiClient::GuiClient(int sock, const std::string &host, network::port_t port)
        : sock(sock) {
    connect_to_gui(host, port);
    direction = siktacka::direction_t::NONE;
}

void GuiClient::connect_to_gui(const std::string &host, network::port_t port) {
    addrinfo addr_hints;
    addrinfo *addr_result;
    memset(&addr_hints, 0, sizeof(struct addrinfo));
    addr_hints.ai_flags = 0;
    addr_hints.ai_family = AF_INET;
    addr_hints.ai_socktype = SOCK_STREAM;
    addr_hints.ai_protocol = IPPROTO_TCP;
    std::string port_str = boost::lexical_cast<std::string>(port);

    if (getaddrinfo(host.c_str(), port_str.c_str(), &addr_hints, &addr_result)
        != 0) {
        throw std::invalid_argument("Error getting gui address");
    }

    if (connect(sock, addr_result->ai_addr, addr_result->ai_addrlen) != 0) {
        throw std::runtime_error("Error connecting to gui");
    }
    freeaddrinfo(addr_result);
}

void GuiClient::send_event(const std::string &event) {
    std::cout << event;
    if (write(sock, event.data(), event.size()) != 0) {
        throw std::runtime_error("GUI Disconnected");
    }
}

void GuiClient::receive_event() {
    network::buffer_t buffer(16u, 0);
    char * data = const_cast<char *>(buffer.data());
    if (read(sock, data, buffer.size() - 1) <= 0) {
        throw std::runtime_error("GUI Disconnected");
    }

    std::string action = buffer.data();

    if (action == "LEFT_KEY_DOWN\n") {
        direction = siktacka::direction_t::LEFT;
    } else if (action == "LEFT_KEY_UP\n") {
        deselect(siktacka::direction_t::LEFT);
    } else if (action == "RIGHT_KEY_DOWN\n") {
        direction = siktacka::direction_t::RIGHT;
    } else if (action == "RIGHT_KEY_UP\n") {
        deselect(siktacka::direction_t::RIGHT);
    }
}

siktacka::direction_t GuiClient::get_direction() const noexcept {
    return direction;
}

void GuiClient::deselect(siktacka::direction_t deselect) noexcept {
    if (direction == deselect) {
        direction = siktacka::direction_t::NONE;
    }
}

