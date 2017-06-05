#include <stdexcept>
#include "message.h"

using namespace siktacka;

namespace {
    direction_t turn_from_value(std::int8_t turn_value) {
        if (turn_value < -1 || turn_value > 1) {
            throw std::invalid_argument("Invalid turn value");
        }
        return static_cast<direction_t>(turn_value);
    }
}


ClientMessage::ClientMessage(session_t session_id, direction_t turn_direction,
                             event_no_t next_event,
                             const std::string &player_name)
        : ClientMessage(session_id, turn_direction, next_event,
                        std::string(player_name)) {}

ClientMessage::ClientMessage(session_t session_id, direction_t turn_direction,
                             event_no_t next_event, std::string &&player_name)
        : session_id(session_id), turn_direction(turn_direction),
          next_event(next_event), player_name(player_name) {
    validate();
}

ClientMessage::ClientMessage(const network::buffer_t &bytes) {
    const char *data = bytes.data();
    std::size_t off = 0u;

    session_id = be64toh(*reinterpret_cast<const session_t *>(data + off));
    off += sizeof(session_t);

    turn_direction = turn_from_value(
            *reinterpret_cast<const std::int8_t *>(data + off));
    off += sizeof(direction_t);

    next_event = be32toh(*reinterpret_cast<const event_no_t *>(data + off));
    off += sizeof(event_no_t);
    player_name = std::string(data + off, bytes.size() - off);

    validate();
}

network::buffer_t ClientMessage::to_bytes() const noexcept {
    const std::size_t data_length =
            sizeof(session_t) + sizeof(direction_t) + sizeof(event_no_t);
    network::buffer_t bytes(data_length, '\0');
    char *data = const_cast<char *>(bytes.data());
    std::size_t off = 0u;

    *reinterpret_cast<session_t *>(data + off) = htobe64(session_id);
    off += sizeof(session_t);

    *reinterpret_cast<direction_t *>(data + off) = turn_direction;
    off += sizeof(direction_t);

    *reinterpret_cast<event_no_t *>(data + off) = htobe32(next_event);
    std::copy(player_name.begin(), player_name.end(),
              std::back_inserter(bytes));
    return bytes;
}

void ClientMessage::validate() {
    if (player_name.length() > 64) {
        throw std::invalid_argument("Name cannot be longer than 64 characters");
    }
    for (const char &letter : player_name) {
        if (letter < static_cast<char>(33) || letter > static_cast<char>(126)) {
            throw std::invalid_argument("Forbidden characters in player name");
        }
    }
}

session_t ClientMessage::get_session() const noexcept {
    return session_id;
}

direction_t ClientMessage::get_turn_direction() const noexcept {
    return turn_direction;
}

event_no_t ClientMessage::get_next_event_no() const noexcept {
    return next_event;
}

const std::string &ClientMessage::get_player_name() const noexcept {
    return player_name;
}
