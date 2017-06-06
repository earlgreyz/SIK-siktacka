#include <iostream>
#include <memory>
#include <csignal>
#include "siktacka/client/client.h"
#include "siktacka/client/parser.h"


namespace {
    std::string player_name;
    std::string server_host;
    network::port_t server_port = 12345u;
    std::string gui_host = "localhost";
    network::port_t gui_port = 12346u;
    std::unique_ptr<sikclient::Client> client;


    /**
     * Registers SIGINT signal.
     */
    void register_signals() {
        if (signal(SIGINT, [](int sig) {
            client->stop();
            std::cerr << "Signal " << sig << " Stopping server." << std::endl;
        }) == SIG_ERR) {
            throw std::runtime_error("Unable to register SIGINT signal");
        }
    }

    void parse_arguments(int argc, char *argv[]) {
        Parser parser;
        if (argc < 3 || argc > 4) {
            throw std::invalid_argument("Invalid arguments count");
        }
        player_name = argv[1];
        server_host = parser.parse_address(argv[2]);
        try {
            server_port = parser.parse_port(argv[2]);
        } catch (const std::out_of_range &) {
            // ipv6 provided or no colon found
        }

        if (argc == 4) {
            gui_host = parser.parse_address(argv[3]);
            try {
                gui_port = parser.parse_port(argv[3]);
            } catch (const std::out_of_range &) {
                // ipv6 provided or no colon found
            }
        }
    }

    void usage(const std::string &program, const std::string &error) {
        std::cerr << "ERROR: " << error << std::endl << std::endl
                  << "Usage: " << program << " player_name"
                  << " game_server_host[:port] [ui_server_host[:port]]"
                  << std::endl << std::endl
                  << "Options:" << std::endl
                  << "  player_name                   Player name"
                  << std::endl
                  << "  game_server_host              Game server hostname/ip"
                  << std::endl
                  << "  port (=12345)                 Optional game server port"
                  << std::endl
                  << "  ui_server_host (=localhost)   Optional gui server"
                  << " hostname/ip"
                  << std::endl
                  << "  port (=12346)                 Optional gui server port"
                  << std::endl;
    }
}

int main(int argc, char *argv[]) {
    try {
        parse_arguments(argc, argv);
    } catch (const std::invalid_argument &e) {
        usage(argv[0], e.what());
        return 1;
    }

    try {
        client = std::make_unique<sikclient::Client>(
                player_name, server_host, server_port, gui_host, gui_port);
        register_signals();
        client->run();
    } catch (const std::exception &e) {
        std::cerr << "Client stopped with message: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}