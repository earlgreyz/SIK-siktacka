#include "event_game_over.h"

using namespace siktacka;

network::buffer_t EventGameOver::get_data() const noexcept {
    return network::buffer_t();
}

EventGameOver::EventGameOver(event_no_t event_no) noexcept
        : Event(event_no, event_t::GAME_OVER) {
}

EventGameOver::EventGameOver(event_no_t event_no, size_t length)
        : Event(event_no, event_t::GAME_OVER) {
    if (length != 0) {
        throw std::invalid_argument("Unexpected data in GAME_OVER");
    }
}

std::string EventGameOver::to_string(
        __attribute__((unused)) const std::vector<std::string> &players)
const noexcept {
    return "GAME_OVER";
}

void EventGameOver::validate_in_game(
        __attribute__((unused)) pixel_t width,
        __attribute__((unused))pixel_t height,
        __attribute__((unused)) std::size_t players_count) const {
    // Game over is always valid as it has no data
    return;
}