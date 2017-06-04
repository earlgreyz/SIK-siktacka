#include "event_player_eliminated.h"

using namespace siktacka;


EventPlayerEliminated::EventPlayerEliminated(
        event_no_t event_no, player_no_t player_no) noexcept
        : Event(event_no, event_t::PLAYER_ELIMINATED), player_no(player_no) {

}

network::buffer_t EventPlayerEliminated::get_data() const noexcept {
    network::buffer_t bytes(EVENT_PLAYER_ELIMINATED_HEADER_LEN);
    *reinterpret_cast<player_no_t *>(bytes.data()) = player_no;
    return bytes;
}

std::size_t EventPlayerEliminated::get_len() const noexcept {
    return Event::get_len() + EVENT_PLAYER_ELIMINATED_HEADER_LEN;
}

EventPlayerEliminated::EventPlayerEliminated(
        event_no_t event_no, const char *data, std::size_t length)
        : Event(event_no, event_t::PLAYER_ELIMINATED) {

    // Check length to avoid unallocated memory access
    if (length != EVENT_PLAYER_ELIMINATED_HEADER_LEN) {
        throw std::invalid_argument("Unexpected end of data");
    }

    player_no = *reinterpret_cast<const player_no_t *>(data);
}
