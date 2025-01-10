/*!
 * \file
 * \brief Frame Buffer structures.
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "fmdt/image/image_struct.h"
#include "fmdt/features/features_struct.h"


/**
 *  Frame structure.
 */
typedef struct {
    int id;
    img_data_t* img;
    RoI_basic_t* RoIs;
    size_t n_RoIs;
} frame_t;


/**
 *  Frame Buffer structure.
 */
typedef struct {
    size_t size; /*!< Size of the bufferization. */
    size_t id_read; /*!< Index of the current buffer to read. */
    size_t id_write; /*!< Index of the current buffer to write. */
    size_t n_filled; /*!< Number of filled buffers. */
    size_t frame_height; /*!< Frames height. */
    size_t frame_width; /*!< Frames width. */
    enum pixfmt_e frame_pixfmt; /*!< Frames pixel format */
    size_t max_RoIs_size; /*!< Maximum capacity of the RoIs arrays. */
    frame_t* frames; /*!< Array of frames. */
} framebuffer_data_t;
