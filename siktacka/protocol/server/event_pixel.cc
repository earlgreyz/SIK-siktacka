#include "event_pixel.h"

using namespace siktacka;

EventPixel::EventPixel(event_no_t event_no, player_no_t player_no, pixel_t x,
                       pixel_t y) noexcept
        : Event(event_no, event_t::PIXEL), player_no(player_no), x(x), y(y) {

}

sik::buffer_t EventPixel::get_data() const noexcept {
    sik::buffer_t bytes(EVENT_PIXEL_HEADER_LEN);
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
