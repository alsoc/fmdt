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
    size_t id; /*!< Frame id. */
    img_data_t* img; /*!< Image data of the frame. */
    RoI_basic_t* RoIs; /*!< Array of RoIs of the frame. */
    size_t n_RoIs; /*!< Number of RoIs. */
} frame_t;

/**
 *  Frame Extractor structure.
 */
typedef struct {
    size_t* track_ids; /*!< Array of track ids ( = buffer). */
    video_writer_t** writers; /*!< Array of writers ( = buffer). */
    size_t n_writers; /*!< Number of writers. */
    char* path; /*!< Path to save meteor images or videos. */
    size_t path_len; /*!< Path length */
    size_t n_meteors; /*!< Number of detected meteors */

    size_t n_threads; /*!< Number of threads per writers */
    size_t frame_height; /*!< Frame height */
    size_t frame_width; /*!< Frame width */
    enum pixfmt_e frame_pixfmt; /*!< Pixel format for writers */
    enum video_codec_e codec_type; /*!< Codec for writers */
} frame_extractor_t;
