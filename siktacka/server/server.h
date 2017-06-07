#ifndef SIK_SIKTACKA_SERVER_H
#define SIK_SIKTACKA_SERVER_H


#include <netinet/in.h>
#include <mutex>
#include "../../network/types.h"
#include "../game/game.h"
#include "../protocol/client/message.h"
#include "../../network/socket.h"
#include "../../network/poll.h"
#include "../../network/sender.h"
#include "../../network/receiver.h"
#include "i_connection_listener.h"
#include "connections.h"
#include "buffer.h"

namespace sikserver {
    const network::port_t SERVER_DEFAULT_PORT = 12345u;

    class Server : public siktacka::IEventListener, public IConnectionListener {
    private:
        /// Indicates whether server should stop main loop
        bool stopping;
        /// Server socket
        std::unique_ptr<network::Socket> socket;
        /// Poll for async IO operations.
        std::unique_ptr<network::Poll<1>> poll;

        /// Game events
        std::unique_ptr<siktacka::Events> events;
        /// The actual game server
        std::unique_ptr<siktacka::Game> game;

        /// Clients connected
        std::unique_ptr<Connections> connections;
        std::mutex connections_mutex;
        /// Message sender
        std::unique_ptr<network::Sender> sender;
        /// Message receiver
        std::unique_ptr<network::Receiver> receiver;

        /// Messages buffer
        std::unique_ptr<Buffer> messages;
        /// Mutex on messages
        std::mutex messages_mutex;

        /// Message queued to be sent next
        network::buffer_t current_message;
        /// Recipients of the current message
        std::queue<sockaddr_storage> current_addresses;
    public:
        /**
         * Constructs new Server instance.
         * @param port port port to listen on.
         * @param game_options game options.
         * @throws ServerException
         */
        Server(network::port_t port, const siktacka::GameOptions &game_options);

        /**
         * Constructs new Server instance.
         * @param port port port to listen on.
         * @param game_options game options.
         * @throws ServerException
         */
        Server(network::port_t port = SERVER_DEFAULT_PORT,
               siktacka::GameOptions &&game_options = siktacka::GameOptions());

        /**
         * Runs server main loop. May be interrupted by calling stop
         * asynchronously from another thread or via system interrupt.
         */
        void run();

        /**
         * Stops server.
         */
        void stop() noexcept;

        /**
         * Implements IEventListener interface.
         * Creates a massage with new event to send to clients.
         * @param event event
         */
        void on_event(std::shared_ptr<siktacka::Event> event) override;

        /**
         * Implements IConnectionListener interface.
         * Sends game the message about player disconnected.
         * @param name
         */
        void on_disconnect(const std::string &name) override;

    private:
        /**
         * Sends single message from messages queue.
         */
        void send_message();

        /**
         * Receives single message from socket.
         */
        void receive_message();

        /**
         * Checks messages queue has a message available, discarding the ones
         * with empty clients queue and setting.
         * @return whether a message and a client pair for sending a message
         * is available
         */
        bool can_send_message() noexcept;

        /**
         * Creates message for given address with events starting from
         * event_no. May create more than one message if events doesn't fit
         * in one. Thread safe.
         * @param address client address.
         * @param event_no next expected event number.
         */
        void
        make_message(sockaddr_storage address, siktacka::event_no_t event_no);

        /**
         * Adds message to the buffer. Thread safe.
         * @param message
         */
        void add_message(Buffer::Message message) noexcept;

        /**
         * Tries to perform game action based on the client message.
         * @throws std::invalid_argument if client name doesn't match
         * @throws std::out_of_range if the client does not exist.
         * @param message player message.
         * @param address player address.
         * @param now timestamp the message was received.
         */
        void on_action(std::shared_ptr<siktacka::ClientMessage> message,
                       sockaddr_storage address, connection_t now);

        /**
         * Adds player to the connected clients and to the game.
         * @param message player message.
         * @param address player address
         * @param now timestamp the message was received.
         */
        void on_connect(std::shared_ptr<siktacka::ClientMessage> message,
                        sockaddr_storage address, connection_t now) noexcept;
    };
}


#endif
