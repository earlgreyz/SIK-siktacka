#ifndef SIK_SIKTACKA_TYPES_H
#define SIK_SIKTACKA_TYPES_H

#include <cstddef>
#include <cstdint>
#include <stdexcept>


namespace siktacka {
    using pixel_t = std::size_t;
    using rounds_t = std::uint16_t;
    using turn_speed_t = std::uint16_t;
    using seed_t = std::int64_t;

    using session_t = std::uint64_t;
    using event_t = std::uint8_t;
    using event_no_t = std::uint32_t;
    using event_len = std::uint32_t;
    using crc_t = std::uint32_t;

    enum turn_t : int8_t {
        Left = -1,
        Straight = 0,
        Right = 1
    };
}

#endif
