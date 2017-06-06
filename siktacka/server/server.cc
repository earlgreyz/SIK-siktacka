#include "server.h"
#include "../protocol/server/message.h"
#include <netdb.h>
#include <boost/lexical_cast.hpp>
#include <fcntl.h>

using namespace sikserver;

Server::Server(network::port_t port,
               const siktacka::GameOptions &game_options)
        : Server::Server(port, siktacka::GameOptions(game_options)) {}

Server::Server(network::port_t port, siktacka::GameOptions &&game_options) {
    bind_socket(port);

    try {
        poll = std::make_unique<network::Poll<1>>();
        poll->add_descriptor(sock, POLLIN | POLLOUT);
    } catch (const network::PollException &e) {
        throw std::runtime_error(e.what());
    }

    sender = std::make_unique<network::Sender>(sock);
    receiver = std::make_unique<network::Receiver>(sock);

    events = std::make_unique<siktacka::Events>();
    game = std::make_unique<siktacka::Game>(game_options, this);
    connections = std::make_unique<Connections>(this);
    messages = std::make_unique<Buffer>();
}

Server::~Server() {
    if (close(sock) < 0) {
        std::cerr << "Error closing server socket" << std::endl;
    }
}

void Server::bind_socket(network::port_t port) {
    addrinfo hints;
    addrinfo *result;
    addrinfo *rp;

    std::string port_buffer = boost::lexical_cast<std::string>(port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    if (getaddrinfo(nullptr, port_buffer.c_str(), &hints, &result) != 0) {
        throw std::invalid_argument("Error setting server_address");
    }

    if ((sock = socket(rp->ai_family, SOCK_DGRAM | O_NONBLOCK, IPPROTO_UDP))
                < 0) {
        throw std::runtime_error("Error creating socket");
    }

    for (rp = result; rp != nullptr; rp = rp->ai_next) {
        if (bind(sock, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }
    }

    if (rp == nullptr) {
        throw std::runtime_error("Could not bind to address");
    }

    freeaddrinfo(result);
}

void Server::run() {
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

        if ((*poll)[sock].revents & POLLIN) {
            receive_message();
        }

        if ((*poll)[sock].revents & POLLOUT) {
            send_message();
        }
    }
}

void Server::stop() noexcept {
    stopping = true;
}

void Server::on_event(std::shared_ptr<siktacka::Event> event) {
    connection_t now = std::chrono::high_resolution_clock::now();
    siktacka::ServerMessage message(game->get_id());
    message.add_event(event);
    std::queue<sockaddr_storage> clients = connections->get_connected_clients(
            now);

    std::cout << "Event " << event->get_event_type() << std::endl;
    std::lock_guard<std::mutex> guard(messages_mutex);
    messages->add(Buffer::Message(message.to_bytes(), std::move(clients)));
    (*poll)[sock].events = POLLIN | POLLOUT;
}

void Server::send_message() {
    {
        std::lock_guard<std::mutex> guard(messages_mutex);
        while (!messages->is_empty() && current_addresses.empty()) {
            Buffer::Message message = messages->pop();
            current_message = message.buffer;
            current_addresses = message.addresses;
        }

        if (current_addresses.size() == 0) {
            (*poll)[sock].events = POLLIN;
            return;
        }
    }

    try {
        sockaddr_storage address = current_addresses.front();
        sender->send_message(&address, current_message);
        current_addresses.pop();
    } catch (const network::WouldBlockException &) {
        return;
    }
}

void Server::receive_message() {
    connection_t now = std::chrono::high_resolution_clock::now();
    sockaddr_storage address = sockaddr_storage();
    network::buffer_t buffer = receiver->receive_message(&address);
    std::shared_ptr<siktacka::ClientMessage> message;

    try {
        message = std::make_shared<siktacka::ClientMessage>(buffer);
    } catch (const std::invalid_argument &) {
        std::cerr << "Invalid message received" << std::endl;
        return;
    }

    try {
        on_action(message, address, now);
    } catch (const std::invalid_argument &e) {
        std::cerr << e.what() << std::endl;
    } catch (const std::out_of_range &) {
        on_connect(message, address, now);
    }
}

void Server::on_disconnect(const std::string &name) {
    game->remove_player(name);
}

void Server::make_message(sockaddr_storage address,
                          siktacka::event_no_t event_no) {
    siktacka::ServerMessage message(game->get_id());
    std::queue<std::shared_ptr<siktacka::Event>> message_events =
            events->get_events(event_no);
    if (message_events.size() == 0) {
        return;
    }

    while (!message_events.empty()) {
        try {
            message.add_event(message_events.front());
            message_events.pop();
        } catch (const std::overflow_error &) {
            network::buffer_t buffer = message.to_bytes();
            add_message(Buffer::Message(buffer, address));
            message = siktacka::ServerMessage(game->get_id());
        }
    }

    network::buffer_t buffer = message.to_bytes();
    add_message(Buffer::Message(buffer, address));
}

void Server::add_message(Buffer::Message message) noexcept {
    std::lock_guard<std::mutex> guard(messages_mutex);
    messages->add(message);
    (*poll)[sock].events = POLLIN | POLLOUT;
}

void Server::on_action(std::shared_ptr<siktacka::ClientMessage> message,
                       sockaddr_storage address, connection_t now) {
    std::string player =
            connections->get_client(address, message->get_session(), now);

    if (player != message->get_player_name()) {
        throw std::invalid_argument("Spoofing detected for player " + player);
    }

    game->player_action(player, message->get_turn_direction());
    make_message(address, message->get_next_event_no());
}

void Server::on_connect(std::shared_ptr<siktacka::ClientMessage> message,
                        sockaddr_storage address, connection_t now) noexcept {
    try {
        game->add_player(message->get_player_name());
        std::cout << message->get_player_name() << " connected" << std::endl;

        siktacka::session_t session = message->get_session();
        std::string name = message->get_player_name();
        connections->add_client(address, session, name, now);

        make_message(address, message->get_next_event_no());
    } catch (const std::invalid_argument &) {
        // Player already with that name is already in game
        return;
    }
}
