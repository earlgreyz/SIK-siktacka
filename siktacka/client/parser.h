#ifndef SIK_SITACKA_PARSER_H
#define SIK_SITACKA_PARSER_H


#include <cstddef>
#include <string>
#include <stdexcept>

class Parser {
public:
    std::uint16_t parse_port(const std::string &input);
    std::string parse_address(const std::string &input);
};


#endif
