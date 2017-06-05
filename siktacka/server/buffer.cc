#include <stdexcept>
#include "buffer.h"

using namespace sikserver;

Buffer::Message::Message(network::buffer_t buffer, sockaddr_in address) noexcept
        : buffer(buffer) {
    addresses.push(address);
}

Buffer::Message::Message(network::buffer_t buffer,
                         std::queue<sockaddr_in> &&addresses) noexcept
        : buffer(buffer), addresses(addresses) {

}


bool Buffer::is_empty() const noexcept {
    return messages.empty();
}

void Buffer::add(Buffer::Message message) noexcept {
    messages.push(message);
}

Buffer::Message Buffer::pop() {
    if (messages.empty()) {
        throw std::out_of_range("Buffer is empty!");
    }
    Buffer::Message message = messages.front();
    messages.pop();
    return message;
}
