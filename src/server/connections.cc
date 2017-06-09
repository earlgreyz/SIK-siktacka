#include <tuple>
#include <cstring>
#include <iostream>
#include <algorithm>
#include "connections.h"

using namespace sikserver;

namespace {
    bool operator==(const sockaddr_storage &a, const sockaddr_storage &b) {
        return memcmp(&a, &b, sizeof(sockaddr_storage)) == 0;
    }
}

Connections::Client::Client(sockaddr_storage address,
                            siktacka::session_t session, std::string name,
                            sikserver::connection_t timestamp) : address(
        address), session(session), name(name), timestamp(timestamp) {

}

bool sikserver::Connections::Client::is_active(
        sikserver::connection_t time_point) const noexcept {
    return time_point - timestamp < std::chrono::seconds(2);
}

Connections::Connections(IConnectionListener *listener) noexcept: listener(
        listener) {

}

std::string
Connections::get_client(sockaddr_storage address, siktacka::session_t session,
                        connection_t now) {
    cleanup_inactive(now);

    for (auto client = clients.begin(); client != clients.end(); client++) {
        if (client->address == address) {
            if (client->session > session) {
                throw std::invalid_argument("Newer session exists");
            } else if (client->session < session) {
                listener->on_disconnect(client->name);
                clients.erase(client);
                throw std::out_of_range("Requested newer session");
            } else {
                client->timestamp = now;
                return client->name;
            }
        }
    }

    throw std::out_of_range("Client does not exist");
}

void
Connections::add_client(sockaddr_storage address, siktacka::session_t session,
                        const std::string &name, connection_t now) noexcept {
    cleanup_inactive(now);
    clients.push_back(Client(address, session, name, now));
}

std::queue<sockaddr_storage>
Connections::get_connected_clients(connection_t now) noexcept {
    cleanup_inactive(now);
    std::queue<sockaddr_storage> connected_clients;
    for (auto &client: clients) {
        connected_clients.push(client.address);
    }
    return connected_clients;
}

void Connections::cleanup_inactive(connection_t now) noexcept {
    for (const auto &client: clients) {
        if (!client.is_active(now)) {
            listener->on_disconnect(client.name);
        }
    }

    clients.erase(std::remove_if(clients.begin(), clients.end(),
                                 [&now](const auto &client) {
                                     return !client.is_active(now);
                                 }), clients.end());
}
