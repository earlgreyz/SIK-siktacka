#ifndef SIK_SIKTACKA_GAME_H
#define SIK_SIKTACKA_GAME_H

#include <ctime>
#include <memory>
#include "../types.h"
#include "../random.h"

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

    class Game {
    private:
        GameOptions game_options;
        game_t game_id;

        std::unique_ptr<Random> random;

    public:
        Game(const GameOptions &game_options) noexcept;

        Game(GameOptions &&game_options) noexcept;
    };
}


#endif
