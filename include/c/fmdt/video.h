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

video_reader_t* video_reader_init(const char* path, const size_t start, const size_t end, const size_t skip,
                                  const int bufferize, const size_t n_ffmpeg_threads, int* i0, int* i1, int* j0,
                                  int* j1);
// return the frame id (positive integer) or -1 if there is no more frame to read
int video_reader_get_frame(video_reader_t* video, uint8_t** I);
void video_reader_free(video_reader_t* video);

// pixel format
enum pixfmt_e { PIXFMT_RGB24 = 0, PIXFMT_GRAY };

typedef struct {
    ffmpeg_options ffmpeg_opts;
    ffmpeg_handle ffmpeg;
    char path[2048];
} video_writer_t;

video_writer_t* video_writer_init(const char* path, const size_t start, const size_t n_ffmpeg_threads,
                                  const size_t img_height, const size_t img_width, const enum pixfmt_e pixfmt);
void video_writer_save_frame(video_writer_t* video, const uint8_t** I);
void video_writer_free(video_writer_t* video);
