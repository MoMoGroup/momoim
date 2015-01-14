include_directories(include)
SET(LIBCOMMON_SRC
    friends.c
    message.c
    user.c
    sqlite3.c
    )
ADD_LIBRARY(imcommon SHARED ${LIBCOMMON_SRC})