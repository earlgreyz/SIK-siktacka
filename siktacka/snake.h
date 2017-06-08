#ifndef SIK_SIKTACKA_PLAYER_H
#define SIK_SIKTACKA_PLAYER_H


#include "types.h"

namespace siktacka {
    using coordinate_x = double;
    using angle_t = int;

    class Snake {
    private:
        /// Snake current position x.
        coordinate_x x;
        /// Snake current position y
        coordinate_x y;
        /// Snake current turn angle.
        angle_t angle;

        /// Snake move direction.
        direction_t direction;

        /// Indicates whether snake is alive.
        bool alive;

        /**
         * Calculates snake x position in pixels.
         * @return x position.
         */
        pixel_t get_x() const noexcept;

        /**
         * Calculates snake y position in pixels.
         * @return y position.
         */
        pixel_t get_y() const noexcept;

    public:
        /**
         * Creates new snake at given position and turn angle.
         * @param x x position.
         * @param y y position.
         * @param angle turn angle.
         */
        Snake(coordinate_x x, coordinate_x y, angle_t angle) noexcept;

        /**
         * Calculates snake position in pixels.
         * @return snake position.
         */
        position_t get_position() const noexcept;

        /**
         * Changes snake move direction.
         * @param direction new move direction.
         */
        void turn(direction_t direction) noexcept;

        /**
         * Rotate snake and move by one unit in calculated direction.
         * @param speed turn speed.
         * @return whether snake actual (pixel) position has changed.
         */
        bool move(turn_speed_t speed) noexcept;

        /**
         * Sets snake alive attribute to false.
         */
        void die() noexcept;

        /**
         * Returns snake alive status.
         */
        bool is_alive() const noexcept;
    };
}


#endif
