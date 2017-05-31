#ifndef SIK_SIKTACKA_TYPES_H
#define SIK_SIKTACKA_TYPES_H

#include <cstddef>
#include <cstdint>


namespace siktacka {
    using pixel_t = std::size_t;
    using rounds_t = std::uint16_t;
    using turn_speed_t = std::uint16_t;
    using seed_t = std::int64_t;

    using session_t = std::uint64_t;
    using event_t = std::uint32_t;

    enum turn_t : int8_t {
        Left = -1,
        Straight = 0,
        Right = 1
    };

    turn_t turn_t_from_value(std::int8_t turn_value) {
        if (turn_value < -1 || turn_value > 1) {
            throw std::invalid_argument("Invalid turn direction value");
        }
        return static_cast<turn_t>(turn_value);
    }
}

#endif
