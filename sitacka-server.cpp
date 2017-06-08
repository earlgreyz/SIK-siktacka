#include <iostream>
#include <ctime>
#include <csignal>
#include <memory>
#include <boost/program_options.hpp>

#include "error.h"
#include "server/server.h"


namespace {
    network::port_t server_port;
    siktacka::GameOptions game_options;
    std::unique_ptr<sikserver::Server> server;

    namespace po = boost::program_options;

    /**
     * Program options setup.
     * @return
     */
    po::options_description prepare_description() {
        po::options_description description("Options");
        description.add_options()
                ("width,W",
                 po::value<siktacka::pixel_t>(
                         &game_options.width)->default_value(
                         siktacka::SERVER_DEFAULT_WIDTH),
                 "Board width")
                ("height,H",
                 po::value<siktacka::pixel_t>(
                         &game_options.height)->default_value(
                         siktacka::SERVER_DEFAULT_HEIGHT),
                 "Board height")
                ("port,p",
                 po::value<network::port_t>(
                         &server_port)->default_value(
                         sikserver::SERVER_DEFAULT_PORT),
                 "Port")
                ("rounds,s",
                 po::value<siktacka::rounds_t>(
                         &game_options.rounds_per_sec)->default_value(
                         siktacka::SERVER_DEFAULT_ROUNDS),
                 "Rounds per second")
                ("turn-speed,t",
                 po::value<siktacka::turn_speed_t>(
                         &game_options.turning_speed)->default_value(
                         siktacka::SERVER_DEFAULT_TURN),
                 "Turning speed")
                ("seed,r",
                 po::value<siktacka::rand_t>(
                         &game_options.seed)->default_value(
                         time(NULL)),
                 "Random number generator seed");
        return description;
    }

    /**
     * Parses arguments provided by user.
     * @param argc arguments count
     * @param argv argument values
     */
    void parse_arguments(int argc, char *argv[]) {
        po::options_description description = prepare_description();
        po::variables_map variables_map;

        try {
            po::store(po::command_line_parser(argc, argv).options(
                    description).run(), variables_map);
            po::notify(variables_map);
        } catch (const po::error &e) {
            std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
            std::cerr << description << std::endl;
            throw std::invalid_argument(e.what());
        }
    }

    /**
     * Registers SIGINT signal.
     */
    void register_signals() {
        if (signal(SIGINT, [](int sig) {
            server->stop();
            std::cerr << "Signal " << sig << " Stopping server." << std::endl;
        }) == SIG_ERR) {
            throw std::runtime_error("Unable to register SIGINT signal");
        }
    }
}

int main(int argc, char *argv[]) {
    try {
        parse_arguments(argc, argv);
        server = std::make_unique<sikserver::Server>(server_port, game_options);
        register_signals();

        server->run();
    } catch (const std::exception &e) {
        std::cout << "Server stopped with " << e.what() << std::endl;
        return RETURN_ERROR;
    }
    return RETURN_OK;
}