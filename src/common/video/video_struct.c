#include "fmdt/video/video_struct.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


enum video_codec_e video_str_to_enum(const char* str) {

    if (strcmp(str, "FFMPEG-IO") == 0) {
	return FFMPEG_IO;
    } else if (strcmp(str, "VCODEC-IO") == 0) {

#ifdef FMDT_USE_VCODECS_IO
	return VCODECS_IO;
#else
	fprintf(stderr, "(EE) '%s()' failed, 'VCODECS_IO' implementation requires to link with the vcodecs-io library'",
		__func__);
	exit(-1);
#endif // FMDT_USE_VCODECS_IO	
    } else {
	fprintf(stderr, "(EE) '%s()' failed, unknow input ('%s').\n", __func__, str);
	exit(-1);
    }
}
