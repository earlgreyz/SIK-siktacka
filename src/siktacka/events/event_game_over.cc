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

std::string
EventGameOver::to_string(
        const std::vector<std::string> &players) const noexcept {
    return "GAME_OVER " + players.size();
}
