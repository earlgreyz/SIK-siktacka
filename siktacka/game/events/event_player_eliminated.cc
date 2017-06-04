#include "event_player_eliminated.h"

using namespace siktacka;


EventPlayerEliminated::EventPlayerEliminated(event_no_t event_no,
                                             player_no_t player_no) noexcept
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
