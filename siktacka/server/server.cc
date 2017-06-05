#include "server.h"
#include "../protocol/server/message.h"
#include <fcntl.h>
#include <zconf.h>
#include <arpa/inet.h>

using namespace sikserver;

Server::Server(network::port_t port,
               const siktacka::GameOptions &game_options)
        : Server::Server(port, siktacka::GameOptions(game_options)) {}

Server::Server(network::port_t port, siktacka::GameOptions &&game_options) {
    open_socket();
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

void Server::open_socket() {
    if ((sock = socket(AF_INET, SOCK_DGRAM | O_NONBLOCK, IPPROTO_UDP)) < 0) {
        throw std::runtime_error("Error opening socket");
    }
}

void Server::bind_socket(network::port_t port) {
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    if (bind(sock, (sockaddr *) &address, (socklen_t) sizeof(address)) < 0) {
        throw std::runtime_error("Error binding to socket");
    }
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
    std::queue<sockaddr_in> clients = connections->get_connected_clients(now);

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
        sockaddr_in address = current_addresses.front();
        sender->send_message(address, current_message);
        current_addresses.pop();
    } catch (const network::WouldBlockException &) {
        return;
    }
}

void Server::receive_message() {
    connection_t now = std::chrono::high_resolution_clock::now();
    sockaddr_in address;
    network::buffer_t buffer = receiver->receive_message(address);
    std::shared_ptr<siktacka::ClientMessage> message;

    try {
        message = std::make_shared<siktacka::ClientMessage>(buffer);
    } catch (const std::invalid_argument &) {
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

void Server::make_message(sockaddr_in address,
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
                       sockaddr_in address, connection_t now) {
    const std::string &player =
            connections->get_client(address, message->get_session(), now);

    if (player != message->get_player_name()) {
        throw std::invalid_argument("Spoofing detected for player " + player);
    }

    game->player_action(player, message->get_turn_direction());
    make_message(address, message->get_next_event_no());
}

void Server::on_connect(std::shared_ptr<siktacka::ClientMessage> message,
                        sockaddr_in address, connection_t now) noexcept {
    try {
        game->add_player(message->get_player_name());

        siktacka::session_t session = message->get_session();
        std::string name = message->get_player_name();
        connections->add_client(address, session, name, now);

        make_message(address, message->get_next_event_no());
    } catch (const std::invalid_argument &) {
        std::cerr << "Player " << message->get_player_name()
                  << "already in game" << std::endl;
        return;
    }
}
