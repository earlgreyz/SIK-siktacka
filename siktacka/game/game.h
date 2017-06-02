#ifndef SIK_SIKTACKA_GAME_H
#define SIK_SIKTACKA_GAME_H

#include <ctime>
#include <memory>
#include <map>
#include "snake.h"
#include "board.h"
#include "../types.h"
#include "../random.h"
#include "../protocol/server/event_new_game.h"

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

        std::unique_ptr<Board> board;
        std::map<std::string, Player> players;
        std::vector<std::unique_ptr<Snake>> snakes;
        std::vector<std::unique_ptr<Event>> events;
        std::size_t snakes_alive = 0u;

        std::unique_ptr<EventNewGame> event_new_game;
        std::uint8_t ready_players_count;

        std::set<std::string> waiting_players;;

    public:
        /**
         * Constructs new game with given options.
         * @param game_options game options.
         */
        Game(const GameOptions &game_options) noexcept;

        /**
         * Constructs new game with given options.
         * @param game_options game options.
         */
        Game(GameOptions &&game_options) noexcept;

        /**
         * Adds new player to the players pool.
         * @param name new player name.
         */
        void add_player(const std::string &name) noexcept;

        /**
         * Removes player from the players pool.
         * @param name removed player name.
         */
        void remove_player(const std::string &name) noexcept;

        /**
         * Reacts to player action.
         * @param name player name.
         * @param direction action direction.
         */
        void player_action(const std::string &name, direction_t direction);

    private:
        /**
         * Initializes new game structures.
         */
        void initialize();

        /**
         * Adds player to the waiting players set.
         * @param name new player name.
         */
        void add_waiting_player(const std::string &name) noexcept;

        /**
         * Adds player to active players pool.
         * @param name new player name.
         */
        void add_active_player(const std::string &name) noexcept;

        /**
         * Reacts to player action when the game hasn't been started.
         * @param player player performing the action.
         * @param direction action direction.
         */
        void starting_action(Player &player, direction_t direction) noexcept;

        /**
         * Reacts to player action when the game is running.
         * @param player player performing the action.
         * @param direction action direction.
         */
        void
        running_action(const Player &player, direction_t direction) noexcept;

        /**
         * Starts the game round if all players are ready.
         */
        void start();

        /**
         * Performs single game loop iteration.
         */
        void request_frame();
    };
}


#endif
