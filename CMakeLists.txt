cmake_minimum_required(VERSION 2.8.4)
project(final)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

add_subdirectory(client)
add_subdirectory(protocol)
add_subdirectory(protocol_test)
add_subdirectory(logger)
add_subdirectory(server)
add_subdirectory(server_test)
