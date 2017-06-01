#include "message.h"

using namespace siktacka;


ServerMessage::ServerMessage(game_t game_id) noexcept
        : game_id(game_id) {
    message_len = 0u;
}

void ServerMessage::add_event(std::unique_ptr<Event> event) {
    if (message_len + event->get_len() > MAX_MESSAGE_DATA_LEN) {
        throw std::overflow_error("Adding event would cause overflow");
    }
    events.push_back(std::move(event));
    message_len += event->get_len();
}

sik::buffer_t ServerMessage::to_bytes() const noexcept {
    sik::buffer_t bytes(sizeof(game_t));
    *reinterpret_cast<game_t *>(bytes.data()) = htobe32(game_id);
    for (const auto &event: events) {
        sik::buffer_t event_bytes(event->to_bytes());
        std::move(event_bytes.begin(), event_bytes.end(),
                  std::back_inserter(bytes));
    }
    return bytes;
}
