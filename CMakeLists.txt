cmake_minimum_required(VERSION 3.7)
project(SIK_sitacka)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES )
add_executable(client ${SOURCE_FILES} client.cpp)
add_executable(server ${SOURCE_FILES} server.cpp)