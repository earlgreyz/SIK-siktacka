#ifndef SIK_SIKTACKA_CLIENT_MESSAGE_H
#define SIK_SIKTACKA_CLIENT_MESSAGE_H

#include "types.h"
#include <string>


namespace siktacka {
    class ClientMessage {
    private:
        session_t session_id;
        turn_t turn_direction;
        event_t next_event;
        std::string player_name;

    public:
        /**
         * Constructs new message.
         * @param session_id Active session id
         * @param turn_direction Turning direction
         * @param next_event Next expected event number
         * @param player_name Player name or empty string if player is just an
         * observer and does not take active part in the game.
         * @throws std::invalid_arguments
         */
        ClientMessage(session_t session_id, turn_t turn_direction,
                      event_t next_event, const std::string &player_name);

        /**
         * Constructs new message.
         * @param session_id Active session id
         * @param turn_direction Turning direction
         * @param next_event Next expected event number
         * @param player_name Player name or empty string
         * @throws std::invalid_arguments
         */
        ClientMessage(session_t session_id, turn_t turn_direction,
                      event_t next_event, std::string &&player_name = "");

        /**
         * Constructs new client message from network formatted bytes.
         * @throws std::invalid_arguments if the bytes does not represent a
         * proper client message.
         * @param bytes client message in network bytes representation.
         * @related to_bytes
         */
        ClientMessage(const std::string &bytes);

        /**
         * Converts message to raw bytes formatted as follows:
         * - 8 bytes - session_id (session_t in network order)
         * - 1 byte - turn_direction
         * - 4 bytes - next_event (event_t in network order)
         * - up to 64 bytes - player name
         * @return network bytes message representation
         */
        std::string to_bytes() const noexcept;

    private:
        /**
         * Validates message params.
         */
        void validate();
    };
}


#endif
