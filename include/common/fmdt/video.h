#pragma once

#include <stdint.h>
#include <ffmpeg-io/common.h>

typedef struct {
    ffmpeg_options ffmpeg_opts;
    ffmpeg_handle ffmpeg;
    size_t frame_start;
    size_t frame_end;
    size_t frame_skip;
    size_t frame_current;
    char filename[2048];

    uint8_t*** fra_buffer;
    size_t fra_count;
    size_t loop_size;
    size_t cur_loop;
} video_t;

video_t* video_init_from_file(const char* filename, const size_t start, const size_t end, const size_t skip,
                              const int bufferize, const size_t n_ffmpeg_threads, int* i0, int* i1, int* j0, int* j1);
int video_get_next_frame(video_t* video, uint8_t** I);
void video_free(video_t* video);
