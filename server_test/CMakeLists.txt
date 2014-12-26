include_directories(../logger/include)
include_directories(../protocol/include)
SET(SERVER_TEST_SOURCE_FILE
    main.c
    )
add_executable(server_test ${SERVER_TEST_SOURCE_FILE})
target_link_libraries(server_test protocol)
target_link_libraries(server_test logger)
target_link_libraries(server_test pthread)
target_link_libraries(server_test ssl)
target_link_libraries(server_test crypto)
