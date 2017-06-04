#ifndef SIK_SIKTACKA_EVENTS_H
#define SIK_SIKTACKA_EVENTS_H


#include <memory>
#include <vector>
#include <queue>
#include "events/event.h"

namespace siktacka {
    class Server;

    class Events {
    private:
        std::vector<std::unique_ptr<Event>> events;
    public:
        void add_event(std::unique_ptr<Event> event);
        std::queue<Event *> get_events(event_no_t next_event) const noexcept;
        void clear() noexcept;
        std::size_t size() const noexcept;
    };
}



#endif