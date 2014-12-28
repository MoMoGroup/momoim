include_directories(include)
include_directories(../logger/include)
include_directories(../protocol/include)
include_directories(../imcommon/include)

SET(SERVER_SOURCE_FILE
    jobs.c
    listener.c
    server.c
    user.c
    worker.c
    sqlite3.c

    data/base.c
    data/auth.c
    data/user.c
    data/file.c

    packets/file/Request.c
    packets/file/Data.c
    packets/file/DataEnd.c
    packets/file/StoreRequest.c

    packets/status/Crash.c
    packets/status/Hello.c
    packets/status/KeepAlive.c
    packets/status/OK.c
    packets/status/Failure.c

    packets/info/Request.c
    packets/friend/Request.c

    packets/login/Login.c
    packets/login/Logout.c

    packets/message/TextMessage.c
    )
add_executable(server ${SERVER_SOURCE_FILE})
target_link_libraries(server protocol)
target_link_libraries(server logger)
target_link_libraries(server pthread)
target_link_libraries(server dl)
target_link_libraries(server ssl)
target_link_libraries(server imcommon)