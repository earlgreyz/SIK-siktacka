#include "event.h"
#include <boost/crc.hpp>

using namespace siktacka;

namespace {
    crc32_t get_crc(const network::buffer_t &bytes) noexcept {
        boost::crc_32_type result;
        result.process_bytes(bytes.data(), bytes.size());
        return static_cast<crc32_t>(result.checksum());
    }
}

Event::Event(event_no_t event_no, event_t event_type)
        : event_no(event_no), event_type(event_type) {}

network::buffer_t siktacka::Event::to_bytes() const noexcept {
    std::size_t header_len =
            sizeof(event_len_t) + sizeof(event_no_t) + sizeof(event_t);
    network::buffer_t bytes(header_len);
    char * data = bytes.data();
    std::size_t off = 0u;

    network::buffer_t event_data(get_data());
    std::move(event_data.begin(), event_data.end(), std::back_inserter(bytes));

    const event_len_t len =
            static_cast<event_len_t>(bytes.size()) - sizeof(event_len_t);

    *reinterpret_cast<event_len_t *>(data + off) = htobe32(len);
    off += sizeof(event_len_t);

    *reinterpret_cast<event_no_t *>(data + off) = htobe32(event_no);
    off += sizeof(event_no_t);

    *reinterpret_cast<event_t *>(data + off) = event_type;
    off = bytes.size();

    crc32_t crc = get_crc(bytes);
    bytes.resize(off + sizeof(crc32_t), 0);
    *reinterpret_cast<crc32_t *>(data + off) = htobe32(crc);
    return bytes;
}

std::size_t Event::get_len() const noexcept {
    return EVENT_HEADER_LEN;
}

event_no_t Event::get_event_no() const noexcept {
    return event_no;
}

event_t Event::get_event_type() const noexcept {
    return event_type;
}
