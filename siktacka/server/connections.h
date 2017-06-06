#ifndef SIK_SIKTACKA_CONNECTIONS_H
#define SIK_SIKTACKA_CONNECTIONS_H


#include <chrono>
#include <netinet/in.h>
#include <vector>
#include <queue>
#include "i_connection_listener.h"
#include "../types.h"

namespace sikserver {
    using connection_t =
    std::chrono::time_point<std::chrono::high_resolution_clock>;

    class Connections {
    private:
        struct Client {
            sockaddr_storage address;
            siktacka::session_t session;
            std::string name;
            connection_t timestamp;

            Client(sockaddr_storage address, siktacka::session_t session,
                   std::string name, connection_t timestamp);

            bool is_active(connection_t time_point) const noexcept;
        };

        std::vector<Client> clients;

        IConnectionListener *listener;
    public:
        Connections(IConnectionListener *listener) noexcept;

        std::string
        get_client(sockaddr_storage address, siktacka::session_t session,
                   connection_t now);

        void add_client(sockaddr_storage address, siktacka::session_t session,
                        const std::string &name, connection_t now) noexcept;

        std::queue<sockaddr_storage>
        get_connected_clients(connection_t now) noexcept;

    private:
        void cleanup_inactive(connection_t now) noexcept;

    };
}


#endif
