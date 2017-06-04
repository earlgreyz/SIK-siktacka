#ifndef SIK_SIKTACKA_SERVER_MESSAGE_H
#define SIK_SIKTACKA_SERVER_MESSAGE_H

#include <vector>
#include <memory>
#include "../../game/events/event.h"
#include "../../types.h"
#include "../../../sik/types.h"


namespace siktacka {
    /**
     * Message sent from server to client.
     */
    class ServerMessage {
    private:
        /// Current game id
        game_t game_id;
        /// Message length
        std::size_t message_len;
        /// Events sent in message
        std::vector<Event *> events;

    public:
        /**
         * Constructs new message with no events.
         * @param game_id Current game id.
         */
        ServerMessage(game_t game_id) noexcept;

        /**
         * Inserts new event to the message.
         * @throws std::overflow_error if the message would exceed maximum
         * message length.
         * @param event event to add.
         */
        void add_event(Event *event);

        /**
         * Converts message into raw bytes
         * @return data in raw bytes format.
         */
        sik::buffer_t to_bytes() const noexcept;

        using iterator = std::vector<Event *>::iterator;

        /**
        * @return iterator to the first element of events array.
        */
        iterator begin() {
            return events.begin();
        }

        /**
         * @return iterator to the last element of events array.
         */
        iterator end() {
            return events.end();
        }
    };
}


#endif
