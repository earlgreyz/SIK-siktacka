#ifndef SIK_SITACKA_PARSER_H
#define SIK_SITACKA_PARSER_H


#include <cstddef>
#include <string>
#include <stdexcept>
#include "../network/types.h"

class Parser {
public:
    /**
     * Parses string with format hostname[:port], IPv4[:port].
     * @param input input string.
     * @throws std::out_of_range if string does not contain :port postfix.
     * @throws std::invalid_argument if port is invalid.
     * @return port.
     */
    network::port_t parse_port(const std::string &input);

    /**
     * Parses string with format hostname[:port], IPv4[:port], IPv6.
     * Returns hostname, IPv4 or IPv6 after removing port postfix.
     * @param input input string.
     * @return hostname.
     */
    std::string parse_address(const std::string &input);
};


#endif
