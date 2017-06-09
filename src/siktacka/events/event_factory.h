#ifndef SIK_SIKTACKA_EVENT_FACTORY_H
#define SIK_SIKTACKA_EVENT_FACTORY_H


#include <memory>
#include "event.h"

namespace siktacka {
    /**
     * Exception thrown when event CRC control sum doesnt match.
     */
    class invalid_crc : public std::exception {
    };

    /**
     * Exception thrown when event has unknown type.
     */
    class unknown_event : public std::exception {
    private:
        /// Event length
        std::size_t length;
    public:
        /**
         * Constructs new unknown_event exception.
         * @param length event length.
         */
        unknown_event(std::size_t length) noexcept : length(length) {}

        /**
         * Returns length of event who caused the exception.
         * @return event length.
         */
        std::size_t get_len() const noexcept {
            return length;
        }
    };

    /**
     * Produces events from buffer.
     */
    class EventFactory {
    public:
        /**
         * Makes event from bytes.
         * @param buffer bytes buffer.
         * @param off buffer offset.
         * @return event constructed from bytes.
         */
        std::shared_ptr<Event>
        make(const network::buffer_t &buffer, std::size_t off) const;
    };
}


#endif
