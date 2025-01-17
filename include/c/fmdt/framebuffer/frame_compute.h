/*!
 * \file
 * \brief Compute functions related to frames.
 */

#pragma once

#include "fmdt/framebuffer/frame_struct.h"
#include "fmdt/video/video_struct.h"
#include "fmdt/tracking/tracking_struct.h"

/**
 * Draw the frame id on the frame image
 *
 * @param frame A pointer to a frame.
 */
void frame_draw_id(frame_t* frame);

/**
 * Register frame_draw_id in the framebuffer.
 *
 * @param framebuffer A pointer to the framebuffer where the action will be registered.
 */
void frame_draw_id_action_register(framebuffer_data_t* framebuffer);

/**
 * Draw a legend on a frame.
 *
 * @param frame A pointer to a frame.
 * @param validation Boolean to draw the validation legend.
 */
void frame_draw_legend(frame_t* frame, const int validation);

/**
 * Register frame_draw_legend in the framebuffer.
 *
 * @param framebuffer A pointer to the framebuffer where the action will be registered.
 * @param validation A pointer to the validation boolean.
 */
void frame_draw_legend_action_register(framebuffer_data_t* framebuffer, const int* validation);

/**
 * Draw bounding boxes around tracked objects on a frame.
 *
 * @param frame A pointer to the frame.
 * @param fb A pointer to the framebuffer containing the frames.
 * @param tracks A vector of tracks.
 * @param draw_id Boolean indicating whether to draw track ids.
 */
void frame_draw_boxes(frame_t* frame, const framebuffer_data_t* fb, const vec_track_t tracks, const int draw_id);

/**
 * Register frame_draw_boxes in the framebuffer.
 *
 * @param framebuffer A pointer to the framebuffer where the action will be registered.
 * @param tracking_data A pointer to the tracking data.
 * @param draw_id A pointer to a boolean indicating whether to draw track ids.
 */
void frame_draw_boxes_action_register(framebuffer_data_t* framebuffer, const tracking_data_t* tracking_data, const int* draw_id);

/**
 * Write a frame to a video writer.
 *
 * @param frame A pointer to a frame.
 * @param video_writer A pointer to the video writer.
 */
void frame_write(frame_t* frame, video_writer_t* video_writer);

/**
 * Register frame_write in the framebuffer.
 *
 * @param framebuffer A pointer to the framebuffer where the action will be registered.
 * @param path The file path for the output video or images.
 * @param start The starting frame index.
 * @param n_ffmpeg_threads The number of threads to use for ffmpeg.
 * @param is_player A flag indicating whether to play or save.
 * @param codec_type The codec type for encoding the video.
 */
void frame_write_action_register(framebuffer_data_t* framebuffer, const char* path, const size_t start,
                                 const size_t n_ffmpeg_threads, const int is_player,
                                 const enum video_codec_e codec_type);

/**
 * Allocate and initialize a frame extractor.
 *
 * @param path The file path for the output video or images.
 * @param n_writers The number of writers for simultaneous meteors.
 * @param frame_height The height of the frames.
 * @param frame_width The width of the frames.
 * @param n_threads The number of threads to use for ffmpeg.
 * @param frame_pixfmt The output pixel format.
 * @param codec The output codec.
 * @return A pointer to the allocated frame extractor.
 */
frame_extractor_t* frame_extractor_alloc_init(const char* path, const size_t n_writers, const size_t frame_height,
                                              const size_t frame_width, const size_t n_threads,
                                              const enum pixfmt_e frame_pixfmt, const enum video_codec_e codec);

/**
 * Free a previously allocated frame extractor.
 *
 * @param frame_extractor A pointer to the frame extractor to free.
 */
void frame_extractor_free(frame_extractor_t* frame_extractor);

/**
 * Extract meteors from a frame to save them.
 *
 * @param frame A pointer to a frame.
 * @param frame_extractor A pointer to the frame extractor to use.
 * @param tracks A vector of tracking data for the objects.
 */
void frame_extract(frame_t* frame, frame_extractor_t* frame_extractor, const vec_track_t tracks);

/**
 * Register frame_extract in the framebuffer.
 *
 * @param framebuffer A pointer to the framebuffer where the action will be registered.
 * @param path The file path for output meteors.
 * @param n_writers The number of writers for simultaneous meteors.
 * @param n_threads The number of threads to use for ffmpeg.
 * @param codec The codec used by ffmpeg.
 * @param tracking_data A pointer to the tracking data.
 */
void frame_extract_action_register(framebuffer_data_t* framebuffer, const char* path, const size_t n_writers,
                                   const size_t n_threads, const enum video_codec_e codec,
                                   const tracking_data_t* tracking_data);
