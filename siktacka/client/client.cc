#include <netdb.h>
#include <cstring>
#include <zconf.h>
#include <iostream>
#include <fcntl.h>
#include <chrono>
#include "client.h"

using namespace sikclient;

Client::Client(const std::string &name, const std::string &host,
               network::port_t port, const std::string &gui_host,
               network::port_t gui_port) : name(name) {
    make_session_id();

    open_sockets();
    setup_address(host, port);

    gui_client = std::make_unique<GuiClient>(gui_sock, gui_host, gui_port);
    sender = std::make_unique<network::Sender>(server_sock);
    receiver = std::make_unique<network::Receiver>(server_sock);
    poll = std::make_unique<network::Poll<2>>();

    poll->add_descriptor(server_sock, POLLIN);
    poll->add_descriptor(gui_sock, POLLIN);

    event_no = 0u;
}

Client::~Client() {
    if (close(server_sock) != 0) {
        std::cerr << "Error closing socket" << std::endl;
    }

    if (close(gui_sock) != 0) {
        std::cerr << "Error closing socket" << std::endl;
    }
}


void Client::open_sockets() {
    if ((server_sock = socket(AF_INET, SOCK_DGRAM | O_NONBLOCK, IPPROTO_UDP)) <
        0) {
        throw std::runtime_error("Error opening server socket");
    }

    if ((gui_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        throw std::runtime_error("Error opening gui socket");
    }
}

void Client::setup_address(const std::string &host, uint16_t port) {
    addrinfo addr_hints;
    addrinfo *addr_result;
    memset(&addr_hints, 0, sizeof(addr_hints));
    addr_hints.ai_family = AF_INET;
    addr_hints.ai_socktype = SOCK_DGRAM;
    addr_hints.ai_protocol = IPPROTO_UDP;

    if (getaddrinfo(host.c_str(), nullptr, &addr_hints, &addr_result)
        != 0) {
        throw std::invalid_argument("Error getting server_address");
    }

    server_address.sin_family = AF_INET;
    sockaddr_in *sockaddr_result = (sockaddr_in *) addr_result->ai_addr;
    server_address.sin_addr.s_addr = sockaddr_result->sin_addr.s_addr;
    server_address.sin_port = htons(port);

    freeaddrinfo(addr_result);
}

void Client::run() {
    add_server_message();
    while (!stopping) {
        while (!stopping) {
            try {
                poll->wait(-1);
            } catch (const std::runtime_error &) {
                continue;
            }

            if (stopping) {
                return;
            }

            if ((*poll)[server_sock].revents & POLLIN) {
                receive_message();
            }

            if ((*poll)[server_sock].revents & POLLOUT) {
                send_message();
            }

            if ((*poll)[gui_sock].revents & POLLIN) {
                gui_client->receive_event();
            }

            if ((*poll)[gui_sock].revents & POLLOUT) {
                //gui_client->send_event();
            }
        }
    }
}

void Client::stop() noexcept {
    stopping = true;
}

void Client::make_session_id() noexcept {
    using std::chrono::microseconds;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    microseconds now = duration_cast<microseconds>(
            high_resolution_clock::now().time_since_epoch());
    session = static_cast<siktacka::session_t>(now.count());
}

void Client::add_server_message() noexcept {
    std::lock_guard<std::mutex> lock(messages_mutex);
    messages.push(std::make_unique<siktacka::ClientMessage>(
            session, gui_client->get_direction(), event_no, name));
    (*poll)[server_sock].events = POLLIN | POLLOUT;
}

void Client::receive_message() {

}

void Client::send_message() {
    {
        std::lock_guard<std::mutex> lock(messages_mutex);
        if (messages.size() == 0) {
            (*poll)[server_sock].events = POLLIN;
            return;
        }
    }
    try {
        sender->send_message(server_address, messages.front()->to_bytes());
        messages.pop();
    } catch (const network::WouldBlockException &) {
        return;
    }
}
