#include "snake.h"
#include <cmath>
#include <tuple>

using namespace siktacka;

Snake::Snake(coordinate_x x, coordinate_x y, angle_t angle) noexcept
        : x(x), y(y), angle(angle), direction(direction_t::NONE), alive(true) {
}

pixel_t Snake::get_x() const noexcept {
    return static_cast<pixel_t>(x);
}

pixel_t Snake::get_y() const noexcept {
    return static_cast<pixel_t>(y);
}

position_t Snake::get_position() const noexcept {
    return std::make_pair(get_x(), get_y());
}

void Snake::turn(direction_t direction) noexcept {
    this->direction = direction;
}

bool Snake::move(turn_speed_t speed) noexcept {
    position_t last_posiion = get_position();
    angle += static_cast<angle_t>((direction * speed) % 360);
    y += sin(angle * M_PI / 180);
    x += cos(angle * M_PI / 180);
    return last_posiion != get_position();
}

void Snake::die() noexcept {
    alive = false;
}

bool Snake::is_alive() const noexcept {
    return alive;
}
