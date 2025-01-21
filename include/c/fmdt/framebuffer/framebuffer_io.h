/*!
 * \file
 * \brief Frame Buffer.
 */

#pragma once

#include "fmdt/framebuffer/framebuffer_struct.h"
#include "fmdt/video/video_struct.h"

/**
 * Allocation and initialization of a Frame Buffer.
 *
 * @param size Number of frames to buffer.
 * @param frame_height Frames height.
 * @param frame_width Frames width.
 * @param frame_skip Number of skipped frames.
 * @param frame_pixfmt Frames pixels format.
 * @param max_RoIs_size Maximum number of RoIs per frame.
 * @return The allocated data.
 */
framebuffer_data_t* framebuffer_alloc_init(const size_t size, const size_t frame_height, const size_t frame_width,
                                           const size_t frame_skip, const enum pixfmt_e frame_pixfmt,
                                           const size_t max_RoIs_size);

/**
 * Push a frame and its data into the Frame Buffer.
 *
 * @param fb A pointer to the Frame Buffer.
 * @param frame_id Identifier for the frame being pushed.
 * @param img_rgb24 Pointer to the image data in RGB24 format.
 * @param RoIs Array of RoIs of the frame.
 * @param n_RoIs Number of RoIs.
 */
void framebuffer_push(framebuffer_data_t* fb, const int frame_id, const uint8_t** img_rgb24, const RoI_basic_t* RoIs,
                      const size_t n_RoIs);

/**
 * Pop the oldest frame from the Frame Buffer and apply registered action on it.
 *
 * @param fb A pointer to the Frame Buffer.
 * @return A pointer to the frame structure that was removed.
 */
frame_t* framebuffer_pop(framebuffer_data_t* fb);

/**
 * Flush all frames from the Frame Buffer and apply registered action on each of them.
 *
 * @param fb A pointer to the Frame Buffer.
 */
void framebuffer_flush(framebuffer_data_t* fb);

/**
 * Allocate a new Action for Frame Buffer.
 *
 * @param n_args Number of arguments the action has.
 * @return A pointer to the allocated framebuffer action structure.
 */
framebuffer_action_t* framebuffer_action_alloc(size_t n_args);

/**
 * Register an action to be executed when the pop method of the Frame Buffer is called.
 *
 * @param fb A pointer to the Frame Buffer.
 * @param action A pointer to the framebuffer action to register.
 */
void framebuffer_action_register(framebuffer_data_t* fb, framebuffer_action_t* action);

/**
 * Free a previously allocated framebuffer action structure.
 *
 * @param action A pointer to the framebuffer action to free.
 */
void framebuffer_action_free(framebuffer_action_t* action);

/**
 * Deallocation of a Frame Buffer.
 *
 * @param fb A pointer to a Frame Buffer.
 */
void framebuffer_free(framebuffer_data_t* fb);
