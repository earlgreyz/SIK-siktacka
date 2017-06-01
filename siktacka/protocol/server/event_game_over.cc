#include "event_game_over.h"

using namespace siktacka;

sik::buffer_t EventGameOver::get_data() const noexcept {
    return sik::buffer_t();
}

EventGameOver::EventGameOver(event_no_t event_no) noexcept
        : Event(event_no, event_t::GAME_OVER) {
}
