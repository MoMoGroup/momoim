include_directories(include)
SET(LIBPROTOCOL_SRC
    base.c
    packets/login/Login.c
    packets/login/Accept.c
    packets/login/Logout.c

    packets/status/Failure.c
    packets/status/Hello.c
    packets/status/KeepAlive.c
    packets/status/OK.c
    packets/status/Crash.c

    packets/message/TextMessage.c
    )
ADD_LIBRARY(protocol SHARED ${LIBPROTOCOL_SRC})