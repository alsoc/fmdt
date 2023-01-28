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
    char path[2048];

    uint8_t*** fra_buffer;
    size_t fra_count;
    size_t loop_size;
    size_t cur_loop;
} video_reader_t;

// pixel format
enum pixfmt_e { PIXFMT_RGB24 = 0, PIXFMT_GRAY };

typedef struct {
    ffmpeg_options ffmpeg_opts;
    ffmpeg_handle ffmpeg;
    char path[2048];
} video_writer_t;
