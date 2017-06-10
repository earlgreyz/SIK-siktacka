#include <iostream>
#include <memory>
#include <sys/socket.h>
#include <netinet/in.h>
#include <boost/lexical_cast.hpp>
#include "../src/siktacka/events/event.h"
#include "../src/network/socket.h"
#include "../src/network/receiver.h"
#include "../src/network/sender.h"
#include "../src/siktacka/events/event_new_game.h"
#include "../src/siktacka/events/event_pixel.h"
#include "../src/siktacka/protocol/server_message.h"
#include "../src/siktacka/events/event_player_eliminated.h"

sockaddr_storage previous;
sockaddr_storage client;
std::unique_ptr<network::Sender> sender;
std::unique_ptr<network::Receiver> receiver;


const siktacka::pixel_t width = 200;
const siktacka::pixel_t height = 100;

void wait_for_client() {
    memset(&client, 0, sizeof(sockaddr_storage));
    (void) receiver->receive_message(&client);
    if (memcmp(&previous, &client, sizeof(sockaddr_storage)) == 0) {
        wait_for_client();
        return;
    }
    memcpy(&previous, &client, sizeof(sockaddr_storage));
    std::cout << "Connected client " << std::endl;
}

void new_game() {
    std::shared_ptr<siktacka::EventNewGame> new_game =
            std::make_shared<siktacka::EventNewGame>(width, height, 0);

    new_game->add_player("testowy");
    new_game->add_player("testowy2");

    siktacka::ServerMessage new_game_message(1);
    new_game_message.add_event(new_game);

    sender->send_message(&client, new_game_message.to_bytes());
}

void test_pixel_w() {
    wait_for_client();
    new_game();
    siktacka::position_t position =
            std::make_pair<siktacka::pixel_t, siktacka::pixel_t>(width + 0, 0);
    std::shared_ptr<siktacka::EventPixel> pixel =
            std::make_shared<siktacka::EventPixel>(1, 0, position);
    siktacka::ServerMessage pixel_message(1);
    pixel_message.add_event(pixel);
    sender->send_message(&client, pixel_message.to_bytes());
}

void test_pixel_h() {
    wait_for_client();
    new_game();
    siktacka::position_t position =
            std::make_pair<siktacka::pixel_t, siktacka::pixel_t>(0, height+ 0);
    std::shared_ptr<siktacka::EventPixel> pixel =
            std::make_shared<siktacka::EventPixel>(1, 0, position);
    siktacka::ServerMessage pixel_message(1);
    pixel_message.add_event(pixel);
    sender->send_message(&client, pixel_message.to_bytes());
}

void test_pixel_p() {
    wait_for_client();
    new_game();
    siktacka::position_t position =
            std::make_pair<siktacka::pixel_t, siktacka::pixel_t>(0, 0);
    std::shared_ptr<siktacka::EventPixel> pixel =
            std::make_shared<siktacka::EventPixel>(1, 10, position);
    siktacka::ServerMessage pixel_message(1);
    pixel_message.add_event(pixel);
    sender->send_message(&client, pixel_message.to_bytes());
}

void test_player_eliminated_p() {
    wait_for_client();
    new_game();
    std::shared_ptr<siktacka::EventPlayerEliminated> player_eliminated =
            std::make_shared<siktacka::EventPlayerEliminated>(1, 10);
    siktacka::ServerMessage player_eliminated_message(1);
    player_eliminated_message.add_event(player_eliminated);
    sender->send_message(&client, player_eliminated_message.to_bytes());
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " port" << std::endl;
    }
    network::port_t port = boost::lexical_cast<network::port_t>(argv[1]);

    memset(&previous, 0, sizeof(sockaddr_storage));

    network::Socket sock(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
    sock.bind_address(port);

    sender = std::make_unique<network::Sender>(sock.get_descriptor());
    receiver = std::make_unique<network::Receiver>(sock.get_descriptor());

    // PIXEL_W
    std::cout << "PIXEL with x outside map boundries" << std::endl;
    test_pixel_w();

    // PIXEL_H
    std::cout << "PIXEL with y outside map boundries" << std::endl;
    test_pixel_w();

    // PIXEL_P
    std::cout << "PIXEL with non existent player id" << std::endl;
    test_pixel_p();

    // PLAYER_ELIMINATED_P
    std::cout << "PLAYER_ELIMINATED with non existent player id" << std::endl;
    test_player_eliminated_p();
}