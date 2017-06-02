#include "game.h"

#include <utility>

using namespace siktacka;

Game::Game(const siktacka::GameOptions &game_options) noexcept
        : Game(GameOptions(game_options)) {}

Game::Game(siktacka::GameOptions &&game_options) noexcept
        : game_options(game_options) {
    random = std::make_unique<Random>(game_options.seed);
}

void Game::initialize() {
    game_id = random->next();
    event_new_game = std::make_unique<EventNewGame>(
            game_options.width, game_options.height, game_id);
    ready_players_count = 0u;
}

void Game::add_player(const std::string &name) noexcept {
    if (players.count(name) || waiting_players.count(name)) {
        throw std::invalid_argument("Player already in game");
    }

    if (!running) {
        add_active_player(name);
    } else {
        add_waiting_player(name);
    }
}

void Game::add_waiting_player(const std::string &name) noexcept {
    waiting_players.insert(name);
}

void Game::add_active_player(const std::string &name) noexcept {
    try {
        event_new_game->add_player(name);
        players.insert(std::make_pair(name, Player()));
    } catch (const std::overflow_error &) {
        add_waiting_player(name);
    } catch (const std::invalid_argument &) {
        return;
    }
}

void Game::player_action(const std::string &name, direction_t direction) {
    try {
        Player &player = players.at(name);
        if (running) {
            running_action(player, direction);
        } else {
            starting_action(player, direction);
        }
    } catch (const std::out_of_range &) {
        return;
    }
}

void Game::starting_action(Player &player, direction_t direction) noexcept {
    if (player.ready || direction == direction_t::NONE) {
        return;
    }
    player.ready = true;
    ++ready_players_count;
    run();
}

void
Game::running_action(const Player &player, direction_t direction) noexcept {
    snakes[player.player_no]->turn(direction);
}

void Game::remove_player(const std::string &name) noexcept {
    // If the player is waiting just remove him
    if (waiting_players.count(name)) {
        waiting_players.erase(name);
        return;
    }

    try {
        Player &player = players.at(name);
        if (!running) {
            // If player was ready decrease
            if (player.ready) {
                --ready_players_count;
            } else {
                run();
            }
        }
        players.erase(name);
    } catch (const std::out_of_range &) {
        return;
    }
}

void Game::run() {
    // Check if all players are ready to start the game
    if (players.size() == 0 || ready_players_count < players.size()) {
        return;
    }
    running = true;
    events.push_back(std::move(event_new_game));
}
