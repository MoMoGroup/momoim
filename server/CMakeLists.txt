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
    run/natTraversal.c
    run/gc.c

    datafile/init.c
    datafile/auth.c
    datafile/user.c
    datafile/storage.c
    datafile/friends.c
    datafile/message.c

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
    packets/status/Cancel.c
    packets/status/SwitchProtocol.c

    packets/info/Request.c
    packets/info/Data.c

    packets/friend/Request.c
    packets/friend/SearchByNickname.c
    packets/friend/Add.c
    packets/friend/Accept.c
    packets/friend/Move.c
    packets/friend/Delete.c
    packets/friend/GroupAdd.c
    packets/friend/GroupRename.c
    packets/friend/GroupMove.c
    packets/friend/GroupDelete.c

    packets/login/Login.c
    packets/login/Logout.c
    packets/login/Register.c

    packets/message/Normal.c
    packets/message/QueryOffline.c

    packets/net/Discover.c
    )
add_executable(server ${SERVER_SOURCE_FILE})
target_link_libraries(server protocol)
target_link_libraries(server logger)
target_link_libraries(server pthread)
target_link_libraries(server dl)
target_link_libraries(server ssl)
target_link_libraries(server imcommon)
target_link_libraries(server rt)
target_link_libraries(server crypto)