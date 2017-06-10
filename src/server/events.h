#ifndef SIK_SIKTACKA_EVENTS_H
#define SIK_SIKTACKA_EVENTS_H


#include <memory>
#include <vector>
#include <queue>
#include "../siktacka/events/event.h"

namespace sikserver {
    class Events {
    private:
        /// Stores all events in current game
        std::vector<std::shared_ptr<siktacka::Event>> events;
    public:
        /**
         * Adds new event to the list.
         * @param event event.
         */
        void add_event(std::shared_ptr<siktacka::Event> event);

        /**
         * Returns list of events with event_no greater than or equal to the
         * next_event parameter.
         * @param next_event Next expected event number.
         * @return list of events.
         */
        std::queue<std::shared_ptr<siktacka::Event>>
        get_events(siktacka::event_no_t next_event) const noexcept;

        /**
         * Clears the events list.
         */
        void clear() noexcept;
    };
}


#endif