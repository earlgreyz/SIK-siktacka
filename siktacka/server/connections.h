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
            sockaddr_in address;
            siktacka::session_t session;
            std::string name;
            connection_t timestamp;

            Client(sockaddr_in address, siktacka::session_t session,
                   std::string name, connection_t timestamp);

            bool is_active(connection_t time_point) const noexcept;
        };

        std::vector<Client> clients;

        IConnectionListener *listener;
    public:
        Connections(IConnectionListener *listener) noexcept;

        const std::string &
        get_client(sockaddr_in address, siktacka::session_t session,
                   connection_t time_point);

        void add_client(sockaddr_in address, siktacka::session_t session,
                        const std::string &name,
                        connection_t time_point) noexcept;

        std::queue<sockaddr_in>
        get_connected_clients(connection_t connection_time) noexcept;

    };
}


#endif
