#ifndef SIK_SIKTACKA_SENDER_H
#define SIK_SIKTACKA_SENDER_H


#include <exception>
#include <netinet/in.h>
#include <cerrno>
#include "types.h"

namespace network {
    /**
     * Exception thrown when function finishes with EWOULDBLOCK errno.
     */
    class WouldBlockException : public std::exception {
    };

    /**
     * Class for sending messages over socket.
     */
    class Sender {
    private:
        /// Socket to send data to.
        int sock;
    public:
        /**
         * Creates new sender.
         * @param sock UDP Socket.
         */
        Sender(int sock) noexcept : sock(sock) {}

        /**
         * Sends message to given address.
         * @param address receiver address.
         * @param buffer message data to send.
         * @param with_message send message with content.
         * @throws WouldBlockException if sendto finishes with errno EWOULDBLOCK
         * @throws ConnectionException when sendto finishes with error
         */
        void
        send_message(sockaddr_storage *address,
                     const buffer_t &buffer) const;
    };
}


#endif //SIK_SIKTACKA_SENDER_H
