cmake_minimum_required(VERSION 3.7)
project(SIK_sitacka)

set(CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

FIND_PACKAGE(Boost COMPONENTS system program_options filesystem REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Required Boost not found")
endif()

set(SOURCE_FILES sik/poll.h error.h sik/types.h siktacka/types.h)
set(SERVER_SOURCE_FILES siktacka/server.cc siktacka/server.h sitacka-server.cpp)
set(CLIENT_SOURCE_FILES sitacka-client.cpp)

add_executable(client ${SOURCE_FILES} ${CLIENT_SOURCE_FILES})
add_executable(server ${SOURCE_FILES} ${SERVER_SOURCE_FILES})

TARGET_LINK_LIBRARIES(server ${Boost_LIBRARIES})