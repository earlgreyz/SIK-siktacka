#include "server.h"
#include "../siktacka/protocol/server/message.h"
#include <netdb.h>
#include <boost/lexical_cast.hpp>
#include <fcntl.h>

using namespace sikserver;

Server::Server(network::port_t port,
               const siktacka::GameOptions &game_options)
        : Server::Server(port, siktacka::GameOptions(game_options)) {}

Server::Server(network::port_t port, siktacka::GameOptions &&game_options) {
    socket = std::make_unique<network::Socket>
            (AF_INET6, SOCK_DGRAM | O_NONBLOCK, IPPROTO_UDP);
    socket->bind_address(port);

    poll = std::make_unique<network::Poll<1>>();
    poll->add_descriptor(socket->get_descriptor(), POLLIN | POLLOUT);

    sender = std::make_unique<network::Sender>(socket->get_descriptor());
    receiver = std::make_unique<network::Receiver>(socket->get_descriptor());

    events = std::make_unique<Events>();
    game = std::make_unique<siktacka::Game>(game_options, this);
    connections = std::make_unique<Connections>(this);
    messages = std::make_unique<Buffer>();
}

void Server::run() {
    stopping = false;
    while (!stopping) {
        try {
            poll->wait(-1);
        } catch (const network::poll_error &) {
            continue;
        }
        if (stopping) {
            return;
        }

        if ((*poll)[socket->get_descriptor()].revents & POLLIN) {
            receive_message();
        }
        if ((*poll)[socket->get_descriptor()].revents & POLLOUT) {
            send_message();
        }
    }
}

void Server::stop() noexcept {
    stopping = true;
}

void Server::on_event(std::shared_ptr<siktacka::Event> event) {
    if (event->get_event_type() == siktacka::event_t::GAME_OVER) {
        events->clear();
    } else {
        events->add_event(event);
    }

    connection_t now = std::chrono::high_resolution_clock::now();
    std::queue<sockaddr_storage> clients;
    siktacka::ServerMessage message(game->get_id());

    message.add_event(event);
    {
        std::lock_guard<std::mutex> guard(connections_mutex);
        clients = connections->get_connected_clients(now);
    }

    add_message(Buffer::Message(message.to_bytes(), std::move(clients)));
}

void Server::on_disconnect(const std::string &name) {
    game->remove_player(name);
}

bool Server::can_send_message() noexcept {
    std::lock_guard<std::mutex> guard(messages_mutex);

    while (!messages->is_empty() && current_addresses.empty()) {
        Buffer::Message message = messages->pop();
        current_message = message.buffer;
        current_addresses = message.addresses;
    }

    if (current_addresses.size() == 0) {
        (*poll)[socket->get_descriptor()].events = POLLIN;
        return false;
    }
    return true;
}

void Server::send_message() {
    if (!can_send_message()) {
        return;
    }

    try {
        sockaddr_storage address = current_addresses.front();
        sender->send_message(&address, current_message);
        current_addresses.pop();
    } catch (const network::would_block_error &) {
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
    } catch (const std::invalid_argument &e) {
        std::cerr << "Invalid message received: " << e.what() << std::endl;
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
            add_message(Buffer::Message(message.to_bytes(), address));
            message = siktacka::ServerMessage(game->get_id());
        }
    }

    add_message(Buffer::Message(message.to_bytes(), address));
}

void Server::add_message(Buffer::Message message) noexcept {
    std::lock_guard<std::mutex> guard(messages_mutex);
    messages->add(message);
    (*poll)[socket->get_descriptor()].events = POLLIN | POLLOUT;
}

void Server::on_action(std::shared_ptr<siktacka::ClientMessage> message,
                       sockaddr_storage address, connection_t now) {
    std::string player;

    {
        std::lock_guard<std::mutex> guard(connections_mutex);
        player = connections->get_client(address, message->get_session(), now);
    }

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

        {
            std::lock_guard<std::mutex> guard(connections_mutex);
            connections->add_client(address, session, name, now);
        }

        make_message(address, message->get_next_event_no());
    } catch (const std::invalid_argument &) {
        // Player already with that name is already in game
        return;
    }
}