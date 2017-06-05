#include <tuple>
#include "connections.h"

using namespace sikserver;

namespace {
    bool operator==(const sockaddr_in &a, const sockaddr_in &b) {
        return std::tie(a.sin_addr.s_addr, a.sin_port)
               == std::tie(b.sin_addr.s_addr, b.sin_port);
    }
}

Connections::Client::Client(sockaddr_in address, siktacka::session_t session,
                            std::string name, sikserver::connection_t timestamp)
        : address(address), session(session), name(name), timestamp(timestamp) {

}

bool sikserver::Connections::Client::is_active(
        sikserver::connection_t time_point) const noexcept {
    return time_point - timestamp < std::chrono::seconds(2);
}

Connections::Connections(IConnectionListener *listener) noexcept
        : listener(listener) {

}

const std::string &
Connections::get_client(sockaddr_in address, siktacka::session_t session,
                        connection_t time_point) {
    for (auto client = clients.begin(); client != clients.end(); ++client) {
        if (client->address == address) {
            if (client->session < session) {
                throw std::invalid_argument(
                        "Newer session for the client exists");
            } else if (client->session > session ||
                       !client->is_active(time_point)) {
                listener->on_disconnect(client->name);
                clients.erase(client);
                throw std::out_of_range("Requested newer session");
            } else {
                client->timestamp = time_point;
                return client->name;
            }
        }
    }

    throw std::out_of_range("Client does not exist");
}

void Connections::add_client(sockaddr_in address, siktacka::session_t session,
                             const std::string &name,
                             connection_t time_point) noexcept {
    clients.push_back(Client(address, session, name, time_point));
}

std::queue<sockaddr_in>
Connections::get_connected_clients(connection_t connection_time) noexcept {
    std::queue<sockaddr_in> connected_clients;

    auto client = clients.begin();
    while (client != clients.end()) {
        if (client->is_active(connection_time)) {
            sockaddr_in address = client->address;
            connected_clients.push(address);
            client++;
        } else {
            auto inactive_client = client;
            listener->on_disconnect(client->name);
            client++;
            clients.erase(inactive_client);
        }
    }

    return connected_clients;
}
