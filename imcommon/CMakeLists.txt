include_directories(include)
SET(LIBCOMMON_SRC
    friends.c
    )
ADD_LIBRARY(imcommon SHARED ${LIBCOMMON_SRC})