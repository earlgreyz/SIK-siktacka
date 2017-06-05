#include "message.h"
#include "../../game/events/event_factory.h"

using namespace siktacka;


ServerMessage::ServerMessage(game_t game_id) noexcept
        : game_id(game_id) {
    message_len = 0u;
}

void ServerMessage::add_event(std::shared_ptr<Event> event) {
    if (message_len + event->get_len() > MAX_MESSAGE_DATA_LEN) {
        throw std::overflow_error("Adding event would cause overflow");
    }
    events.push_back(event);
    message_len += event->get_len();
}

network::buffer_t ServerMessage::to_bytes() const noexcept {
    network::buffer_t bytes(sizeof(game_t), '\0');
    char * data = const_cast<char *>(bytes.data());
    *reinterpret_cast<game_t *>(data) = htobe32(game_id);
    for (const auto &event: events) {
        network::buffer_t e(event->to_bytes());
        std::copy(e.begin(), e.end(), std::back_inserter(bytes));
    }
    return bytes;
}

ServerMessage::ServerMessage(const network::buffer_t &bytes) {
    game_id = be32toh(*reinterpret_cast<const game_t *>(bytes.data()));
    std::size_t off = sizeof(game_t);

    EventFactory event_factory;
    while (off < bytes.size()) {
        try {
            std::shared_ptr<Event> event = event_factory.make(bytes, off);
            add_event(event);
            off += event->get_len();
        } catch (const invalid_crc &e) {
            //std::cerr << "Invalid crc stop" << std::endl;
            break;
        } catch (const unknown_event &e) {
            std::cerr << "Unknown event" << std::endl;
            off += e.get_length();
        }
    }
}

std::vector<std::shared_ptr<Event>>::iterator ServerMessage::begin() {
    return events.begin();
}

std::vector<std::shared_ptr<Event>>::iterator ServerMessage::end() {
    return events.end();
}
