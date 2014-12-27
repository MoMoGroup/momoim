cmake_minimum_required(VERSION 2.8.4)
project(final)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c11")

add_subdirectory(logger)
add_subdirectory(imcommon)
add_subdirectory(protocol)
add_subdirectory(protocol_test)
add_subdirectory(client)
add_subdirectory(server)
add_subdirectory(server_test)
