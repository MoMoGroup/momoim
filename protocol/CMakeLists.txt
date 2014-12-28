include_directories(include)
include_directories(../imcommon/include)

SET(LIBPROTOCOL_SRC
    base.c
    packets/friend/Request.c
    packets/friend/Data.c

    packets/info/Request.c
    packets/info/Data.c

    packets/login/Login.c
    packets/login/Accept.c
    packets/login/Logout.c

    packets/status/Failure.c
    packets/status/Hello.c
    packets/status/KeepAlive.c
    packets/status/OK.c
    packets/status/Crash.c

    packets/file/Data.c
    packets/file/Request.c
    packets/file/DataEnd.c
    packets/file/StoreRequest.c

    packets/message/TextMessage.c
    )
ADD_LIBRARY(protocol SHARED ${LIBPROTOCOL_SRC})
target_link_libraries(protocol imcommon)