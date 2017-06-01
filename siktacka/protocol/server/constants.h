
#ifndef SIK_SIKTACKA_CONSTANTS_H
#define SIK_SIKTACKA_CONSTANTS_H

#include "../../types.h"


namespace siktacka {
    constexpr std::size_t MAX_MESSAGE_LEN = 512u;
    constexpr std::size_t MESSAGE_HEADER_LEN = sizeof(game_t);
    constexpr std::size_t MAX_MESSAGE_DATA_LEN =
            MAX_MESSAGE_LEN - MESSAGE_HEADER_LEN;
}


#endif
