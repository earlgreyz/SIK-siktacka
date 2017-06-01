#ifndef SIK_SIKTACKA_TYPES_H
#define SIK_SIKTACKA_TYPES_H

#include <cstddef>
#include <cstdint>
#include <stdexcept>


namespace siktacka {
    using pixel_t = std::size_t;
    using rounds_t = std::uint16_t;
    using turn_speed_t = std::uint16_t;
    using rand_t = std::uint32_t;

    using game_t = std::uint32_t;
    using session_t = std::uint64_t;
    using event_no_t = std::uint32_t;
    using event_len_t = std::uint32_t;
    using crc32_t = std::uint32_t;

    enum turn_t: std::int8_t {
        LEFT = -1,
        NONE = 0,
        RIGHT = 1
    };

    enum event_t: std::uint8_t {
        NEW_GAME = 0,
        PIXEL = 1,
        PLAYER_ELIMINATED = 2,
        GAME_OVER = 3
    };

    using player_no_t = std::uint8_t;
}

#endif
