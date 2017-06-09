#ifndef SIK_SIKTACKA_EVENT_NEW_GAME_H
#define SIK_SIKTACKA_EVENT_NEW_GAME_H

#include <set>
#include "event.h"


namespace siktacka {
    constexpr std::size_t EVENT_NEW_GAME_HEADER_LEN = sizeof(pixel_t) * 2;
    constexpr std::size_t MAX_PLAYER_NAME_LENGTH = 64u;

    /**
     * Event NEW_GAME.
     */
    class EventNewGame : public Event {
    private:
        /// Board width
        pixel_t max_x;
        /// Board height
        pixel_t max_y;
        /// Player names
        std::set<std::string> players;
        /// Length of player names
        std::size_t players_len = 0u;

        /**
         * @related Event::get_data()
         * @return bytes representation of the new game event data.
         */
        network::buffer_t get_data() const noexcept override;

    public:
        /**
         * Constructs new NEW_GAME event.
         * @param max_x board width.
         * @param max_y board height.
         * @param event_no event ordinal.
         */
        EventNewGame(pixel_t max_x, pixel_t max_y,
                     event_no_t event_no) noexcept;

        /**
         * Constructs new NEW_GAME event from buffer.
         * @param event_no event number.
         * @param data buffer in protocol format.
         * @param length buffer length.
         */
        EventNewGame(event_no_t event_no, const char *data, std::size_t length);

        /**
         * Adds new player to the player list.
         * @throws std::overflow_error if adding the player would exceed
         * maximum event bytes length.
         * @param player player name.
         */
        void add_player(const std::string &player);

        /**
         * Adds new player to the player list.
         * @throws std::overflow_error if adding the player would exceed
         * maximum event bytes length.
         * @param player player name.
         * @throws std::invalid_argument if buffer doesn't describe proper
         * NEW_GAME event
         */
        void add_player(std::string &&player);

        /**
         * Calculates length of event bytes representation.
         * @return length of event bytes representation.
         */
        std::size_t get_len() const noexcept override;

        /**
         * @return event max_x
         */
        pixel_t get_width() const noexcept;

        /**
         * @return event max_y.
         */
        pixel_t get_height() const noexcept;

        std::set<std::string>::const_iterator begin() const noexcept;

        std::set<std::string>::const_iterator end() const noexcept;


        /**
         * Converts event to string.
         * @return converted event.
         */
        std::string to_string(
                const std::vector<std::string> &players) const noexcept override;

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
