
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

add_library(momo_media SHARED audio.c  sndwav_common.c main.c yuv422_rgb.c yuv_to_jpeg.c jpeg_to_rgb.c)
#add_executable(media client.c sndwav_common.c)

target_link_libraries(momo_media asound)
target_link_libraries(momo_media pthread)
target_link_libraries(momo_media ${GTK3_LIBRARIES})
target_link_libraries(momo_media pthread)
target_link_libraries(momo_media jpeg)