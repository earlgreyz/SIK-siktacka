#ifndef SIK_SIKTACKA_SERVER_MESSAGE_H
#define SIK_SIKTACKA_SERVER_MESSAGE_H

#include <vector>
#include <memory>
#include "../events/event.h"
#include "../types.h"
#include "../../network/types.h"


namespace siktacka {
    /**
     * Message sent from server to client.
     */
    class ServerMessage {
    private:
        /// Current game id
        game_t game_id;
        /// Message length
        std::size_t message_len = 0u;
        /// Events sent in message
        std::vector<std::shared_ptr<Event>> events;

    public:
        /**
         * Constructs new message with no events.
         * @param game_id Current game id.
         */
        ServerMessage(game_t game_id) noexcept;

        /**
         * Constructs server message from bytes.
         * @param bytes buffer.
         */
        ServerMessage(const network::buffer_t &bytes);

        /**
         * Inserts new event to the message.
         * @throws std::overflow_error if the message would exceed maximum
         * message length.
         * @param event event to add.
         */
        void add_event(std::shared_ptr<Event> event);

        /**
         * Converts message into raw bytes
         * @return data in raw bytes format.
         */
        network::buffer_t to_bytes() const noexcept;

        /**
         * Returns game_id provided in message.
         * @return game_id.
         */
        game_t get_game_id() const noexcept;

        std::vector<std::shared_ptr<Event>>::iterator begin();

        std::vector<std::shared_ptr<Event>>::iterator end();
    };
}


#endif
