#include "event.h"

#include "../../utility.h"
#include <boost/crc.hpp>

using namespace siktacka;

namespace {
    crc_t get_crc(const std::string &bytes) noexcept {
        boost::crc_32_type result;
        result.process_bytes(bytes.data(), bytes.length());
        return static_cast<crc_t>(result.checksum());
    }
}

std::string siktacka::Event::to_bytes() const noexcept {
    std::size_t data_length =
            sizeof(event_len) + sizeof(event_no_t) + sizeof(event_t);
    std::string bytes(data_length, 0);
    const char *data = bytes.c_str();
    std::size_t off = 0u;
    bytes.append(std::move(get_data()));
    const event_len len =
            static_cast<event_len>(bytes.length()) - sizeof(event_len);
    *ptr_cast<event_len *>(data + off) = htobe32(len);
    off += sizeof(event_len);
    *ptr_cast<event_no_t *>(data + off) = htobe32(event_no);
    off += sizeof(event_no_t);
    *ptr_cast<event_t *>(data + off) = event_type;
    off = bytes.length();
    crc_t crc = get_crc(bytes);
    bytes.resize(off + sizeof(crc_t), 0);
    *ptr_cast<crc_t *>(data + off) = htobe32(crc);
    return std::move(bytes);
}
