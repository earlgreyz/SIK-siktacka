#ifndef SIK_SIKTACKA_EVENT_H
#define SIK_SIKTACKA_EVENT_H

#include "constants.h"
#include "../../types.h"
#include "../../../sik/types.h"


namespace siktacka {
    class Event {
    protected:
        const event_no_t event_no;
        const event_t event_type;

        virtual sik::buffer_t get_data() const noexcept = 0;

    public:
        Event(event_no_t event_no, event_t event_type);

        sik::buffer_t to_bytes() const noexcept;

        virtual std::size_t get_len() const noexcept;
    };
}


#endif
