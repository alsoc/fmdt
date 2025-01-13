/*!
 * \file
 * \brief Compute functions related to frames.
 */

#pragma once

#include "fmdt/framebuffer/frame_struct.h"
#include "fmdt/video/video_struct.h"

void frame_draw_id(frame_t* frame);

void frame_write(frame_t* frame, video_writer_t* video_writer);