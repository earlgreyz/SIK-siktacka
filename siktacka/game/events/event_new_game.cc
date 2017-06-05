#include <sstream>
#include "event_new_game.h"

using namespace siktacka;

EventNewGame::EventNewGame(pixel_t max_x, pixel_t max_y,
                           event_no_t event_no) noexcept
        : Event(event_no, event_t::NEW_GAME), max_x(max_x), max_y(max_y) {
    players_len = 0u;
}

network::buffer_t EventNewGame::get_data() const noexcept {
    network::buffer_t bytes(players_len + EVENT_NEW_GAME_HEADER_LEN, '\0');
    std::size_t off = 0u;
    char *data = const_cast<char *>(bytes.data());

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
    if (player.length() > MAX_PLAYER_NAME_LENGTH) {
        throw std::invalid_argument(
                "Player name cannot be longer than 64 characters");
    }
    if (players_len + player.length() + 1 > MAX_EVENT_DATA_LEN) {
        throw std::overflow_error("Adding player would exceed max length");
    }
    players_len += player.length() + 1;
    players.insert(player);
}

std::size_t EventNewGame::get_len() const noexcept {
    return Event::get_len() + players_len + EVENT_NEW_GAME_HEADER_LEN;
}

EventNewGame::EventNewGame(event_no_t event_no, const char *data,
                           std::size_t length)
        : Event(event_no, event_t::NEW_GAME) {
    std::size_t off = 0u;

    // Check length to avoid unallocated memory access
    // 4u is at least 2 players with single letter and '\0'
    if (length < EVENT_NEW_GAME_HEADER_LEN + 4u) {
        throw std::invalid_argument("Unexpected end of data");
    }

    // Parse width
    max_x = be32toh(*reinterpret_cast<const pixel_t *>(data + off));
    off += sizeof(pixel_t);

    // Parse height
    max_y = be32toh(*reinterpret_cast<const pixel_t *>(data + off));
    off += sizeof(pixel_t);

    // Check if there is a null termination so string constructor ends properly
    if (*(data + length - 1) != '\0') {
        throw std::invalid_argument("Invalid username");
    }

    std::string last_player = "";

    // Parse names
    while (off < length) {
        std::string player_name(data + off);
        if (player_name < last_player) {
            throw std::invalid_argument("Players not in alphabetical order");
        }

        try {
            add_player(player_name);
            last_player = player_name;
        } catch (std::overflow_error &e) {
            // It should not happen but just in case we'll catch it
            throw std::invalid_argument(e.what());
        }
        off += player_name.length() + 1;
    }
}

std::set<std::string>::const_iterator EventNewGame::begin() const noexcept {
    return players.begin();
}

std::set<std::string>::const_iterator EventNewGame::end() const noexcept {
    return players.end();
}

std::string
EventNewGame::to_string(
        const std::vector<std::string> &players) const noexcept {
    std::ostringstream os;
    os << "NEW_GAME " << max_x << " " << max_y;
    for (const auto &player: players) {
        os << " " << player;
    }
    return os.str();
}
