#include <algorithm>
#include <csignal>
#include <sys/time.h>
#include "game.h"
#include "../protocol/server/event_player_eliminated.h"
#include "../protocol/server/event_pixel.h"
#include "../protocol/server/event_game_over.h"

using namespace siktacka;


Game::Game(const siktacka::GameOptions &game_options) noexcept
        : Game(GameOptions(game_options)) {}

Game::Game(siktacka::GameOptions &&game_options) noexcept
        : game_options(game_options), frame_signal([&](int) { do_frame(); }) {
    random = std::make_unique<Random>(game_options.seed);
    board = std::make_unique<Board>(game_options.width, game_options.height);
    initialize();
}

void Game::initialize() {
    game_id = random->next();
    players_ready_count = 0u;
    for (auto &player: players) {
        player.second.ready = false;
        player.second.in_game = false;
    }
}

void Game::add_player(const std::string &name) noexcept {
    if (name.length() > MAX_PLAYER_NAME_LENGTH) {
        throw std::invalid_argument(
                "Player name cannot be longer than 64 characters");
    }

    if (players.count(name)) {
        throw std::invalid_argument("Player already in game");
    }

    players.insert(std::make_pair(name, Player()));
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
    ++players_ready_count;
    start();
}

void
Game::running_action(const Player &player, direction_t direction) noexcept {
    if (!player.in_game) {
        return;
    }
    snakes[player.player_no]->turn(direction);
}

void Game::remove_player(const std::string &name) noexcept {
    try {
        Player &player = players.at(name);
        if (!running) {
            if (player.ready) {
                --players_ready_count;
            }
            start();
        }
        players.erase(name);
    } catch (const std::out_of_range &) {
        return;
    }
}

void Game::start() noexcept {
    if (players.size() == 0 || players_ready_count < players.size()) {
        return;
    }
    running = true;

    new_game();
    request_next_frame();
}

void Game::request_next_frame() noexcept {
    if (!running) {
        return;
    }

    itimerval itm;
    itm.it_interval.tv_sec = 0;
    itm.it_value.tv_sec = 0;
    itm.it_interval.tv_usec = 1000000 / game_options.rounds_per_sec;
    itm.it_value.tv_usec = 1000000 / game_options.rounds_per_sec;

    if (setitimer(ITIMER_REAL, &itm, 0) != 0) {
        // TODO: Print error
    }
}

void Game::do_frame() noexcept {
    request_next_frame();

    if (snakes_alive_count <= 1) {
        events.push_back(std::make_unique<EventGameOver>(events.size()));
        running = false;
        initialize();
        return;
    }

    for (player_no_t i = 0; i < snakes.size(); i++) {
        Snake *snake = snakes[i].get();
        if (!snake->is_alive() || !snake->move(game_options.turning_speed)) {
            continue;
        }
        place_snake(snake, i);
    }
}

void Game::new_game() noexcept {
    player_no_t player_no = 0u;

    board->clear();
    snakes.clear();
    events.clear();

    std::unique_ptr<EventNewGame> event_new_game =
            std::make_unique<EventNewGame>(
                    game_options.width, game_options.height, 0u);

    for (auto &player: players) {
        try {
            event_new_game->add_player(player.first);
            player.second.player_no = player_no;
            player.second.in_game = true;
            std::unique_ptr<Snake> snake = make_snake();
            place_snake(snake.get(), player_no);
            snakes.push_back(std::move(snake));
            player_no++;
        } catch (const std::overflow_error &) {
            // Player name doesn't fit int the new game message skip
            continue;
        } catch (const std::invalid_argument &) {
            // Player name is not a valid name skip
            continue;
        }
    }

    snakes_alive_count = player_no - 1u;
    events.push_back(std::move(event_new_game));
}

std::unique_ptr<Snake> Game::make_snake() noexcept {
    return std::make_unique<Snake>(
            random->next() % game_options.width + .5,
            random->next() % game_options.height + .5,
            random->next() % 360
    );
}

void Game::place_snake(Snake *snake, player_no_t player_no) noexcept {
    position_t snake_position = snake->get_position();
    if (board->is_empty(snake_position)) {
        board->mark_occupied(snake_position);
        events.push_back(std::make_unique<EventPixel>(
                events.size(), player_no, snake_position
        ));
    } else {
        snakes_alive_count--;
        events.push_back(std::make_unique<EventPlayerEliminated>(
                events.size(), player_no
        ));
    }
}

