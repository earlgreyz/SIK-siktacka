#ifndef SIK_SIKTACKA_BUFFER_H
#define SIK_SIKTACKA_BUFFER_H


#include <netinet/in.h>
#include <queue>
#include "../network/types.h"

namespace sikserver {

    /**
     * Buffer for ready to send messages.
     */
    class Buffer {
    public:
        /**
         * Message representation.
         */
        struct Message {
            /// Message content
            network::buffer_t buffer;
            /// Message recipients
            std::queue<sockaddr_storage> addresses;

            /**
             * Constructs message with one recipient.
             */
            Message(network::buffer_t, sockaddr_storage) noexcept;

            /**
             * Constructs message with many recipients.
             */
            Message(network::buffer_t,
                    std::queue<sockaddr_storage> &&) noexcept;
        };

    private:
        /// Messages queue
        std::queue<Message> messages;
    public:
        /**
         * @return if messages queue is empty.
         */
        bool is_empty() const noexcept;

        /**
         * Adds new message to messages queue.
         */
        void add(Message) noexcept;

        /**
         * Removes message from queue.
         * @return message removed from queue.
         */
        Message pop();
    };
}


#endif
