#include "game.h"

#include <utility>

using namespace siktacka;

Game::Game(const siktacka::GameOptions &game_options) noexcept
        : Game(std::move(GameOptions(game_options))) {}

Game::Game(siktacka::GameOptions &&game_options) noexcept
        : game_options(std::move(game_options)) {
}
