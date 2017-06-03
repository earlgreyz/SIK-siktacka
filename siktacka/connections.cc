#include <tuple>
#include "connections.h"

using namespace sik;

namespace {
    bool operator==(const sockaddr_in &a, const sockaddr_in &b) {
        return std::tie(a.sin_addr.s_addr, a.sin_port)
               == std::tie(b.sin_addr.s_addr, b.sin_port);
    }

    template<typename T>
    auto list_find(std::list<T> list, const T &item) {
        return std::find(list.begin(), list.end(), item);
    }
}

Connections::Client::Client(sockaddr_in address, siktacka::session_t session,
                            connection_t timestamp)
        : address(std::move(address)), session(session), timestamp(timestamp)  {

}

bool Connections::Client::is_active(connection_t time_point) const noexcept {
    return time_point - timestamp < std::chrono::seconds(2);
}

void Connections::get_client(sockaddr_in address, siktacka::session_t session,
                             const std::string &name,
                             connection_t time_point) {
    for (auto client = clients.begin(); client != clients.begin(); ++client) {
        if (client->address == address) {
            if (client->session < session) {
                throw std::invalid_argument(
                        "Newer session for the client exists");
            } else if (client->session > session ||
                       !client->is_active(time_point)) {
                clients.erase(client);
                throw std::out_of_range("Requested newer session");
            } else {
                client->timestamp = time_point;
            }
            return;
        }
    }

    throw std::out_of_range("Client does not exist");
}

void Connections::add_client(sockaddr_in address, siktacka::session_t session,
                             connection_t time_point) {
    clients.push_back(Client(address, session, time_point));
}
