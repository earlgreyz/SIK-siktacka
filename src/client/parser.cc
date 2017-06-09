#include "parser.h"
#include <boost/lexical_cast.hpp>
#include <arpa/inet.h>

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

    bool is_ipv6(const std::string &input) {
        struct sockaddr_in6 sa;
        return inet_pton(AF_INET6, input.c_str(), &(sa.sin6_addr)) != 0;
    }
}


std::uint16_t Parser::parse_port(const std::string &input) {
    std::size_t colon = input.find_last_of(':');
    if (is_ipv6(input) || colon == std::string::npos) {
        throw std::out_of_range("Port not available in the input string");
    }
    std::string port = input.substr(colon + 1);
    try {
        return positive_cast<std::uint16_t>(port);
    } catch (const boost::bad_lexical_cast &) {
        throw std::invalid_argument("Port must be a positive 16bit integer");
    }
}

std::string Parser::parse_address(const std::string &input) {
    if (is_ipv6(input)) {
        return input;
    }
    std::size_t colon = input.find_last_of(':');
    if (colon == std::string::npos) {
        return input;
    } else {
        return input.substr(0, colon);
    }
}