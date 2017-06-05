#ifndef SIK_SIKTACKA_GAME_H
#define SIK_SIKTACKA_GAME_H

#include <ctime>
#include <memory>
#include <map>
#include <thread>
#include "../types.h"
#include "../random.h"
#include "snake.h"
#include "board.h"
#include "events.h"
#include "events/event_new_game.h"
#include "i_event_listener.h"

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
        bool in_game = false;
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
        std::uint8_t players_ready_count;
        std::vector<std::unique_ptr<Snake>> snakes;
        std::size_t snakes_alive_count;
        IEventListener *listener;
        event_no_t event_no;

        std::thread game_thread;

    public:
        /**
         * Constructs new game with given options.
         * @param game_options game options.
         */
        Game(const GameOptions &game_options, IEventListener *server) noexcept;

        /**
         * Constructs new game with given options.
         * @param game_options game options.
         */
        Game(GameOptions &&game_options, IEventListener *server) noexcept;

        ~Game();

        /**
         * Adds new player to the players pool.
         * @param name new player name.
         */
        void add_player(const std::string &name);

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

        /**
         * Returns current game id.
         * @return game id.
         */
        game_t get_id() const noexcept;

    private:
        /**
         * Initializes new game structures.
         */
        void initialize();

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
        void start() noexcept;

        /**
         * Performs single game loop iteration.
         */
        void do_frame() noexcept;

        /**
         * Starts new game.
         */
        void new_game() noexcept;

        /**
         * Creates new snake at random position with random rotation.
         * @return new snake.
         */
        std::unique_ptr<Snake> make_snake() noexcept;

        /**
         * Places snake pixel on the board.
         * @param snake snake to place.
         * @param player_no snake owner.
         */
        std::unique_ptr<Event>
        place_snake(Snake *snake, player_no_t player_no) noexcept;
    };
}


#endif
