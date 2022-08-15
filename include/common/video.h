#ifndef __VIDEO_H__
#define __VIDEO_H__

#include <stdint.h>
#include <ffmpeg-io/common.h>

typedef struct {
    ffmpeg_handle ffmpeg;
    int frame_start;
    int frame_end;
    int frame_skip;
    int frame_current;
} video_t;

video_t* video_init_from_file(char* filename, int start, int end, int skip, int* i0, int* i1, int* j0, int* j1);
int video_get_next_frame(video_t* video, uint8_t** I);
void video_free(video_t* video);

#endif // __VIDEO_H__
