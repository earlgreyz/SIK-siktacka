#include "parser.h"
#include <boost/lexical_cast.hpp>

namespace {
    /**
     * Performs boost::lexical cast with additional check if input is a positive
     * number.
     * @tparam T cast type
     * @param input string to parse
     * @return parsed value
     */
    template<typename T>
    static T positive_cast(const std::string &input) {
        T value = boost::lexical_cast<T>(input);
        if (boost::lexical_cast<std::string>(value) != input || value == 0) {
            throw boost::bad_lexical_cast();
        }
        return value;
    }
}

std::size_t Parser::parse_width(const std::string &input) {
    try {
        return positive_cast<std::size_t>(input);
    } catch (const boost::bad_lexical_cast &) {
        throw std::invalid_argument("Width must be a positive integer");
    }
}

std::size_t Parser::parse_height(const std::string &input) {
    try {
        return positive_cast<std::size_t>(input);
    } catch (const boost::bad_lexical_cast &) {
        throw std::invalid_argument("Height must be a positive integer");
    }
}

std::uint16_t Parser::parse_port(const std::string &input) {
    try {
        return positive_cast<std::uint16_t>(input);
    } catch (const boost::bad_lexical_cast &) {
        throw std::invalid_argument("Port must be a positive 16bit integer");
    }
}

std::uint8_t Parser::parse_game_param(const std::string &input) {
    try {
        return positive_cast<std::uint8_t>(input);
    } catch (const boost::bad_lexical_cast &) {
        throw std::invalid_argument("Game param must be a positive integer");
    }
}