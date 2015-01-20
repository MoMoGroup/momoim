#include <stdio.h>
#include <setjmp.h>
#include <string.h>
//#include <jpegint.h>
#include "jpeglib.h"

struct my_error_mgr
{
    struct jpeg_error_mgr pub;
    /* "public" fields */

    jmp_buf setjmp_buffer;    /* for return to caller */
};

typedef struct my_error_mgr *my_error_ptr;


void my_error_exit(j_common_ptr cinfo)
{
    my_error_ptr myerr = (my_error_ptr) cinfo->err;

    (*cinfo->err->output_message)(cinfo);

    longjmp(myerr->setjmp_buffer, 1);
}

////////////////////////////////////////////////////////////////////////////////
int read_JPEG_file(char *buf1, char *buf2, size_t bufSize)
////////////////////////////////////////////////////////////////////////////////
{
    struct jpeg_decompress_struct cinfo;
    struct my_error_mgr jerr;
//    FILE * infile;        /* source file */
//    FILE * outfile;
    JSAMPARRAY buffer;        /* Output row buffer */
    int row_stride;        /* physical row width in output buffer */
////////////////////////////////////////////////////////////////////////////////
//    if ((infile = fopen(filename, "rb")) == NULL)
//    {
//        fprintf(stderr, "can't open %s\n", filename);
//        return 0;
//    }
//    if ((outfile = fopen(outfilename, "wb")) == NULL)
//    {
//        fprintf(stderr, "can't open %s\n", outfilename);
//        return 0;
//    }
////////////////////////////////////////////////////////////////////////////////
    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer))
    {
        //printf("Eror\n");
        jpeg_destroy_decompress(&cinfo);
//        fclose(infile);
        return 0;
    }

    jpeg_create_decompress(&cinfo);
////////////////////////////////////////////////////////////////////////////////
//    jpeg_stdio_src(&cinfo, infile);

    jpeg_mem_src(&cinfo, (unsigned char*)buf1, bufSize);
////////////////////////////////////////////////////////////////////////////////
    jpeg_read_header(&cinfo, TRUE);


    jpeg_start_decompress(&cinfo);
    row_stride = cinfo.output_width * cinfo.output_components;
    buffer = (*cinfo.mem->alloc_sarray)
            ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    while (cinfo.output_scanline < cinfo.output_height)
    {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        //put_scanline_someplace(buffer[0], row_stride);
////////////////////////////////////////////////////////////////////////////////
//        fwrite(buffer[0], 1, row_stride, outfile);
        memcpy(buf2, *buffer, row_stride);
        buf2 += row_stride;

////////////////////////////////////////////////////////////////////////////////

    }
    jpeg_finish_decompress(&cinfo);

    jpeg_destroy_decompress(&cinfo);


    return row_stride;
}