include_directories(include)
SET(LIBCOMMON_SRC
    friends.c
    message.c
    user.c
    recursive.c
    )
ADD_LIBRARY(imcommon SHARED ${LIBCOMMON_SRC})