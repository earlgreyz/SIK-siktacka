#ifndef SIK_SIKTACKA_PLAYER_H
#define SIK_SIKTACKA_PLAYER_H


#include "../types.h"

namespace siktacka {
    using coordinate_x = double;
    using angle_t = int;

    class Player {
    private:
        /// Player current position x.
        coordinate_x x;
        /// Player current position y
        coordinate_x y;
        /// Player current turn angle.
        angle_t angle;

        /// Player move direction.
        direction_t direction;

        /**
         * Calculates player x position in pixels.
         * @return x position.
         */
        pixel_t get_x() const noexcept;

        /**
         * Calculates player y position in pixels.
         * @return y position.
         */
        pixel_t get_y() const noexcept;

    public:
        /**
         * Creates new player at given position and turn angle.
         * @param x x position.
         * @param y y position.
         * @param angle turn angle.
         */
        Player(coordinate_x x, coordinate_x y, angle_t angle) noexcept;

        /**
         * Calculates player position in pixels.
         * @return player position.
         */
        position_t get_position() const noexcept;

        /**
         * Changes player move direction.
         * @param direction new move direction.
         */
        void turn(direction_t direction) noexcept;

        /**
         * Rotate player and move by one unit in calculated direction.
         * @param speed turn speed.
         * @return whether player actual (pixel) position has changed.
         */
        bool move(turn_speed_t speed) noexcept;
    };
}


#endif
