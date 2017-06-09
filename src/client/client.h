#ifndef SIK_SIKTACKA_CLIENT_H
#define SIK_SIKTACKA_CLIENT_H


#include <memory>
#include <netinet/in.h>
#include <queue>
#include <list>
#include <mutex>
#include <thread>
#include "../network/sender.h"
#include "../network/receiver.h"
#include "../network/poll.h"
#include "../siktacka/protocol/client_message.h"
#include "../siktacka/events/event.h"
#include "gui_client.h"
#include "../siktacka/events/event_new_game.h"

namespace sikclient {
    class Client {
    private:
        /// Socket
        int server_sock;
        int gui_sock;
        /// Server address
        sockaddr_storage server_address;
        /// Indicates whether client loop should stop
        bool stopping = false;

        // Game message variables
        siktacka::game_t game_id;
        std::string name;
        siktacka::session_t session;
        siktacka::event_no_t event_no;

        std::unique_ptr<GuiClient> gui_client;

        /// Message sender
        std::unique_ptr<network::Sender> sender;
        /// Message receiver
        std::unique_ptr<network::Receiver> receiver;

        /// Poll
        std::unique_ptr<network::Poll<2>> poll;

        /// Messages to send to server
        std::queue<std::unique_ptr<siktacka::ClientMessage>> messages;
        std::mutex messages_mutex;

        std::thread client_thread;

        /// Events received
        std::queue<std::string> events;

        /// Players list
        std::vector<std::string> players;
        /// Game params
        siktacka::pixel_t width;
        siktacka::pixel_t height;
    public:
        Client(const std::string &name,
               const std::string &host, network::port_t port,
               const std::string &gui_host, network::port_t gui_port);

        ~Client();

        /**
         * Starts client main loop.
         */
        void run();

        /**
         * Stops client main loop.
         */
        void stop() noexcept;

    private:
        /**
         * Opens new UDP socket and saves it to the sock.
         * @throws ServerException when opening socket fails.
         */
        void open_sockets();

        /**
         * Prepares address structure.
         * @param host server host.
         * @param port server port.
         */
        void setup_address(const std::string &host, network::port_t port);


        /**
         * Calculates time in microseconds from epoch and saves it as session.
         */
        void make_session_id() noexcept;

        /**
         * Constructs new server message with current player params.
         */
        void add_server_message() noexcept;

        /**
         * Receives message from game server.
         */
        void receive_message();

        /**
         * Sends message to game server.
         */
        void send_message();

        /**
         * Sends event to GUI server.
         */
        void send_event();

        /**
         * Initializes players table based on the new game event.
         */
        void initialize_players(siktacka::EventNewGame *event);
    };
}


#endif
