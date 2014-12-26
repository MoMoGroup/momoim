include_directories(include)
include_directories(../logger/include)
include_directories(../protocol/include)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

SET(CLIENT_SOURCE_FILE
    client.c
    )
add_executable(client ${CLIENT_SOURCE_FILE})
target_link_libraries(client protocol)
target_link_libraries(client logger)
target_link_libraries(client pthread)
target_link_libraries(client ${GTK3_LIBRARIES})