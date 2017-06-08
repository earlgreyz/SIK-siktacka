#ifndef SIK_SIKTACKA_EVENT_FACTORY_H
#define SIK_SIKTACKA_EVENT_FACTORY_H


#include <memory>
#include "event.h"

namespace siktacka {
    class invalid_crc : public std::exception {
    };

    class unknown_event : public std::exception {
    private:
        std::size_t length;
    public:
        unknown_event(std::size_t length) noexcept : length(length) {}

        std::size_t get_len() const noexcept {
            return length;
        }
    };

    class EventFactory {
    public:
        std::shared_ptr<Event>
        make(const network::buffer_t &buffer, std::size_t off) const;
    };
}


#endif
