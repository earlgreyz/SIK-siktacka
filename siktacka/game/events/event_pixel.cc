#include "event_pixel.h"

using namespace siktacka;

EventPixel::EventPixel(event_no_t event_no, player_no_t player_no,
                       position_t position) noexcept
        : Event(event_no, event_t::PIXEL), player_no(player_no) {
    x = position.first;
    y = position.second;
}

network::buffer_t EventPixel::get_data() const noexcept {
    network::buffer_t bytes(EVENT_PIXEL_HEADER_LEN);
    char *data = bytes.data();
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
        throw std::invalid_argument("Unexpected end of data");
    }

    player_no = *reinterpret_cast<const player_no_t *>(data + off);
    off += sizeof(player_no_t);

    x = be32toh(*reinterpret_cast<const pixel_t *>(data + off));
    off += sizeof(pixel_t);

    y = be32toh(*reinterpret_cast<const pixel_t *>(data + off));
}
