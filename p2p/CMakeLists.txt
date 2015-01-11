include_directories(include)

SET(LIBP2P_SRC
  p2p.c
)
ADD_LIBRARY(momop2p SHARED ${LIBP2P_SRC})
target_link_libraries(momop2p pthread)
target_link_libraries(momop2p ssl)
target_link_libraries(momop2p mcrypt)