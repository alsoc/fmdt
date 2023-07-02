#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmdt/video/video_struct.h"

enum video_codec_e video_str_to_enum(const char* str) {
    if (strcmp(str, "FFMPEG-IO") == 0) {
        return VCDC_FFMPEG_IO;
    } else if (strcmp(str, "VCODECS-IO") == 0) {
#ifdef FMDT_USE_VCODECS_IO
        return VCDC_VCODECS_IO;
#else
        fprintf(stderr, "(EE) '%s()' failed, 'VCODECS_IO' implementation requires to link with the vcodecs-io library.",
            __func__);
        exit(-1);
#endif
    } else {
        fprintf(stderr, "(EE) '%s()' failed, unknow input ('%s').\n", __func__, str);
        exit(-1);
    }
}
