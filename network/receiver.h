#ifndef SIK_SIKTACKA_RECEIVER_H
#define SIK_SIKTACKA_RECEIVER_H

#include <string>
#include <netinet/in.h>
#include "types.h"


namespace network {
    /**
     * Class for receiving messages from socket.
     */
    class Receiver {
    private:
        /// Socket to receive data from
        int sock;
    public:
        /**
         * Creates new receiver.
         * @param sock socket to send data to.
         */
        Receiver(int sock) noexcept;

        /**
         * Receives message from socket.
         * @param address sender address.
         * @return received message
         * @throws runtime_error if recvfrom finishes with error
         */
        buffer_t receive_message(const sockaddr_in &address);
    };
}

#endif //SIK_UDP_SENDER_H