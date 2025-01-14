/*!
 * \file
 * \brief Compute functions related to frames.
 */

#pragma once

#include "fmdt/framebuffer/frame_struct.h"
#include "fmdt/video/video_struct.h"
#include "fmdt/tracking/tracking_struct.h"

void frame_draw_id(frame_t* frame);

void frame_draw_legend(frame_t* frame, const int validation);

void frame_draw_boxes(frame_t* frame, const framebuffer_data_t* fb, const vec_track_t tracks, const int draw_id);

void frame_write(frame_t* frame, video_writer_t* video_writer);

frame_extractor_t* frame_extractor_alloc_init(const char* path_begin, const char* path_end, const size_t n_writers,
                                              const size_t frame_height, const size_t frame_width,
                                              const size_t n_threads, const enum pixfmt_e frame_pixfmt,
                                              const enum video_codec_e codec);

void frame_extract(frame_t* frame, frame_extractor_t* frame_extractor, const vec_track_t tracks);

void frame_extractor_free(frame_extractor_t* frame_extractor);