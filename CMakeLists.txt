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

set(SOURCE_FILES sik/poll.h error.h sik/types.h siktacka/types.h siktacka/game.cc siktacka/game.h siktacka/protocol/client/message.cc siktacka/protocol/client/message.h siktacka/protocol/server/message.cc siktacka/protocol/server/message.h siktacka/utility.h siktacka/protocol/server/event_new_game.cc siktacka/protocol/server/event_new_game.h siktacka/protocol/server/constants.h siktacka/protocol/server/event.cc siktacka/protocol/server/event.h siktacka/protocol/server/event_pixel.cc siktacka/protocol/server/event_pixel.h)
set(SERVER_SOURCE_FILES siktacka/server.cc siktacka/server.h sitacka-server.cpp)
set(CLIENT_SOURCE_FILES sitacka-client.cpp)

add_executable(client ${SOURCE_FILES} ${CLIENT_SOURCE_FILES})
add_executable(server ${SOURCE_FILES} ${SERVER_SOURCE_FILES})

TARGET_LINK_LIBRARIES(server ${Boost_LIBRARIES})