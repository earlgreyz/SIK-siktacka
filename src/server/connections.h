#ifndef SIK_SIKTACKA_CONNECTIONS_H
#define SIK_SIKTACKA_CONNECTIONS_H


#include <chrono>
#include <netinet/in.h>
#include <vector>
#include <queue>
#include "i_connection_listener.h"
#include "../siktacka/types.h"

namespace sikserver {
    using connection_t =
    std::chrono::time_point<std::chrono::high_resolution_clock>;

    /// Timeout interval
    constexpr std::chrono::seconds TIMEOUT = std::chrono::seconds(2);

    /**
     * Manages active connections.
     */
    class Connections {
    private:
        /**
         * Single client connection.
         */
        struct Client {
            /// Client address
            sockaddr_storage address;
            /// Connection session id
            siktacka::session_t session;
            /// Client name
            std::string name;
            /// Client connection timestamp
            connection_t timestamp;

            /**
             * Constructs new client.
             * @param address client address.
             * @param session client connection session.
             * @param name client name.
             * @param timestamp connection timestamp.
             */
            Client(sockaddr_storage address, siktacka::session_t session,
                   std::string name, connection_t timestamp);

            /*
             * Checks if client is active in given time_point.
             */
            bool is_active(connection_t time_point) const noexcept;
        };

        /// Connected clients
        std::vector<Client> clients;

        /// Listener of on_disconnect events.
        IConnectionListener *listener;
    public:
        /**
         * Constructs new connections.
         * @param listener `on_disconnect` event listener.
         */
        Connections(IConnectionListener *listener) noexcept;

        /**
         * Gets client name with given address and session.
         * Cleans up inactive connections.
         * @param address client address.
         * @param session client session.
         * @param now current time_point.
         * @return client name.
         * @throws std::out_of_range if client does not exist or requested
         * newer session than the existing one.
         */
        std::string
        get_client(sockaddr_storage address, siktacka::session_t session,
                   connection_t now);

        /**
         * Adds new client to the connections list.
         * Cleans up inactive connections.
         * @param address client address.
         * @param session client session.
         * @param name client name.
         * @param now current time_point.
         */
        void add_client(sockaddr_storage address, siktacka::session_t session,
                        const std::string &name, connection_t now) noexcept;

        /**
         * Gets all clients connected at given time_point.
         * @param now current time_point.
         * @return list of connected clients.
         */
        std::queue<sockaddr_storage>
        get_connected_clients(connection_t now) noexcept;

    private:
        /**
         * Removes inactive clients from clients list.
         * @param now current time_point.
         */
        void cleanup_inactive(connection_t now) noexcept;
    };
}


#endif
