#ifndef SIK_SIKTACKA_GAME_H
#define SIK_SIKTACKA_GAME_H

#include <ctime>
#include <memory>
#include <map>
#include "../types.h"
#include "../random.h"
#include "../protocol/server/event_new_game.h"
#include "snake.h"

namespace siktacka {
    const pixel_t SERVER_DEFAULT_WIDTH = 800u;
    const pixel_t SERVER_DEFAULT_HEIGHT = 600u;
    const rounds_t SERVER_DEFAULT_ROUNDS = 50u;
    const turn_speed_t SERVER_DEFAULT_TURN = 6u;
    const rand_t SERVER_DEFAULT_SEED = static_cast<rand_t>(time(NULL));

    struct GameOptions {
        pixel_t width = SERVER_DEFAULT_WIDTH;
        pixel_t height = SERVER_DEFAULT_HEIGHT;
        rounds_t rounds_per_sec = SERVER_DEFAULT_ROUNDS;
        turn_speed_t turning_speed = SERVER_DEFAULT_TURN;
        rand_t seed = SERVER_DEFAULT_SEED;
    };

    struct Player {
        player_no_t player_no = 0;
        bool ready = false;
    };

    class Game {
    private:
        GameOptions game_options;
        game_t game_id;
        bool running;

        std::unique_ptr<Random> random;

        std::vector<std::unique_ptr<Snake>> snakes;
        std::vector<std::unique_ptr<Event>> events;

        std::unique_ptr<EventNewGame> event_new_game;
        std::map<std::string, Player> players;
        std::uint8_t ready_players_count;

        std::set<std::string> waiting_players;

    public:
        Game(const GameOptions &game_options) noexcept;

        Game(GameOptions &&game_options) noexcept;

        void add_player(const std::string &name) noexcept;

        void remove_player(const std::string &name) noexcept;

        void initialize();

        void player_action(const std::string &name, direction_t direction);

    private:
        void add_waiting_player(const std::string &name) noexcept;

        void add_active_player(const std::string &name) noexcept;

        void starting_action(Player &player, direction_t direction) noexcept;

        void
        running_action(const Player &player, direction_t direction) noexcept;

        void run();
    };
}


#endif
