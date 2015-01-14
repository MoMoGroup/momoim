include_directories(include)
add_definitions ("-DSQLITE_ENABLE_UPDATE_DELETE_LIMIT")
SET(LIBCOMMON_SRC
    friends.c
    message.c
    user.c
    sqlite3.c
    )
ADD_LIBRARY(imcommon SHARED ${LIBCOMMON_SRC})