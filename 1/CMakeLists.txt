include_directories(include)
include_directories(../logger/include)
include_directories(../protocol/include)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

SET(CHART_SOURCE_FILE
../client/chart.c
    )
add_executable(chart ${CHART_SOURCE_FILE})
target_link_libraries(chart protocol)
target_link_libraries(chart logger)
target_link_libraries(chart pthread)
target_link_libraries(chart ${GTK3_LIBRARIES})
target_link_libraries(chart ssl)
target_link_libraries(chart crypto)