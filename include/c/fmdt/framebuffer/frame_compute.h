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