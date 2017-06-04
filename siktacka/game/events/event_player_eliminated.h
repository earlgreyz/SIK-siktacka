#ifndef SIK_SIKTACKA_EVENT_PLAYER_ELIMINATED_H
#define SIK_SIKTACKA_EVENT_PLAYER_ELIMINATED_H


#include "event.h"

namespace siktacka {
    constexpr std::size_t EVENT_PLAYER_ELIMINATED_HEADER_LEN =
            sizeof(player_no_t);

    class EventPlayerEliminated : public Event {
    private:
        player_no_t player_no;

        /**
         * Serializes GAME_OVER event data.
         * @return empty buffer.
         */
        network::buffer_t get_data() const noexcept override;

    public:
        /**
         * Constructs new PLAYER_ELIMINATED event.
         * @param event_no event ordinal.
         * @param player_no eliminated player number.
         */
        EventPlayerEliminated(event_no_t event_no,
                              player_no_t player_no) noexcept;

        EventPlayerEliminated(event_no_t event_no, const char *data,
                              std::size_t length);

        std::size_t get_len() const noexcept override;

        /**
         * Converts event to string.
         * @return converted event.
         */
        std::string to_string(
                const std::vector<std::string> &players) const override;
    };
}

#endif
