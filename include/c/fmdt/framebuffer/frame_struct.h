/*!
 * \file
 * \brief Frame structures.
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "fmdt/image/image_struct.h"
#include "fmdt/video/video_struct.h"
#include "fmdt/features/features_struct.h"

/**
 *  Frame structure.
 */
typedef struct {
    size_t id;
    img_data_t* img;
    RoI_basic_t* RoIs;
    size_t n_RoIs;
} frame_t;

/**
 *  Frame Extractor structure.
 */
typedef struct {
    size_t* track_ids;
    video_writer_t** writers;
    size_t n_writers;
    char* path_begin;
    char* path_end;
    size_t path_len;

    size_t n_threads;
    size_t frame_height;
    size_t frame_width;
    enum pixfmt_e frame_pixfmt;
    enum video_codec_e codec_type;
} frame_extractor_t;
