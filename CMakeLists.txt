cmake_minimum_required(VERSION 3.7)
project(SIK_siktacka)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
#set(CMAKE_CXX_COMPILER /usr/bin/clang)

find_package (Threads REQUIRED )
FIND_PACKAGE(Boost COMPONENTS system program_options filesystem REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Required Boost not found")
endif()

set(SOURCE_FILES
        error.h
        network/constants.h
        network/poll.h network/types.h
        siktacka/types.h
        network/sender.cc network/sender.h
        network/receiver.cc network/receiver.h

        siktacka/protocol/client/message.cc siktacka/protocol/client/message.h

        siktacka/protocol/server/constants.h
        siktacka/protocol/server/message.cc siktacka/protocol/server/message.h

        siktacka/game/events/event.cc siktacka/game/events/event.h
        siktacka/game/events/event_new_game.cc siktacka/game/events/event_new_game.h
        siktacka/game/events/event_pixel.cc siktacka/game/events/event_pixel.h
        siktacka/game/events/event_game_over.cc siktacka/game/events/event_game_over.h
        siktacka/game/events/event_player_eliminated.cc siktacka/game/events/event_player_eliminated.h
        siktacka/game/events/event_factory.cc siktacka/game/events/event_factory.h)

set(SERVER_SOURCE_FILES
        siktacka/server/server.cc siktacka/server/server.h
        siktacka/random.cc siktacka/random.h
        siktacka/game/game.cc siktacka/game/game.h
        siktacka/game/snake.cc siktacka/game/snake.h
        siktacka/game/board.cc siktacka/game/board.h
        siktacka/game/i_event_listener.h
        siktacka/game/events.cc siktacka/game/events.h
        siktacka/server/i_connection_listener.h
        siktacka/server/connections.cc siktacka/server/connections.h)

set(CLIENT_SOURCE_FILES
        siktacka/client/client.h siktacka/client/client.cc
        siktacka/client/gui_client.cc siktacka/client/gui_client.h)

set(TESTS_SOURCE_FILES tests/test_event.cc)

add_executable(client sitacka-client.cpp ${SOURCE_FILES} ${CLIENT_SOURCE_FILES})
add_executable(server sitacka-server.cpp ${SOURCE_FILES} ${SERVER_SOURCE_FILES})
add_executable(tests tests/tests.cc ${SOURCE_FILES} ${TESTS_SOURCE_FILES})

TARGET_LINK_LIBRARIES(client ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
TARGET_LINK_LIBRARIES(server ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
TARGET_LINK_LIBRARIES(tests ${Boost_LIBRARIES})