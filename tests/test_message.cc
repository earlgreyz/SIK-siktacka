#include "catch.hpp"
#include "../siktacka/game/events/event.h"
#include "../siktacka/game/events/event_new_game.h"
#include "../siktacka/game/events/event_factory.h"
#include "../siktacka/protocol/server/message.h"

using namespace siktacka;


TEST_CASE("Server Message serializes", "[Server Message]") {
    ServerMessage message(17);
    std::shared_ptr<EventNewGame> new_game =
            std::make_shared<EventNewGame>(42, 43, 44);
    new_game->add_player("misio");
    new_game->add_player("janusz");
    message.add_event(new_game);

    network::buffer_t buffer = message.to_bytes();
    const char *data = buffer.data();
    std::size_t off = 0u;

    game_t game_id;
    game_id = be32toh(*reinterpret_cast<const game_t *>(data + off));
    off += sizeof(game_t);
    CHECK(game_id == 17);

    network::buffer_t event(data + off, buffer.size() - off);
    CHECK(event == new_game->to_bytes());

    CHECK(buffer.length() == 38);
}

TEST_CASE("Server Message from bytes constructor", "[Server Message]") {
    ServerMessage real_message(17);
    std::shared_ptr<EventNewGame> new_game =
            std::make_shared<EventNewGame>(42, 43, 44);
    new_game->add_player("M");
    new_game->add_player("W");
    real_message.add_event(new_game);

    network::buffer_t buffer = real_message.to_bytes();

    ServerMessage message(buffer);
    auto it = message.begin();
    CHECK((*it)->get_event_no() == 44);
    CHECK((*it)->get_event_type() == event_t::NEW_GAME);
    CHECK((*it)->get_len() == new_game->get_len());
}