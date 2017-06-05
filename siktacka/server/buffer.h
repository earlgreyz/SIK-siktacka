#ifndef SIK_SIKTACKA_BUFFER_H
#define SIK_SIKTACKA_BUFFER_H


#include <netinet/in.h>
#include <queue>
#include "../../network/types.h"

namespace sikserver {
    class Buffer {
    public:
        struct Message {
            network::buffer_t buffer;
            std::queue<sockaddr_in> addresses;

            Message(network::buffer_t, sockaddr_in) noexcept;

            Message(network::buffer_t, std::queue<sockaddr_in> &&) noexcept;
        };
    private:
        std::queue<Message> messages;
    public:

        bool is_empty() const noexcept;

        void add(Message) noexcept;

        Message pop();
    };
}


#endif
