#include <sstream>
#include "event_pixel.h"

using namespace siktacka;

EventPixel::EventPixel(event_no_t event_no, player_no_t player_no,
                       position_t position) noexcept
        : Event(event_no, event_t::PIXEL), player_no(player_no) {
    x = position.first;
    y = position.second;
}

network::buffer_t EventPixel::get_data() const noexcept {
    network::buffer_t bytes(EVENT_PIXEL_HEADER_LEN, '\0');
    char *data = const_cast<char *>(bytes.data());
    std::size_t off = 0u;

    *reinterpret_cast<player_no_t *>(data + off) = player_no;
    off += sizeof(player_no_t);

    *reinterpret_cast<pixel_t *>(data + off) = htobe32(x);
    off += sizeof(pixel_t);

    *reinterpret_cast<pixel_t *>(data + off) = htobe32(y);
    return bytes;
}

std::size_t EventPixel::get_len() const noexcept {
    return Event::get_len() + EVENT_PIXEL_HEADER_LEN;
}

EventPixel::EventPixel(event_no_t event_no, const char *data,
                       std::size_t length)
        : Event(event_no, event_t::PIXEL) {
    std::size_t off = 0u;

    // Check length to avoid unallocated memory access
    if (length != EVENT_PIXEL_HEADER_LEN) {
        throw std::invalid_argument("Unexpected data in PIXEL");
    }

    player_no = *reinterpret_cast<const player_no_t *>(data + off);
    off += sizeof(player_no_t);

    x = be32toh(*reinterpret_cast<const pixel_t *>(data + off));
    off += sizeof(pixel_t);

    y = be32toh(*reinterpret_cast<const pixel_t *>(data + off));
}

std::string
EventPixel::to_string(const std::vector<std::string> &players) const {
    std::ostringstream os;
    if (players.size() < player_no) {
        throw std::invalid_argument("Player with given number does not exist");
    }
    os << "PIXEL " << x << " " << y << " " << players[player_no] << "\n";
    return os.str();
}

void EventPixel::validate_in_game(pixel_t width, pixel_t height,
                                  std::size_t players_count) const {
    if (x >= width || y >= height) {
        throw std::invalid_argument("PIXEL has invalid coordinates!");
    }

    if (player_no >= players_count) {
        throw std::invalid_argument("PIXEL for non existent user!");
    }
}
