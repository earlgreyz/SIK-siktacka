
#ifndef SIK_SIKTACKA_CONSTANTS_H
#define SIK_SIKTACKA_CONSTANTS_H

#include "../../types.h"


namespace siktacka {
    constexpr std::size_t MAX_MESSAGE_LEN = 512u;
    constexpr std::size_t MESSAGE_HEADER_LEN = sizeof(game_t);
    constexpr std::size_t MAX_MESSAGE_DATA_LEN =
            MAX_MESSAGE_LEN - MESSAGE_HEADER_LEN;

    constexpr std::size_t EVENT_HEADER_LEN =
            sizeof(event_len_t) + sizeof(event_no_t) + sizeof(event_t) +
            sizeof(crc32_t);
    constexpr std::size_t MAX_EVENT_DATA_LEN =
            MAX_MESSAGE_DATA_LEN - EVENT_HEADER_LEN;
}


#endif
