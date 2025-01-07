/*!
 * \file
 * \brief Frame Buffer.
 */

#pragma once

#include "fmdt/framebuffer/framebuffer_struct.h"

/**
 * Allocation and initialization of a frame buffer.
 *
 * @param size Number of frames to buffer.
 * @param frame_height Frames height.
 * @param frame_width Frames width.
 * @param frame_pixfmt Frames pixels format
 * @return The allocated data.
 */
framebuffer_data_t* framebuffer_alloc_init(const size_t size, const size_t frame_height, const size_t frame_width,
                                           const enum pixfmt_e frame_pixfmt);

/**
 * Deallocation of a frame buffer.
 *
 * @param fb A pointer to a frame buffer.
 */
void framebuffer_free(framebuffer_data_t* fb);
