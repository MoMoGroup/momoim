#include <stdlib.h>
#include <stdio.h>
#include<stdint.h>
#include <jpeglib.h>
#include <string.h>

unsigned long jpegWrite(unsigned char *img, unsigned char *dst) {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    unsigned long outsize = 640 * 480 * 4;
//char name[4]={'a','b','c','x'};
// try to open file for saving
// create jpeg data
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_mem_dest(&cinfo, &dst, &outsize);
// set image parameters
    cinfo.image_width = 640;
    cinfo.image_height = 480;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
// set jpeg compression parameters to default
    jpeg_set_defaults(&cinfo);
// and then adjust quality setting
    jpeg_set_quality(&cinfo, 50, TRUE);
// start compress
    jpeg_start_compress(&cinfo, TRUE);
// feed data
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &img[cinfo.next_scanline * cinfo.image_width *
                cinfo.input_components];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
// finish compression
    jpeg_finish_compress(&cinfo);
// destroy jpeg data
    jpeg_destroy_compress(&cinfo);
// close output file
    return outsize;
}
