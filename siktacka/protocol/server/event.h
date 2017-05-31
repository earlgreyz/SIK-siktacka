#ifndef SIK_SIKTACKA_EVENT_H
#define SIK_SIKTACKA_EVENT_H

#include "../../types.h"

namespace siktacka {
    class Event {
    protected:
        event_no_t event_no;
        event_t event_type;
        virtual std::string get_data() const noexcept = 0;
    public:
        std::string to_bytes() const noexcept;
    };
}


#endif
