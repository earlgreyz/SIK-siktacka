#include <boost/crc.hpp>
#include "catch.hpp"
#include "../siktacka/game/events/event.h"
#include "../siktacka/game/events/event_new_game.h"
#include "../siktacka/game/events/event_game_over.h"
#include "../siktacka/game/events/event_pixel.h"
#include "../siktacka/game/events/event_player_eliminated.h"
#include "../siktacka/game/events/event_factory.h"

using namespace siktacka;

namespace {
    crc32_t get_crc(const char * data, std::size_t size) noexcept {
        boost::crc_32_type result;
        result.process_bytes(data, size);
        return static_cast<crc32_t>(result.checksum());
    }
}


TEST_CASE("Event new game serializes", "[Event NEW_GAME]") {
    std::unique_ptr<EventNewGame> event =
            std::make_unique<EventNewGame>(142, 342, 42);
    event->add_player("M");
    event->add_player("W");
    network::buffer_t buffer = event->to_bytes();
    const char * data = buffer.data();

    std::size_t expected_len = EVENT_HEADER_LEN + EVENT_NEW_GAME_HEADER_LEN + 4;
    CHECK(buffer.size() == expected_len);

    event_len_t event_len;
    std::size_t off = 0;

    event_len = be32toh(*reinterpret_cast<const event_len_t *>(data + off));
    off += sizeof(event_len_t);
    CHECK(event_len == sizeof(event_no_t) + sizeof(event_t) +  2 * sizeof(pixel_t) + 4);

    event_no_t event_no;
    event_no = be32toh(*reinterpret_cast<const event_no_t *>(data + off));
    off += sizeof(event_no_t);
    CHECK(event_no == 42);

    event_t event_type;
    event_type = *reinterpret_cast<const event_t *>(data + off);
    off += sizeof(event_t);
    CHECK(event_type == event_t::NEW_GAME);

    pixel_t max_x;
    max_x = be32toh(*reinterpret_cast<const pixel_t *>(data + off));
    off += sizeof(pixel_t);
    CHECK(max_x == 142);

    pixel_t max_y;
    max_y = be32toh(*reinterpret_cast<const pixel_t *>(data + off));
    off += sizeof(pixel_t);
    CHECK(max_y == 342);

    char M[2];
    M[0] = *(data + off);
    M[1] = *(data + off + 1);
    off += 2;
    CHECK(M[0] == 'M');
    CHECK(M[1] == '\0');

    char W[2];
    W[0] = *(data + off);
    W[1] = *(data + off + 1);
    off += 2;
    CHECK((W[0] == 'W' && W[1] == '\0'));

    crc32_t crc;
    crc = be32toh(*reinterpret_cast<const crc32_t *>(data + off));
    REQUIRE(crc == get_crc(data, expected_len - sizeof(crc32_t)));
 }

TEST_CASE("Event factory parses NEW GAME", "[Event NEW_GAME]") {
    std::unique_ptr<EventNewGame> new_game =
            std::make_unique<EventNewGame>(142, 342, 42);
    new_game->add_player("M");
    new_game->add_player("W");
    network::buffer_t buffer = new_game->to_bytes();

    EventFactory event_factory;
    std::shared_ptr<Event> event = event_factory.make(buffer, 0);
    CHECK(event->get_event_no() == 42);
    CHECK(event->get_event_type() == event_t::NEW_GAME);
    CHECK(event->get_len() == buffer.length());
}