#include "server.h"
#include "protocol/client/message.h"
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

using namespace siktacka;

Server::Server(network::port_t port, const GameOptions &game_options)
        : Server::Server(port, GameOptions(game_options)) {}

Server::Server(network::port_t port, GameOptions &&game_options) {
    open_socket();
    bind_socket(port);

    try {
        poll = std::make_unique<network::Poll<1>>();
        poll->add_descriptor(sock, POLLIN | POLLOUT);
    } catch (const network::PollException &e) {
        throw ServerException(e.what());
    }

    connections = std::make_unique<network::Connections>(this);
    sender = std::make_unique<network::Sender>(sock);
    receiver = std::make_unique<network::Receiver>(sock);

    events = std::make_unique<Events>();
    game = std::make_unique<Game>(game_options, this);
}

void Server::bind_socket(network::port_t port) {
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);

    if (bind(sock, (sockaddr *) &address, (socklen_t) sizeof(address)) < 0) {
        throw ServerException("Error binding to socket");
    }
}

void Server::open_socket() {
    if ((sock = socket(AF_INET, SOCK_DGRAM | O_NONBLOCK, IPPROTO_UDP)) < 0) {
        throw ServerException("Error opening socket");
    }
}

Server::~Server() {
    if (close(sock) < 0) {
        std::cerr << "Error closing server socket" << std::endl;
    }
}

void Server::on_event(std::unique_ptr<Event> event) {
    network::Connections::connection_t event_time =
            std::chrono::system_clock::now();
    std::queue<sockaddr_in> clients = connections->get_connected_clients(
            event_time);
    std::unique_ptr<ServerMessage> message = std::make_unique<ServerMessage>(
            game->get_id());
    message->add_event(event.get());
    messages.push(std::make_pair(std::move(message), clients));
    (*poll)[sock].events = POLLIN | POLLOUT;

    if (event->get_event_type() == event_t::GAME_OVER) {
        events->clear();
    } else {
        events->add_event(std::move(event));
    }
}


void Server::on_disconnect(const std::string &name) {
    game->remove_player(name);
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

void Server::send_message() {
    while (messages.size() > 0 && messages.front().second.empty()) {
        messages.pop();
    }

    if (messages.size() == 0) {
        (*poll)[sock].events = POLLIN;
        return;
    }

    try {
        sender->send_message(messages.front().second.front(),
                             messages.front().first->to_bytes());
        messages.front().second.pop();
    } catch (const network::WouldBlockException &) {
        return;
    }
}

void Server::receive_message() {
    try {
        sockaddr_in client_address = sockaddr_in();
        ClientMessage message(receiver->receive_message(client_address));
        network::Connections::connection_t connection_time =
                std::chrono::high_resolution_clock::now();
        try {
            player_action(client_address, &message, connection_time);
        } catch (const std::out_of_range &) {
            player_connect(client_address, &message, connection_time);
        } catch (const std::invalid_argument &) {
            return;
        }

        make_message(message.get_next_event_no(), client_address);
    } catch (const std::runtime_error &e) {
        std::cerr << "Error while receiving message: " << e.what() << std::endl;
        return;
    } catch (const std::invalid_argument &e) {
        std::cerr << "Received invalid message: " << e.what() << std::endl;
        return;
    }
}

void Server::make_message(event_no_t next_event, sockaddr_in client_address) {
    std::queue<Event *> message_events = events->get_events(next_event);
    if (message_events.size() == 0) {
        return;
    }

    std::queue<sockaddr_in> client;
    client.push(client_address);

    std::unique_ptr<ServerMessage> message = std::make_unique<ServerMessage>(
            game->get_id());

    while (!message_events.empty()) {
        Event *event = message_events.front();
        try {
            message->add_event(event);
        } catch (const std::overflow_error &) {
            messages.push(std::make_pair(std::move(message), client));
            message = std::make_unique<ServerMessage>(game->get_id());
        }
        message_events.pop();
    }
    messages.push(std::make_pair(std::move(message), client));
    (*poll)[sock].events = POLLIN | POLLOUT;
}

void Server::player_action(sockaddr_in client_address, ClientMessage *message,
                           network::Connections::connection_t connection_time) {
    const std::string &name = connections->get_client(
            client_address, message->get_session(), connection_time);
    if (name == "" || name != message->get_player_name()) {
        return;
    }
    game->player_action(name, message->get_turn_direction());
}

void Server::player_connect(sockaddr_in client_address, ClientMessage *message,
                            network::Connections::connection_t connection_time) {
    try {
        game->add_player(message->get_player_name());
    } catch (const std::invalid_argument &e) {
        std::cerr << "Error adding player to game: " << e.what() << std::endl;
        return;
    } catch (const std::length_error &e) {
        std::cerr << "[This should never happen]: " << e.what() << std::endl;
        return;
    }
    connections->add_client(client_address, message->get_session(),
                            message->get_player_name(), connection_time);

}

