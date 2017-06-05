#ifndef SIK_SIKTACKA_I_CONNECTION_LISTENER_H
#define SIK_SIKTACKA_I_CONNECTION_LISTENER_H

#include <string>

namespace sikserver {
    class IConnectionListener {
    public:
        virtual void on_disconnect(const std::string &name) = 0;
    };
}


#endif
