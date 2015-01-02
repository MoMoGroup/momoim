include_directories(include)
include_directories(../logger/include)
include_directories(../protocol/include)
include_directories(../imcommon/include)

SET(SERVER_SOURCE_FILE
    server.c
    sqlite3.c

    run/jobs.c
    run/listener.c
    run/user.c
    run/worker.c

    data/base.c
    data/auth.c
    data/user.c
    data/file.c

    packets/file/Request.c
    packets/file/Data.c
    packets/file/DataEnd.c
    packets/file/StoreRequest.c
    packets/file/Reset.c

    packets/status/Crash.c
    packets/status/Hello.c
    packets/status/KeepAlive.c
    packets/status/OK.c
    packets/status/Failure.c

    packets/info/Request.c
    packets/info/Data.c

    packets/friend/Request.c
    packets/friend/SearchByNickname.c
    packets/friend/Add.c

    packets/login/Login.c
    packets/login/Logout.c
    packets/login/Register.c

    packets/message/Normal.c
    )
add_executable(server ${SERVER_SOURCE_FILE})
target_link_libraries(server protocol)
target_link_libraries(server logger)
target_link_libraries(server pthread)
target_link_libraries(server dl)
target_link_libraries(server ssl)
target_link_libraries(server imcommon)
target_link_libraries(server rt)