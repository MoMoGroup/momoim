#include_directories(include)
#include_directories(../logger/include)
#include_directories(../protocol/include)
#SET(SERVER_SOURCE_FILE
#    jobs.c
#    listener.c
#    server.c
#    user.c
#    worker.c)
#add_executable(server ${SERVER_SOURCE_FILE})
#target_link_libraries(server protocol)
#target_link_libraries(server logger)
#target_link_libraries(server pthread)