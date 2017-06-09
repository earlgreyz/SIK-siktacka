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
        network::buffer_t get_data() const noexcept override;

    public:
        /**
         * Constructs new GAME_OVER event.
         * @param event_no event ordinal.
         */
        EventGameOver(event_no_t event_no) noexcept;

        /**
         * Constructs new GAME_OVER events from bytes.
         * @param event_no event number.
         * @param length size of buffer.
         * @throws std::invalid_argument if buffer doesn't describe proper
         * GAME_OVER event
         */
        EventGameOver(event_no_t event_no, std::size_t length);

        /**
         * Converts event to string.
         * @return converted event.
         */
        std::string to_string(
                const std::vector<std::string> &players) const noexcept override;
    };
}

#endif