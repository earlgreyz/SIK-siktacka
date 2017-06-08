#include <netdb.h>
#include <cstring>
#include <zconf.h>
#include <iostream>
#include <fcntl.h>
#include <chrono>
#include <arpa/inet.h>
#include <boost/lexical_cast.hpp>
#include "client.h"
#include "../siktacka/protocol/server/message.h"
#include "../siktacka/events/event_new_game.h"

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
    if ((gui_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        throw std::runtime_error("Error opening gui socket");
    }
}

void Client::setup_address(const std::string &host, network::port_t port) {
    addrinfo hints;
    addrinfo *result;
    std::string port_buffer = boost::lexical_cast<std::string>(port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    if (getaddrinfo(host.c_str(), port_buffer.c_str(), &hints, &result) != 0) {
        throw std::invalid_argument("Error getting server_address");
    }

    if ((server_sock = socket(result->ai_family, SOCK_DGRAM | O_NONBLOCK,
                              IPPROTO_UDP)) < 0) {
        throw std::runtime_error("Error opening server socket");
    }

    memcpy(&server_address, result->ai_addr, result->ai_addrlen);
    freeaddrinfo(result);
}

void Client::run() {
    add_server_message();
    client_thread = std::thread([&]() {
        using std::chrono::milliseconds;
        using std::chrono::duration_cast;
        using std::chrono::high_resolution_clock;
        milliseconds round_time(20);
        milliseconds sleep_time(0);
        while (!stopping) {
            auto start = high_resolution_clock::now();
            add_server_message();
            auto end = high_resolution_clock::now();
            sleep_time += round_time;
            sleep_time -= duration_cast<milliseconds>(end - start);
            if (sleep_time > milliseconds(0)) {
                std::this_thread::sleep_for(sleep_time);
                sleep_time = milliseconds(0);
            }
        }
    });
    client_thread.detach();

    while (!stopping) {
        try {
            poll->wait(40);
        } catch (const network::poll_timeout &) {
            continue;
        } catch (const network::poll_error &) {
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
            send_event();
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
    network::buffer_t buffer = receiver->receive_message(&server_address);
    siktacka::ServerMessage message(buffer);
    if (game_id != message.get_game_id()) {
        game_id = message.get_game_id();
        event_no = 0;
    }

    for (const auto &event: message) {
        if (event->get_event_no() < event_no) {
            continue;
        } else if (event->get_event_no() == event_no) {
            if (event->get_event_type() == siktacka::event_t::GAME_OVER) {
                continue;
            } else if (event->get_event_type() == siktacka::event_t::NEW_GAME) {
                initialize_players(
                        reinterpret_cast<siktacka::EventNewGame *>(event.get()));
            }

            event_no++;
            events.push(event->to_string(players));
            (*poll)[gui_sock].events = POLLIN | POLLOUT;
        } else {
            break;
        }
    }
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
        sender->send_message(&server_address, messages.front()->to_bytes());
        messages.pop();
    } catch (const network::would_block_error &) {
        return;
    }
}

void Client::send_event() {
    if (events.size() == 0) {
        (*poll)[gui_sock].events = POLLIN;
    } else {
        try {
            gui_client->send_event(events.front());
            events.pop();
        } catch (network::would_block_error &) {
            return;
        }
    }
}

void Client::initialize_players(siktacka::EventNewGame *event) {
    if (event->get_event_type() != siktacka::event_t::NEW_GAME) {
        throw std::invalid_argument("Players can be initialized only from"
                                            "NEW_GAME event");
    }

    players.clear();
    for (const std::string &player: *event) {
        players.push_back(player);
    }
}
