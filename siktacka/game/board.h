#ifndef SIK_SIKTACKA_BOARD_H
#define SIK_SIKTACKA_BOARD_H


#include <vector>
#include "../types.h"

namespace siktacka {
    class Board {
    private:
        std::vector<std::vector<bool>> fields;
    public:
        Board(pixel_t width, pixel_t height) noexcept;
        void clear() noexcept;
        void mark_occupied(position_t position) noexcept;
        bool is_empty(position_t position) const noexcept;
    };
}


#endif
