#include <cstring>
#include <netinet/in.h>
#include <netdb.h>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include "gui_client.h"

using namespace sikclient;


GuiClient::GuiClient(int sock, const std::string &host, network::port_t port)
        : sock(sock), direction(siktacka::direction_t::NONE) {
    connect_to_gui(host, port);
    buffer = std::string(64u, 0);
    offset = 0u;
}

void GuiClient::connect_to_gui(const std::string &host, network::port_t port) {
    addrinfo hints, *result;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = 0;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    std::string port_str = boost::lexical_cast<std::string>(port);

    if (getaddrinfo(host.c_str(), port_str.c_str(), &hints, &result) != 0) {
        throw std::invalid_argument("Error getting GUI address");
    }

    if (connect(sock, result->ai_addr, result->ai_addrlen) != 0) {
        throw std::runtime_error("GUI Connection error");
    }
    freeaddrinfo(result);
}

void GuiClient::send_event(const std::string &event) {
    if (send(sock, event.data(), event.length(), 0) < 0) {
        throw std::runtime_error("GUI Connection error");
    }
}

void GuiClient::receive_event() {
    char *data = const_cast<char *>(buffer.data());
    ssize_t len;
    if ((len = read(sock, data + offset, buffer.size() - offset - 1)) <= 0) {
        throw std::runtime_error("GUI Connection error");
    }
    offset += len;
    std::string action("");

    for (std::size_t i = 0; i < offset; i++) {
        if (buffer[i] == '\n') {
            action = buffer.substr(0, i);
            buffer = buffer.substr(i + 1);
            buffer.resize(64u, 0);
            offset -= i + 1;
            break;
        }
    }

    if (action == "") {
        return;
    }

    if (action == "LEFT_KEY_DOWN") {
        direction = siktacka::direction_t::LEFT;
    } else if (action == "LEFT_KEY_UP") {
        deselect(siktacka::direction_t::LEFT);
    } else if (action == "RIGHT_KEY_DOWN") {
        direction = siktacka::direction_t::RIGHT;
    } else if (action == "RIGHT_KEY_UP") {
        deselect(siktacka::direction_t::RIGHT);
    }
}

siktacka::direction_t GuiClient::get_direction() const noexcept {
    return direction;
}

void GuiClient::deselect(siktacka::direction_t deselected) noexcept {
    if (direction == deselected) {
        direction = siktacka::direction_t::NONE;
    }
}

