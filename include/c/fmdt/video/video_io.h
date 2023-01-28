#pragma once

#include "fmdt/video/video_struct.h"

video_reader_t* video_reader_init(const char* path, const size_t start, const size_t end, const size_t skip,
                                  const int bufferize, const size_t n_ffmpeg_threads, int* i0, int* i1, int* j0,
                                  int* j1);
// return the frame id (positive integer) or -1 if there is no more frame to read
int video_reader_get_frame(video_reader_t* video, uint8_t** I);
void video_reader_free(video_reader_t* video);

video_writer_t* video_writer_init(const char* path, const size_t start, const size_t n_ffmpeg_threads,
                                  const size_t img_height, const size_t img_width, const enum pixfmt_e pixfmt);
void video_writer_save_frame(video_writer_t* video, const uint8_t** I);
void video_writer_free(video_writer_t* video);
