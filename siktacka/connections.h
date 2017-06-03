#ifndef SIK_SIKTACKA_CONNECTIONS_H
#define SIK_SIKTACKA_CONNECTIONS_H


#include <netinet/in.h>
#include <ctime>
#include <algorithm>
#include <chrono>
#include <map>
#include <list>
#include "types.h"


namespace sik {
    class Connections {
    public:
        using connection_t = std::chrono::time_point<std::chrono::system_clock>;
    private:
        struct Client {
            /// Client socket address
            sockaddr_in address;
            siktacka::session_t session;
            connection_t timestamp;
            std::string name;

            Client(sockaddr_in address, siktacka::session_t session,
                   connection_t timestamp);

            bool is_active(connection_t time_point) const noexcept;
        };

        std::list<Client> clients;
    public:
        void get_client(sockaddr_in address, siktacka::session_t session,
                        const std::string &name, connection_t time_point);

        void add_client(sockaddr_in address, siktacka::session_t session,
                        connection_t time_point);
    };
}


#endif
