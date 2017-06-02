#include "game.h"
#include "../protocol/server/event_player_eliminated.h"
#include "../protocol/server/event_pixel.h"

using namespace siktacka;


Game::Game(const siktacka::GameOptions &game_options) noexcept
        : Game(GameOptions(game_options)) {}

Game::Game(siktacka::GameOptions &&game_options) noexcept
        : game_options(game_options) {
    random = std::make_unique<Random>(game_options.seed);
    board = std::make_unique<Board>(game_options.width, game_options.height);
    initialize();
}

void Game::initialize() {
    game_id = random->next();

    snakes.clear();
    events.clear();
    board->clear();

    snakes_alive = 0u;
    ready_players_count = 0u;

    event_new_game = std::make_unique<EventNewGame>(
            game_options.width, game_options.height, events.size());

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
    start();
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
            if (player.ready) {
                --ready_players_count;
            }
            // TODO: try adding one of the waiting users.
            start();
        }
        players.erase(name);
    } catch (const std::out_of_range &) {
        return;
    }
}

void Game::start() {
    // Check if all players are ready to start the game
    if (players.size() == 0 || ready_players_count < players.size()) {
        return;
    }
    running = true;
    events.push_back(std::move(event_new_game));

    player_no_t player_no = 0;
    for (auto &player: players) {
        player.second.player_no = player_no++;
    }

    for (player_no_t i = 0; i < players.size(); i++) {
        std::unique_ptr<Snake> snake = std::make_unique<Snake>(
                random->next() % game_options.width + .5,
                random->next() % game_options.height + .5,
                random->next() % 360
        );
        position_t snake_position = snake->get_position();
        if (board->is_empty(snake_position)) {
            snake->die();
            events.push_back(
                    std::make_unique<EventPlayerEliminated>(events.size(), i));
        } else {
            snakes_alive++;
            board->mark_occupied(snake_position);
            events.push_back(std::make_unique<EventPixel>(
                    events.size(), i,
                    snake_position.first, snake_position.second
            ));
        }
        snakes.push_back(std::move(snake));
    }

    // TODO: start main game loop
}

void Game::request_frame() {
    if (snakes_alive == 1) {

    }
    for (player_no_t i = 0; i < snakes.size(); i++) {
        Snake *snake = snakes[i].get();

        // If snake is dead or hasn't changed its position just continue
        if (!snake->is_alive() || !snake->move(game_options.turning_speed)) {
            continue;
        }

        position_t snake_position = snake->get_position();
        if (board->is_empty(snake_position)) {
            snake->die();
            events.push_back(
                    std::make_unique<EventPlayerEliminated>(events.size(), i));
        } else {
            board->mark_occupied(snake_position);
            events.push_back(std::make_unique<EventPixel>(
                    events.size(), i,
                    snake_position.first, snake_position.second
            ));
        }
    }
}
