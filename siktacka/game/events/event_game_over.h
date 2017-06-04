#ifndef SIK_SIKTACKA_EVENT_GAME_OVER_H
#define SIK_SIKTACKA_EVENT_GAME_OVER_H


#include "event.h"

namespace siktacka {
    class EventGameOver : public Event {
    private:
        /**
         * Serializes GAME_OVER event data.
         * @return empty buffer.
         */
        sik::buffer_t get_data() const noexcept override;
    public:
        /**
         * Constructs new GAME_OVER event.
         * @param event_no event ordinal.
         */
        EventGameOver(event_no_t event_no) noexcept;
    };
}


#endif