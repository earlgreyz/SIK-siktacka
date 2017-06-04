#ifndef SIK_SIKTACKA_SERVER_H
#define SIK_SIKTACKA_SERVER_H

#include <string>
#include <memory>
#include <netinet/in.h>
#include <queue>

#include "types.h"
#include "game/game.h"
#include "game/events.h"
#include "game/i_event_listener.h"
#include "../error.h"
#include "../sik/types.h"
#include "../sik/poll.h"
#include "../sik/sender.h"
#include "../sik/receiver.h"
#include "../sik/connections.h"
#include "protocol/server/message.h"


namespace siktacka {
    const sik::port_t SERVER_DEFAULT_PORT = 12345u;

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
    class Server: public IEventListener {
        using MessageInstance =
        std::pair<std::unique_ptr<ServerMessage>, std::queue<sockaddr_in>>;
    private:
        /// Server UDP socket
        int sock;
        /// Server address bound to the socket.
        sockaddr_in address;
        /// Indicates whether server should stop main loop
        bool stopping;
        /// Poll for async IO operations.
        std::unique_ptr<sik::Poll<1>> poll;

        /// Current game
        std::unique_ptr<Events> events;
        std::unique_ptr<Game> game;
        std::queue<MessageInstance> messages;

        // Server helper structures
        std::unique_ptr<sik::Connections> connections;
        std::unique_ptr<sik::Sender> sender;
        std::unique_ptr<sik::Receiver> receiver;
    public:
        /**
         * Constructs new Server instance.
         * @param port port port to listen on.
         * @param game_options game options.
         * @throws ServerException
         */
        Server(sik::port_t port, const GameOptions &game_options);

        /**
         * Constructs new Server instance.
         * @param port port port to listen on.
         * @param game_options game options.
         * @throws ServerException
         */
        Server(sik::port_t port = SERVER_DEFAULT_PORT,
               GameOptions &&game_options = GameOptions());

        ~Server();

        void notify(std::unique_ptr<Event> event);

        void clear_events() {
            events->clear();
        }

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
        void bind_socket(sik::port_t port);

        void send_message();

        void receive_message();

        void add_message(event_no_t next_event, sockaddr_in client_address);
    };
}


#endif
