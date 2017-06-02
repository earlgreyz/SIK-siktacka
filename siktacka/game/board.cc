#include "board.h"

using namespace siktacka;

namespace {
    constexpr bool EMPTY = false;
    constexpr bool OCCUPIED = true;
}

Board::Board(pixel_t width, pixel_t height) noexcept {
    fields = std::vector<std::vector<bool>>(width);
    for (pixel_t x = 0; x < width; x++) {
        fields[x] = std::vector<bool>(height);
    }
    clear();
}

void Board::clear() noexcept {
    for (pixel_t x = 0; x < fields.size(); x++) {
        for (pixel_t y = 0; y < fields[x].size(); y++) {
            fields[x][y] = EMPTY;
        }
    }
}

void Board::mark_occupied(position_t position) noexcept {
    fields[position.first][position.second] = OCCUPIED;
}

bool Board::is_empty(position_t position) const noexcept {
    return fields[position.first][position.second] == EMPTY;
}


