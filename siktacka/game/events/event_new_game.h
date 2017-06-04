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
        std::size_t players_len;

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
         */
        void add_player(std::string &&player);

        /**
         * Calculates length of event bytes representation.
         * @return length of event bytes representation.
         */
        std::size_t get_len() const noexcept override;
    };
}


#endif
