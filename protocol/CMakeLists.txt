include_directories(include)
SET(LIBPROTOCOL_SRC
    base.c
    packets/login/LoginFailure.c
    packets/login/Login.c
    packets/login/Logout.c
    packets/status/Hello.c
    packets/status/KeepAlive.c
    packets/status/OK.c
    packets/message/TextMessage.c
    packets/status/Crash.c
    )
ADD_LIBRARY(protocol SHARED ${LIBPROTOCOL_SRC})