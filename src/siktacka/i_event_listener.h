#ifndef SIK_SIKTACKA_I_EVENT_LISTENER_H
#define SIK_SIKTACKA_I_EVENT_LISTENER_H

#include <memory>
#include "events/event.h"


namespace siktacka {
    class IEventListener {
    public:
        virtual void on_event(std::shared_ptr<Event> event) = 0;
    };
}


#endif
