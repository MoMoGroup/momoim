include_directories(include)
SET(LIBLOGGER_SOURCE_FILES logger.c)
ADD_LIBRARY(logger SHARED ${LIBLOGGER_SOURCE_FILES})