#ifndef SIK_SIKTACKA_SERVER_H
#define SIK_SIKTACKA_SERVER_H

#include <string>
#include <memory>
#include <netinet/in.h>

#include "types.h"
#include "../error.h"
#include "../sik/types.h"
#include "../sik/poll.h"


namespace siktacka {
    const pixel_t SERVER_DEFAULT_WIDTH = 800u;
    const pixel_t SERVER_DEFAULT_HEIGHT = 600u;
    const sik::port_t SERVER_DEFAULT_PORT = 12345u;
    const rounds_t SERVER_DEFAULT_ROUNDS = 50u;
    const turn_t SERVER_DEFAULT_TURN = 6u;
    const seed_t SERVER_DEFAULT_SEED = time(NULL);

    struct GameOptions {
        pixel_t width = SERVER_DEFAULT_WIDTH;
        pixel_t height = SERVER_DEFAULT_HEIGHT;
        rounds_t rounds_per_sec = SERVER_DEFAULT_ROUNDS;
        turn_t turning_speed = SERVER_DEFAULT_TURN;
        seed_t seed = SERVER_DEFAULT_SEED;
    };

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
    class Server {
    private:
        /// Options for current game
        GameOptions game_options;
        /// Server UDP socket
        int sock;
        /// Server address bound to the socket.
        sockaddr_in address;
        /// Indicates whether server should stop main loop
        bool stopping;
        /// Poll for async IO operations.
        std::unique_ptr<sik::Poll<1>> poll;

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

        /**
         * Starts server loop, going forever until stop method is asynchronously
         * called.
         */
        void run() noexcept;

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
    };
}

#endif
