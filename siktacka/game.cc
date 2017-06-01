#include "game.h"

#include <utility>

using namespace siktacka;

Game::Game(const siktacka::GameOptions &game_options) noexcept
        : Game(GameOptions(game_options)) {}

Game::Game(siktacka::GameOptions &&game_options) noexcept
        : game_options(game_options) {
}
