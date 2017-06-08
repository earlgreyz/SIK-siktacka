#ifndef SIK_SIKTACKA_BOARD_H
#define SIK_SIKTACKA_BOARD_H


#include <vector>
#include "../types.h"

namespace siktacka {
    class Board {
    private:
        std::vector<std::vector<bool>> fields;
        pixel_t width;
        pixel_t height;
    public:
        /**
         * Constructs new board with given dimensions.
         * @param width board width.
         * @param height board height.
         */
        Board(pixel_t width, pixel_t height) noexcept;

        /**
         * Marks all fields on board as empty.
         */
        void clear() noexcept;

        /**
         * Marks the given field as occupied.
         * @param position coordinates of the field.
         */
        void mark_occupied(position_t position) noexcept;

        /**
         * Checks if the given field is empty.
         * Fields outside the board dimensions are considered always occupied.
         * @param position coordinates of the field.
         * @return if the given field is on the board and is not occupied.
         */
        bool is_empty(position_t position) const noexcept;
    };
}


#endif
