#include <stdexcept>
#include "client_message.h"

using namespace siktacka;

ClientMessage::ClientMessage(session_t session_id, turn_t turn_direction,
                             event_t next_event, const std::string &player_name)
        : ClientMessage(session_id, turn_direction, next_event,
                        std::move(std::string(player_name))) {}

ClientMessage::ClientMessage(session_t session_id, turn_t turn_direction,
                             event_t next_event, std::string &&player_name)
        : session_id(session_id), turn_direction(turn_direction),
          next_event(next_event), player_name(player_name) {
    validate();
}

ClientMessage::ClientMessage(const std::string &bytes) {
    const char *data = bytes.c_str();
    std::size_t off = 0u;
    session_id = be64toh(*static_cast<session_t *>(data + off));
    off += sizeof(session_t);
    turn_direction = turn_t_from_value(*static_cast<std::int8_t *>(data + off));
    off += sizeof(turn_t);
    next_event = be32toh(*static_cast<event_t *>(data + off));
    off += sizeof(event_t);
    player_name = std::string(data + off);
    validate();
}

std::string ClientMessage::to_bytes() const noexcept {
    const std::size_t data_length =
            sizeof(session_t) + sizeof(turn_t) + sizeof(event_t);
    std::string bytes(data_length, 0);
    const char *data = bytes.c_str();
    std::size_t off = 0u;
    *static_cast<session_t *>(data + off) = htobe64(session_id);
    off += sizeof(session_t);
    *static_cast<turn_t *>(data + off) = turn_direction;
    off += sizeof(turn_t);
    *static_cast<event_t *>(data + off) = htobe32(next_event);
    bytes.append(player_name);
    return std::move(bytes);
}

void ClientMessage::validate() {
    if (player_name.length() > 64) {
        throw std::invalid_argument("Name cannot be longer than 64 characters");
    }
}
