#include "game.h"
#include "events/event_player_eliminated.h"
#include "events/event_pixel.h"
#include "events/event_game_over.h"
#include <algorithm>

using namespace siktacka;


Game::Game(const siktacka::GameOptions &game_options,
           IEventListener *server) noexcept
        : Game(GameOptions(game_options), server) {

}

Game::Game(siktacka::GameOptions &&game_options,
           IEventListener *server) noexcept
        : game_options(game_options), listener(server) {
    running = false;
    random = std::make_unique<Random>(game_options.seed);
    board = std::make_unique<Board>(game_options.width, game_options.height);
    initialize();
}

void Game::initialize() {
    game_id = random->next();
    players_ready_count = 0u;
    event_no = 1u;
    for (auto &player: players) {
        player.second.ready = false;
        player.second.in_game = false;
    }
}

void Game::add_player(const std::string &name) {
    if (name.length() > MAX_PLAYER_NAME_LENGTH) {
        throw std::length_error(
                "Player name cannot be longer than 64 characters");
    }

    if (players.count(name)) {
        throw std::invalid_argument("Player already in game");
    }

    players.insert(std::make_pair(name, Player()));
}

void Game::player_action(const std::string &name, direction_t direction) {
    if (players.count(name)) {
        if (running) {
            running_action(players[name], direction);
        } else {
            starting_action(players[name], direction);
        }
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
        std::cout << name << " removed from game " << std::endl;
    } catch (const std::out_of_range &) {
        return;
    }
}

void Game::start() noexcept {
    if (players.size() < 2 || players_ready_count < players.size()) {
        return;
    }
    running = true;

    new_game();
    game_thread = std::thread([&]() {
        using std::chrono::microseconds;
        using std::chrono::duration_cast;
        using std::chrono::high_resolution_clock;

        microseconds round_time(1000000 / game_options.rounds_per_sec);
        microseconds sleep_time(0);
        while (running) {
            auto start = high_resolution_clock::now();
            do_frame();
            auto end = high_resolution_clock::now();
            sleep_time += round_time;
            sleep_time -= duration_cast<microseconds>(end - start);
            if (sleep_time > microseconds(0)) {
                std::this_thread::sleep_for(sleep_time);
                sleep_time = microseconds(0);
            }
        }
        std::cout << "Game finished" << std::endl;
    });
    game_thread.detach();
}

game_t Game::get_id() const noexcept {
    return game_id;
}

void Game::do_frame() noexcept {
    if (snakes_alive_count <= 1) {
        listener->on_event(std::make_unique<EventGameOver>(event_no++));
        running = false;
        initialize();
        return;
    }

    bool did_something = false;
    for (player_no_t i = 0; i < snakes.size(); i++) {
        Snake *snake = snakes[i].get();
        if (!snake->is_alive() || !snake->move(game_options.turning_speed)) {
            continue;
        }
        did_something = true;
        listener->on_event(place_snake(snake, i));
    }

    if (!did_something) {
        std::cout << "Did nothing" << std::endl;
    }
}

void Game::new_game() noexcept {
    player_no_t player_no = 0u;
    snakes_alive_count = 0u;

    board->clear();
    snakes.clear();

    std::queue<std::unique_ptr<Event>> events;

    std::unique_ptr<EventNewGame> event_new_game =
            std::make_unique<EventNewGame>(
                    game_options.width, game_options.height, 0u);

    for (auto &player: players) {
        try {
            event_new_game->add_player(player.first);
            player.second.player_no = player_no;
            player.second.in_game = true;

            std::unique_ptr<Snake> snake = make_snake();
            snakes_alive_count++;

            events.push(place_snake(snake.get(), player_no));
            snakes.push_back(std::move(snake));

            player_no++;
        } catch (const std::overflow_error &) {
            // Player name doesn't fit in the new game message stop adding
            break;
        } catch (const std::invalid_argument &) {
            // Player name is not a valid name skip
            continue;
        }
    }

    listener->on_event(std::move(event_new_game));
    while (!events.empty()) {
        listener->on_event(std::move(events.front()));
        events.pop();
    }
}

std::unique_ptr<Snake> Game::make_snake() noexcept {
    return std::make_unique<Snake>(
            random->next() % game_options.width + .5,
            random->next() % game_options.height + .5,
            random->next() % 360
    );
}

std::unique_ptr<Event>
Game::place_snake(Snake *snake, player_no_t player_no) noexcept {
    position_t snake_position = snake->get_position();
    if (board->is_empty(snake_position)) {
        board->mark_occupied(snake_position);
        return std::make_unique<EventPixel>(
                event_no++, player_no, snake_position);
    } else {
        snake->die();
        snakes_alive_count--;
        return std::make_unique<EventPlayerEliminated>(event_no++, player_no);
    }
}

Game::~Game() {
    running = false;
}
