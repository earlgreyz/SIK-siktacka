#include "player.h"
#include <cmath>
#include <tuple>

using namespace siktacka;

Player::Player(coordinate_x x, coordinate_x y, angle_t angle) noexcept
        : x(x), y(y), angle(angle) {

}

pixel_t Player::get_x() const noexcept {
    return static_cast<pixel_t>(x);
}

pixel_t Player::get_y() const noexcept {
    return static_cast<pixel_t>(y);
}

position_t Player::get_position() const noexcept {
    return std::make_pair(get_x(), get_y());
}

void Player::turn(direction_t direction) {
    this->direction = direction;
}

bool Player::move(turn_speed_t speed) noexcept {
    position_t last_posiion = get_position();
    angle += static_cast<angle_t>(angle * speed) % 360;
    y += sin(y * M_PI / 180);
    x += cos(x * M_PI / 180);
    return last_posiion != get_position();
}
