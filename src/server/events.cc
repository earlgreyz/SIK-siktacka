#include "events.h"

using namespace sikserver;

void Events::add_event(std::shared_ptr<siktacka::Event> event) {
    events.push_back(event);
}

std::queue<std::shared_ptr<siktacka::Event>>
Events::get_events(siktacka::event_no_t next_event) const noexcept {
    std::queue<std::shared_ptr<siktacka::Event>> message_events;
    // TODO: binsearch
    for (std::size_t i = 0; i < events.size(); i++) {
        if (events[i]->get_event_no() >= next_event) {
            message_events.push(events[i]);
        }
    }
    return message_events;
}

void Events::clear() noexcept {
    events.clear();
}
