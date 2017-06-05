#ifndef SIK_SIKTACKA_SERVER_H
#define SIK_SIKTACKA_SERVER_H

#include <string>
#include <memory>
#include <netinet/in.h>
#include <queue>
#include <mutex>

#include "types.h"
#include "game/game.h"
#include "game/events.h"
#include "game/i_event_listener.h"
#include "../error.h"
#include "../network/types.h"
#include "../network/poll.h"
#include "../network/sender.h"
#include "../network/receiver.h"
#include "../network/connections.h"
#include "protocol/server/message.h"
#include "../network/i_connection_listener.h"
#include "protocol/client/message.h"


namespace siktacka {
    const network::port_t SERVER_DEFAULT_PORT = 12345u;

    /**
     * Exception thrown when server error occurs.
     */
    class ServerException : public Exception {
    public:
        explicit ServerException(const std::string &message) : Exception(
                message) {}

        explicit ServerException(std::string &&message) : Exception(
                std::move(message)) {}
    };

    /**
     * Game server.
     */
    class Server : public IEventListener, public IConnectionListener {
        using MessageInstance =
        std::pair<ServerMessage, std::queue<sockaddr_in>>;
    private:
        /// Server UDP socket
        int sock;
        /// Server address bound to the socket.
        sockaddr_in address;
        /// Indicates whether server should stop main loop
        bool stopping;
        /// Poll for async IO operations.
        std::unique_ptr<network::Poll<1>> poll;

        /// Current game
        std::unique_ptr<Events> events;
        std::unique_ptr<Game> game;
        std::queue<MessageInstance> messages;
        std::mutex messages_mutex;

        // Server helper structures
        std::unique_ptr<network::Connections> connections;
        std::unique_ptr<network::Sender> sender;
        std::unique_ptr<network::Receiver> receiver;
    public:
        /**
         * Constructs new Server instance.
         * @param port port port to listen on.
         * @param game_options game options.
         * @throws ServerException
         */
        Server(network::port_t port, const GameOptions &game_options);

        /**
         * Constructs new Server instance.
         * @param port port port to listen on.
         * @param game_options game options.
         * @throws ServerException
         */
        Server(network::port_t port = SERVER_DEFAULT_PORT,
               GameOptions &&game_options = GameOptions());

        ~Server();

        /**
         * Implements IEventListener interface.
         * Creates a massage with new event to send to clients.
         * @param event event
         */
        void on_event(std::shared_ptr<Event> event) override;

        /**
         * Implements IConnectionListener interface.
         * Notifies the game about player disconnected.
         * @param name
         */
        void on_disconnect(const std::string &name) override;

        /**
         * Starts server loop, going forever until stop method is asynchronously
         * called.
         */
        void run() noexcept;

        /**
         * Stops running server.
         */
        void stop() noexcept;

    private:
        /**
         * Opens new UDP socket and saves it to the sock.
         * @throws ServerException when opening socket fails.
         */
        void open_socket();

        /**
         * Binds socket to the given port.
         * @param port port to bind socket to.
         * @throws ServerException when binding fails.
         */
        void bind_socket(network::port_t port);

        /**
         * Sends single message from messages queue.
         */
        void send_message();

        /**
         * Receives single message from socket.
         */
        void receive_message();

        /**
         * Handles received message for existing players.
         * @param client_address
         * @param message
         * @param connection_time
         */
        void player_action(sockaddr_in client_address, ClientMessage *message,
                           network::Connections::connection_t connection_time);

        /**
         * Handles received massage for new players.
         * @param client_address
         * @param message
         * @param connection_time
         */
        void player_connect(sockaddr_in client_address, ClientMessage *message,
                            network::Connections::connection_t connection_time);

        /**
         * Constructs message with events newer that next_event and adds it to
         * the queue. May add more than one message if events doesn't fit in
         * single message.
         * @param next_event
         * @param client_address
         */
        void make_message(event_no_t next_event, sockaddr_in client_address);
    };
}


#endif
