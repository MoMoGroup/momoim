include_directories(include)
include_directories(../logger/include)
include_directories(../protocol/include)
include_directories(../imcommon/include)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

SET(CLIENT_SOURCE_FILE
    client.c
    chart.c
    ClientSockfd.c
    MainInterface.c
    newuser.c
    PopupWinds.c
    MessageLoop.c
    common/util.c
    common/draw.c
    Infomation.c
    addfriend.c
    UpdataFriendList.c
    onlylookinfo.c
    changestatus.c
    chartmessage.c
    SetupWind.c
    managegroup/FriendsGroupMove.c
    managegroup/AddGroup.c
    managegroup/DeleteGroup.c
    managegroup/RenameGroup.c
    managegroup/MoveGroup.c
    ChartRecord.c
    )
add_executable(client ${CLIENT_SOURCE_FILE})
target_link_libraries(client protocol)
target_link_libraries(client logger)
target_link_libraries(client pthread)
target_link_libraries(client ${GTK3_LIBRARIES})
target_link_libraries(client ssl)
target_link_libraries(client crypto)