#ifndef SIK_SIKTACKA_GUI_CLIENT_H
#define SIK_SIKTACKA_GUI_CLIENT_H


#include "../network/types.h"
#include "../siktacka/events/event.h"

namespace sikclient {
    class GuiClient {
    private:
        int sock;
        siktacka::direction_t direction;

    public:
        GuiClient(int sock, const std::string &name, network::port_t port);

        void send_event(const std::string &event);

        void receive_event();

        siktacka::direction_t get_direction() const noexcept;

    private:
        void connect_to_gui(const std::string &name, network::port_t port);

        void deselect(siktacka::direction_t deselect) noexcept;
    };
}


#endif //SIK_SIKTACKA_GUI_CLIENT_H
