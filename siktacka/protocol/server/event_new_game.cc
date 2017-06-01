#include "event_new_game.h"

using namespace siktacka;

EventNewGame::EventNewGame(pixel_t max_x, pixel_t max_y,
                           event_no_t event_no) noexcept
        : Event(event_no, event_t::NEW_GAME), max_x(max_x), max_y(max_y) {
    players_len = 0u;
}

sik::buffer_t EventNewGame::get_data() const noexcept {
    sik::buffer_t bytes(players_len + EVENT_NEW_GAME_HEADER_LEN);
    std::size_t off = 0u;
    char *data = bytes.data();

    *reinterpret_cast<pixel_t *>(data + off) = htobe32(max_x);
    off += sizeof(pixel_t);

    *reinterpret_cast<pixel_t *>(data + off) = htobe32(max_y);
    off += sizeof(pixel_t);

    for (const auto &player: players) {
        std::copy(player.begin(), player.end(), bytes.begin() + off);
        off += player.length();
        bytes[off++] = '\0';
    }
    return bytes;
}

void EventNewGame::add_player(const std::string &player) {
    add_player(std::string(player));
}

void EventNewGame::add_player(std::string &&player) {
    if (players_len + player.length() + 1 > MAX_EVENT_DATA_LEN) {
        throw std::overflow_error("Adding player would exceed max length");
    }
    players_len += player.length() + 1;
    players.push_back(player);
}

std::size_t EventNewGame::get_len() const noexcept {
    return Event::get_len() + players_len + EVENT_NEW_GAME_HEADER_LEN;
}
