#include <stdio.h>
#include <stdlib.h>

#include "fmdt/image/image_struct.h"

size_t image_get_pixsize(const enum pixfmt_e pixfmt)
{
    size_t pixsize;
    switch (pixfmt) {
        case PIXFMT_RGB24:
            pixsize = 3;
            break;
        case PIXFMT_GRAY8:
            pixsize = 1;
            break;
        default:
            fprintf(stderr, "(EE) Unsupported pixel format.\n");
            exit(1);
    }
    return pixsize;
}