#ifndef SIK_SIKTACKA_SERVER_H
#define SIK_SIKTACKA_SERVER_H


#include <netinet/in.h>
#include <mutex>
#include "../../network/types.h"
#include "../game/game.h"
#include "../../network/poll.h"
#include "../../network/sender.h"
#include "../../network/receiver.h"
#include "i_connection_listener.h"
#include "connections.h"

namespace sikserver {
    const network::port_t SERVER_DEFAULT_PORT = 12345u;

    class Server : public siktacka::IEventListener, public IConnectionListener {
    private:
        /// Server UDP socket
        int sock;
        /// Server address bound to the socket.
        sockaddr_in address;
        /// Indicates whether server should stop main loop
        bool stopping;
        /// Poll for async IO operations.
        std::unique_ptr<network::Poll<1>> poll;

        std::unique_ptr<siktacka::Events> events;
        std::unique_ptr<siktacka::Game> game;
        std::unique_ptr<Connections> connections;

        std::unique_ptr<network::Sender> sender;
        std::unique_ptr<network::Receiver> receiver;

        struct MessageInstance {
            network::buffer_t message;
            std::queue<sockaddr_in> addresses;

            MessageInstance(network::buffer_t, sockaddr_in);
            MessageInstance(network::buffer_t, std::queue<sockaddr_in> &&);
        };

        std::queue<MessageInstance> messages;
        std::mutex messages_mutex;

        network::buffer_t current_message;
        std::queue<sockaddr_in> current_addresses;
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

        ~Server();

        void run();

        void stop() noexcept;


        /**
         * Implements IEventListener interface.
         * Creates a massage with new event to send to clients.
         * @param event event
         */
        void on_event(std::shared_ptr<siktacka::Event> event) override;

        void on_disconnect(const std::string &name) override;

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

        void make_response_message(sockaddr_in address, siktacka::event_no_t event_no);
    };
}


#endif
