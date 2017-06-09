#ifndef SIK_SIKTACKA_EVENT_PIXEL_H
#define SIK_SIKTACKA_EVENT_PIXEL_H


#include "event.h"

namespace siktacka {
    constexpr std::size_t EVENT_PIXEL_HEADER_LEN =
            sizeof(player_no_t) + sizeof(pixel_t) * 2;

    class EventPixel : public Event {
    private:
        /// Player number
        player_no_t player_no;
        /// Position x coordinate
        pixel_t x;
        /// Position y coordinate
        pixel_t y;

        /**
         * @related Event::get_data()
         * @return bytes representation of the new game event data.
         */
        network::buffer_t get_data() const noexcept override;

    public:
        /**
         * Constructs new event PIXEL.
         * @param event_no event ordinal.
         * @param player_no player number.
         * @param position player coordinates.
         */
        EventPixel(event_no_t event_no, player_no_t player_no,
                   position_t position) noexcept;

        /**
         * Constructs new event PIXEL from bytes.
         * @param event_no event number.
         * @param data buffer in protocol format.
         * @param length buffer size.
         * @throws std::invalid_argument if buffer doesn't describe proper
         * PIXEL event
         */
        EventPixel(event_no_t event_no, const char *data,
                   std::size_t length);

        /**
         * Calculates length of event bytes representation.
         * @return length of event bytes representation.
         */
        std::size_t get_len() const noexcept override;

        /**
         * Converts event to string.
         * @return converted event.
         */
        std::string to_string(
                const std::vector<std::string> &players) const override;

        /**
         * Validates if event is proper event in game with parameters:
         * @param width board width.
         * @param height board height.
         * @param players_count player names.
         * @throws std::invalid_argument if event has invalid data.
         */
        void validate_in_game(pixel_t width, pixel_t height,
                              std::size_t players_count) const override;

    };
}

#endif
