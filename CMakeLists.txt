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
        src/error.h
        src/network/constants.h
        src/network/poll.h
        src/network/types.h
        src/siktacka/types.h
        src/network/sender.cc src/network/sender.h
        src/network/receiver.cc src/network/receiver.h
        src/network/socket.cc src/network/socket.h

        src/siktacka/protocol/client_message.cc src/siktacka/protocol/client_message.h

        src/siktacka/protocol/constants.h
        src/siktacka/protocol/server_message.cc src/siktacka/protocol/server_message.h

        src/siktacka/events/event.cc src/siktacka/events/event.h
        src/siktacka/events/event_new_game.cc src/siktacka/events/event_new_game.h
        src/siktacka/events/event_pixel.cc src/siktacka/events/event_pixel.h
        src/siktacka/events/event_game_over.cc src/siktacka/events/event_game_over.h
        src/siktacka/events/event_player_eliminated.cc src/siktacka/events/event_player_eliminated.h
        src/siktacka/events/event_factory.cc src/siktacka/events/event_factory.h)

set(SERVER_SOURCE_FILES
        src/server/server.cc src/server/server.h
        src/siktacka/random.cc src/siktacka/random.h
        src/siktacka/game.cc src/siktacka/game.h
        src/siktacka/snake.cc src/siktacka/snake.h
        src/siktacka/board.cc src/siktacka/board.h
        src/siktacka/i_event_listener.h
        src/server/events.cc src/server/events.h
        src/server/i_connection_listener.h
        src/server/connections.cc src/server/connections.h
        src/server/buffer.cc src/server/buffer.h)

set(CLIENT_SOURCE_FILES
        src/client/client.h src/client/client.cc
        src/client/gui_client.cc src/client/gui_client.h
        src/client/parser.cc src/client/parser.h)

set(TESTS_SOURCE_FILES tests/test_event.cc tests/test_message.cc)

add_executable(client src/siktacka-client.cc ${SOURCE_FILES} ${CLIENT_SOURCE_FILES})
add_executable(server src/siktacka-server.cc ${SOURCE_FILES} ${SERVER_SOURCE_FILES})
add_executable(tests tests/tests.cc ${SOURCE_FILES} ${TESTS_SOURCE_FILES})
add_executable(malicious_server tests/malicious_server.cc ${SOURCE_FILES})

TARGET_LINK_LIBRARIES(client ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
TARGET_LINK_LIBRARIES(server ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
TARGET_LINK_LIBRARIES(tests ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(malicious_server ${Boost_LIBRARIES})