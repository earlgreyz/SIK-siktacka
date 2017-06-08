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
        network/poll.h
        network/types.h
        siktacka/types.h
        network/sender.cc network/sender.h
        network/receiver.cc network/receiver.h

        siktacka/protocol/client/message.cc siktacka/protocol/client/message.h

        siktacka/protocol/server/constants.h
        siktacka/protocol/server/message.cc siktacka/protocol/server/message.h

        siktacka/events/event.cc siktacka/events/event.h
        siktacka/events/event_new_game.cc siktacka/events/event_new_game.h
        siktacka/events/event_pixel.cc siktacka/events/event_pixel.h
        siktacka/events/event_game_over.cc siktacka/events/event_game_over.h
        siktacka/events/event_player_eliminated.cc siktacka/events/event_player_eliminated.h
        siktacka/events/event_factory.cc siktacka/events/event_factory.h)

set(SERVER_SOURCE_FILES
        server/server.cc server/server.h
        siktacka/random.cc siktacka/random.h
        siktacka/game.cc siktacka/game.h
        siktacka/snake.cc siktacka/snake.h
        siktacka/board.cc siktacka/board.h
        siktacka/i_event_listener.h
        server/events.cc server/events.h
        server/i_connection_listener.h
        server/connections.cc server/connections.h
        server/buffer.cc server/buffer.h network/socket.cc network/socket.h)

set(CLIENT_SOURCE_FILES
        client/client.h client/client.cc
        client/gui_client.cc client/gui_client.h
        client/parser.cc client/parser.h)

set(TESTS_SOURCE_FILES tests/test_event.cc tests/test_message.cc)

add_executable(client sitacka-client.cpp ${SOURCE_FILES} ${CLIENT_SOURCE_FILES})
add_executable(server sitacka-server.cpp ${SOURCE_FILES} ${SERVER_SOURCE_FILES})
add_executable(tests tests/tests.cc ${SOURCE_FILES} ${TESTS_SOURCE_FILES})

TARGET_LINK_LIBRARIES(client ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
TARGET_LINK_LIBRARIES(server ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
TARGET_LINK_LIBRARIES(tests ${Boost_LIBRARIES})