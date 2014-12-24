include_directories(include)
SET(LIBPROTOCOL_SRC
    base.c
    packets/Login/LoginFailure.c
    packets/Login/Login.c
    )
ADD_LIBRARY(protocol SHARED ${LIBPROTOCOL_SRC})