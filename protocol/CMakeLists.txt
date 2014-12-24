include_directories(include)
SET(LIBPROTOCOL_SRC
    base.c
    packets/login/LoginFailure.c
    packets/login/Login.c
    )
ADD_LIBRARY(protocol SHARED ${LIBPROTOCOL_SRC})