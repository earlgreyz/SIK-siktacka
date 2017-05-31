#ifndef SIK_SIKTACKA_GAME_H
#define SIK_SIKTACKA_GAME_H

#include <ctime>
#include "types.h"

namespace siktacka {
    const pixel_t SERVER_DEFAULT_WIDTH = 800u;
    const pixel_t SERVER_DEFAULT_HEIGHT = 600u;
    const rounds_t SERVER_DEFAULT_ROUNDS = 50u;
    const turn_t SERVER_DEFAULT_TURN = 6u;
    const seed_t SERVER_DEFAULT_SEED = time(NULL);

    struct GameOptions {
        pixel_t width = SERVER_DEFAULT_WIDTH;
        pixel_t height = SERVER_DEFAULT_HEIGHT;
        rounds_t rounds_per_sec = SERVER_DEFAULT_ROUNDS;
        turn_t turning_speed = SERVER_DEFAULT_TURN;
        seed_t seed = SERVER_DEFAULT_SEED;
    };

    class Game {
    private:
        GameOptions game_options;

    public:
        Game(const GameOptions &game_options) noexcept;

        Game(GameOptions &&game_options) noexcept;
    };
}


#endif