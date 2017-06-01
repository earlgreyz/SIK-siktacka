#ifndef SIK_SIKTACKA_EVENT_NEW_GAME_H
#define SIK_SIKTACKA_EVENT_NEW_GAME_H

#include <vector>
#include "event.h"


namespace siktacka {
    class EventNewGame : public Event {
    private:
        pixel_t max_x;
        pixel_t max_y;
        std::vector<std::string> players;
        std::size_t players_len;

        sik::buffer_t get_data() const noexcept override;

    public:
        EventNewGame(pixel_t max_x, pixel_t max_y,
                     event_no_t event_no) noexcept;

        void add_player(const std::string &player);

        void add_player(std::string &&player);
    };
}


#endif
