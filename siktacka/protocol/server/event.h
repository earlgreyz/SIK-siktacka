#ifndef SIK_SIKTACKA_EVENT_H
#define SIK_SIKTACKA_EVENT_H

#include "constants.h"
#include "../../types.h"
#include "../../../sik/types.h"


namespace siktacka {
    constexpr std::size_t EVENT_HEADER_LEN =
            sizeof(event_len_t) + sizeof(event_no_t) + sizeof(event_t) +
            sizeof(crc32_t);
    constexpr std::size_t MAX_EVENT_DATA_LEN =
            MAX_MESSAGE_DATA_LEN - EVENT_HEADER_LEN;

    /**
     * Base event type.
     */
    class Event {
    protected:
        /// Event ordinal
        const event_no_t event_no;
        /// Event type
        const event_t event_type;

        /**
         * Converts event_data to network ready byte format.
         * @return event_data bytes.
         */
        virtual sik::buffer_t get_data() const noexcept = 0;

    public:
        /**
         * Constructs new event.
         * @param event_no event ordinal.
         * @param event_type event type.
         */
        Event(event_no_t event_no, event_t event_type);

        /**
         * Serializes event to network ready byte format.
         * @return event bytes representation.
         */
        sik::buffer_t to_bytes() const noexcept;

        /**
         * @return length of event bytes.
         */
        virtual std::size_t get_len() const noexcept;
    };
}


#endif
