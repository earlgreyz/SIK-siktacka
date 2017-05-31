#include <iostream>
#include <ctime>
#include <boost/program_options.hpp>

#include "error.h"


namespace {
    std::size_t width;
    std::size_t height;
    std::uint16_t port;
    std::uint16_t rounds_per_sec;
    std::uint16_t turning_speed;
    std::int64_t seed;

    namespace po = boost::program_options;

    /**
     * Program options setup.
     * @return
     */
    po::options_description prepare_description() {
        po::options_description description("Options");
        description.add_options()
                ("width,W",
                 po::value<std::size_t>(&width)->default_value(800u),
                 "Board width")
                ("height,H",
                 po::value<std::size_t>(&height)->default_value(600u),
                 "Board height")
                ("port,p",
                 po::value<std::uint16_t>(&port)->default_value(12345u),
                 "Port")
                ("rounds,s",
                 po::value<std::uint16_t>(&rounds_per_sec)->default_value(50u),
                 "Rounds per second")
                ("turn-speed,t",
                 po::value<std::uint16_t>(&turning_speed)->default_value(6u),
                 "Turning speed")
                ("seed,r",
                 po::value<std::int64_t>(&seed)->default_value(time(NULL)),
                 "Random number generator seed");
        return std::move(description);
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
}

int main(int argc, char *argv[]) {
    try {
        parse_arguments(argc, argv);
    } catch (const std::exception &) {
        return RETURN_ERROR;
    }
    return RETURN_OK;
}