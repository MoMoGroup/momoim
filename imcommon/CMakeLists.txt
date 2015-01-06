include_directories(include)
SET(LIBCOMMON_SRC
    friends.c
    message.c
    )
ADD_LIBRARY(imcommon SHARED ${LIBCOMMON_SRC})