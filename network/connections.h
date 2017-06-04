#ifndef SIK_SIKTACKA_CONNECTIONS_H
#define SIK_SIKTACKA_CONNECTIONS_H


#include <netinet/in.h>
#include <ctime>
#include <algorithm>
#include <chrono>
#include <map>
#include <list>
#include <queue>
#include "../siktacka/types.h"
#include "i_connection_listener.h"


namespace network {
    class Connections {
    public:
        using connection_t = std::chrono::time_point<std::chrono::system_clock>;
    private:
        struct Client {
            /// Client socket address
            sockaddr_in address;
            siktacka::session_t session;
            std::string name;
            connection_t timestamp;

            Client(sockaddr_in address, siktacka::session_t session,
                   std::string name, connection_t timestamp);

            bool is_active(connection_t time_point) const noexcept;
        };

        std::list<Client> clients;
        IConnectionListener *listener;
    public:
        Connections(IConnectionListener *listener) noexcept;

        void get_client(sockaddr_in address, siktacka::session_t session,
                        connection_t time_point);

        void add_client(sockaddr_in address, siktacka::session_t session,
                        const std::string &name,
                        connection_t time_point) noexcept;

        std::queue<sockaddr_in>
        get_connected_clients(connection_t connection_time) noexcept;
    };
}


#endif
