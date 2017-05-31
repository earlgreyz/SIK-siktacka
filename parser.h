#ifndef SIK_SITACKA_PARSER_H
#define SIK_SITACKA_PARSER_H


#include <cstddef>
#include <string>
#include <stdexcept>

class Parser {
public:
    std::size_t parse_width(const std::string &input);

    std::size_t parse_height(const std::string &input);

    std::uint16_t parse_port(const std::string &input);

    std::uint8_t parse_game_param(const std::string &input);
};


#endif
