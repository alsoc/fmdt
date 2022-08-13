#ifndef __VIDEO_H__
#define __VIDEO_H__

#include <ffmpeg-io/reader.h>
#include <ffmpeg-io/writer.h>
#include <nrc2.h>

typedef struct {
    ffmpeg_handle ffmpeg;
    int frame_start;
    int frame_end;
    int frame_skip;
    int frame_current;
} Video;

Video* Video_init_from_file(char* filename, int start, int end, int skip, int* i0, int* i1, int* j0, int* j1);
int Video_nextFrame(Video* video, uint8** I);
void Video_free(Video* video);

#endif // __VIDEO_H__
