#ifndef SIK_SIKTACKA_GUI_CLIENT_H
#define SIK_SIKTACKA_GUI_CLIENT_H


#include "../network/types.h"
#include "../siktacka/events/event.h"

namespace sikclient {

    class GuiClient {
    private:
        /// GUI Server TCP socket
        int sock;
        /// Client current direction
        siktacka::direction_t direction;

    public:
        /**
         * Constructs new GUI Client.
         * @param sock GUI Server TCP socket.
         * @param host GUI Server hostname/ip.
         * @param port GUI Server port.
         */
        GuiClient(int sock, const std::string &host, network::port_t port);

        /**
         * Sends event to the GUI.
         * @param event event serialized to GUI readable format.
         */
        void send_event(const std::string &event);

        /**
         * Receives event from GUI and changes direction depending on received
         * event.
         */
        void receive_event();

        /**
         * Returns client current direction.
         * @return direction.
         */
        siktacka::direction_t get_direction() const noexcept;

    private:
        /**
         * Binds socket to the GUI Server.
         * @param host GUI Server hostname/ip.
         * @param port GUI Server port.
         */
        void connect_to_gui(const std::string &host, network::port_t port);

        /**
         * Deselects direction if matches currently selected one.
         * @param deselected direction.
         */
        void deselect(siktacka::direction_t deselected) noexcept;
    };
}


#endif //SIK_SIKTACKA_GUI_CLIENT_H
