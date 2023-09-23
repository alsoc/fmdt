/*!
 * \file
 * \brief Visualization.
 */

#pragma once

#include "fmdt/visu/visu_struct.h"
#include "fmdt/tracking/tracking_struct.h"

/**
 * Allocation and initialization of the visualization module.
 * @param path Path to the video or images.
 * @param start Start frame number (first frame is frame 0).
 * @param n_ffmpeg_threads Number of threads used in FFMPEG to encode the video sequence (0 means FFMPEG will decide).
 * @param img_height Images height.
 * @param img_width Images width.
 * @param pixfmt Pixels format (grayscale or RGB).
 * @param codec_type Select the API to use for video codec (`VCDC_FFMPEG_IO` or `VCDC_VCODECS_IO`).
 * @param draw_track_id If 1, draw the track id corresponding to the bounding box.
 * @param win_play Boolean, if 0 write into a file, if 1 play in a SDL window.
 * @param buff_size Number of frames to buffer.
 * @param max_RoIs_size Max number of RoIs to allocate per frame.
 * @return The allocated data.
 */
visu_data_t* visu_alloc_init(const char* path, const size_t start, const size_t n_ffmpeg_threads,
                             const size_t img_height, const size_t img_width, const enum pixfmt_e pixfmt,
                             const enum video_codec_e codec_type, const uint8_t draw_track_id, const int win_play,
                             const size_t buff_size, const size_t max_RoIs_size);

/**
 * Display a frame. If the buffer is not fully filled: display nothing and just copy the current frame to the buffer.
 * @param visu A pointer of previously allocated inner visu data.
 * @param img Input grayscale/RGB image (2D array \f$[\texttt{img\_height}][\texttt{img\_width}]\f$).
 * @param RoIs Last RoIs to bufferize.
 * @param tracks A vector of tracks.
 */
void visu_display(visu_data_t* visu, const uint8_t** img, const RoIs_basic_t* RoIs, const vec_track_t tracks);

/**
 * Display all the remaining frames (= flush the the buffer).
 * @param visu A pointer of previously allocated inner visu data.
 * @param tracks A vector of tracks.
 */
void visu_flush(visu_data_t* visu, const vec_track_t tracks);

/**
 * Deallocation of inner visu data.
 * @param video A pointer of video writer inner data.
 */
void visu_free(visu_data_t* visu);
