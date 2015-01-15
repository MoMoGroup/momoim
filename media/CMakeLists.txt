add_library(momo_media SHARED client.c  sndwav_common.c )
add_library(momo_video SHARED main.c yuv422_rgb.c yuv_to_jpeg.c jpeg_to_rgb.c)
#add_executable(media client.c sndwav_common.c)

target_link_libraries(momo_media asound)
target_link_libraries(momo_media pthread)
target_link_libraries(momo_video ${GTK3_LIBRARIES})
target_link_libraries(momo_video pthread)
target_link_libraries(momo_video jpeg)