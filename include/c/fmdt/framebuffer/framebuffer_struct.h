/*!
 * \file
 * \brief Frame Buffer structures.
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "fmdt/framebuffer/frame_struct.h"

/**
 *  Frame Buffer Action structure.
 */
typedef struct framebuffer_action {
    struct framebuffer_action* next;
    void (*apply)(frame_t*, void*[]);
    void (*free)(void*[]);
    void* args[];
} framebuffer_action_t;
/**
 *  Frame Buffer structure.
 */
typedef struct {
    size_t size; /*!< Size of the bufferization. */
    size_t id_read; /*!< Index of the current buffer to read. */
    size_t id_write; /*!< Index of the current buffer to write. */
    size_t n_filled; /*!< Number of filled buffers. */
    size_t frame_skip; /*!< Number of skipped frames between two processings (generally 0). */
    size_t frame_height; /*!< Frames height. */
    size_t frame_width; /*!< Frames width. */
    enum pixfmt_e frame_pixfmt; /*!< Frames pixel format */
    size_t max_RoIs_size; /*!< Maximum capacity of the RoIs arrays. */
    frame_t* frames; /*!< Array of frames. */
    framebuffer_action_t* actions;
} framebuffer_data_t;