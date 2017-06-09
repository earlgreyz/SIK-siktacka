#ifndef SIK_SOCKET_H
#define SIK_SOCKET_H


#include <exception>
#include "types.h"

namespace network {
    class socket_error : std::exception {
    };

    class bind_error : std::exception {
    };

    class Socket {
    private:
        /// Descriptor
        int socket_descriptor;
    public:
        /**
         * Creates new socket with given options.
         * @param domain socket domain e.p. AF_INET, AF_INET6 etc.
         * @param type socket type flags e.p. SOCK_STREAM, SOCK_DGRAM etc.
         * @param protocol socket protocol e.p. IPPROTO_TCP, IPPROTO_UDP etc,
         * @throws socket_error if creating socket fails.
         */
        Socket(int domain, int type, int protocol);

        /**
         * Closes the socket.
         * Prints out error message if closing socket fails.
         */
        ~Socket();

        /**
         * Returns descriptor associated with the socket.
         * @return socket descriptor.
         */
        int get_descriptor() const noexcept;

        /**
         * Binds socket to the given address.
         * @param address address structure.
         * @param address_len address structure length.
         * @throws bind_error if binding fails.
         */
        void bind_address(sockaddr *address, socklen_t address_len) const;

        /**
         * Binds socket to localhost with given port.
         * @param port port.
         * @throws bind_error if binding fails.
         */
        void bind_address(port_t port) const;

    };
}


#endif
