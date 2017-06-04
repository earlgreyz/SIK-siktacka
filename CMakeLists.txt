cmake_minimum_required(VERSION 3.7)
project(SIK_siktacka)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
#set(CMAKE_CXX_COMPILER /usr/bin/clang)

FIND_PACKAGE(Boost COMPONENTS system program_options filesystem REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Required Boost not found")
endif()

set(SOURCE_FILES
        error.h
        network/poll.h network/types.h
        siktacka/types.h system/signal.h

        siktacka/game/game.cc siktacka/game/game.h
        siktacka/game/snake.cc siktacka/game/snake.h

        siktacka/protocol/client/message.cc siktacka/protocol/client/message.h

        siktacka/protocol/server/constants.h
        siktacka/protocol/server/message.cc siktacka/protocol/server/message.h

        siktacka/game/events/event.cc siktacka/game/events/event.h
        siktacka/game/events/event_new_game.cc siktacka/game/events/event_new_game.h
        siktacka/game/events/event_pixel.cc siktacka/game/events/event_pixel.h
        siktacka/game/events/event_game_over.cc siktacka/game/events/event_game_over.h
        siktacka/game/events/event_player_eliminated.cc siktacka/game/events/event_player_eliminated.h
        network/sender.cc network/sender.h
        network/receiver.cc network/receiver.h)

set(SERVER_SOURCE_FILES
        siktacka/random.cc siktacka/random.h
        siktacka/server.cc siktacka/server.h
        siktacka/game/board.cc siktacka/game/board.h
        sitacka-server.cpp network/connections.cc
        network/connections.h siktacka/game/events.cc siktacka/game/events.h siktacka/game/i_event_listener.h network/constants.h network/i_connection_listener.h)

set(CLIENT_SOURCE_FILES sitacka-client.cpp)

add_executable(client ${SOURCE_FILES} ${CLIENT_SOURCE_FILES})
add_executable(server ${SOURCE_FILES} ${SERVER_SOURCE_FILES})

TARGET_LINK_LIBRARIES(server ${Boost_LIBRARIES})