#include <boost/crc.hpp>
#include "event_factory.h"
#include "event_new_game.h"
#include "event_pixel.h"
#include "event_player_eliminated.h"

using namespace siktacka;

namespace {
    crc32_t get_crc(const network::buffer_t &bytes, std::size_t offset,
                    std::size_t size) noexcept {
        boost::crc_32_type result;
        result.process_bytes(bytes.data() + offset, size);
        return static_cast<crc32_t>(result.checksum());
    }
}


std::shared_ptr<Event>
EventFactory::make(const network::buffer_t &buffer, std::size_t offset) const {
    const char *data = buffer.data() + offset;
    std::size_t data_len;
    std::size_t off = 0;

    event_len_t event_len;
    event_no_t event_no;
    event_t event_type;
    const char *event_data;
    crc32_t crc;

    // Parse event length
    event_len = be32toh(*reinterpret_cast<const event_len_t *>(data));
    off += sizeof(event_len_t);

    // Whole event size
    data_len = event_len + sizeof(event_len_t) + sizeof(crc32_t);

    // Check length to avoid unallocated memory access
    if (data_len > buffer.size() - offset) {
        throw invalid_crc();
    }

    // Parse CRC and check if it matches the proper one
    crc = be32toh(*reinterpret_cast<const crc32_t *>(data + off + event_len));
    if (crc != get_crc(buffer, offset, event_len + sizeof(event_len_t))) {
        throw invalid_crc();
    }

    // Parse event number
    event_no = be32toh(*reinterpret_cast<const event_no_t *>(data + off));
    off += sizeof(event_no_t);

    // Parse event type
    event_type = *reinterpret_cast<const event_t *>(data + off);
    off += sizeof(event_t);

    // Prepare to parse event data
    event_data = data + off;
    data_len = event_len - sizeof(event_no_t) - sizeof(event_t);

    switch (event_type) {
        case event_t::NEW_GAME:
            return std::make_shared<EventNewGame>(
                    event_no, event_data, data_len);
        case event_t::PIXEL:
            return std::make_shared<EventPixel>(
                    event_no, event_data, data_len);
        case event_t::PLAYER_ELIMINATED:
            return std::make_shared<EventPlayerEliminated>(
                    event_no, event_data, data_len);
        case event_t::GAME_OVER:
            break;
        default:
            throw unknown_event(
                    sizeof(event_len_t) + event_len + sizeof(crc32_t));
    }

    return std::shared_ptr<Event>();
}
