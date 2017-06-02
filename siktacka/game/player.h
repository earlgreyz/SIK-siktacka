#ifndef SIK_SIKTACKA_PLAYER_H
#define SIK_SIKTACKA_PLAYER_H


#include "../types.h"

namespace siktacka {
    using coordinate_x = double;
    using turn_t = std::uint16_t;

    class Player {
    private:
        coordinate_x x;
        coordinate_x y;
        turn_t turn;

        pixel_t get_x() const noexcept;

        pixel_t get_y() const noexcept;

    public:
        Player(coordinate_x x, coordinate_x y, turn_t turn) noexcept;

        position_t get_position() const noexcept;

        bool move(direction_t direction, turn_speed_t speed) noexcept;
    };
}


#endif
