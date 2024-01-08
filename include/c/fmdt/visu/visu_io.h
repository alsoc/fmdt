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
 * @param draw_legend If 1, draw the legend on images.
 * @param win_play Boolean, if 0 write into a file, if 1 play in a SDL window.
 * @param buff_size Number of frames to buffer.
 * @param max_RoIs_size Max number of RoIs to allocate per frame.
 * @param skip_fra Number of skipped frames between two 'visu_display' calls (generally this is 0).
 * @return The allocated data.
 */
visu_data_t* visu_alloc_init(const char* path, const size_t start, const size_t n_ffmpeg_threads,
                             const size_t img_height, const size_t img_width, const enum pixfmt_e pixfmt,
                             const enum video_codec_e codec_type, const uint8_t draw_track_id,
                             const uint8_t draw_legend, const int win_play, const size_t buff_size,
                             const size_t max_RoIs_size, const uint8_t skip_fra);

/**
 * Display a frame. If the buffer is not fully filled: display nothing and just copy the current frame to the buffer.
 * @param visu A pointer of previously allocated inner visu data.
 * @param img Input grayscale/RGB image (2D array \f$[\texttt{img\_height}][\texttt{img\_width}]\f$).
 * @param RoIs_xmin Array of minimum \f$x\f$ coordinates of the bounding box.
 * @param RoIs_xmax Array of maximum \f$x\f$ coordinates of the bounding box.
 * @param RoIs_ymin Array of minimum \f$y\f$ coordinates of the bounding box.
 * @param RoIs_ymax Array of maximum \f$y\f$ coordinates of the bounding box.
 * @param RoIs_x Array of centroids abscissa.
 * @param RoIs_y Array of centroids ordinate.
 * @param n_RoIs Number of connected-components (= number of RoIs) in the 2D array of `labels`.
 * @param tracks A vector of tracks.
 * @param frame_id the current frame id.
 */
void _visu_display(visu_data_t* visu, const uint8_t** img, const uint32_t* RoIs_xmin, const uint32_t* RoIs_xmax,
                   const uint32_t* RoIs_ymin, const uint32_t* RoIs_ymax, const float* RoIs_x, const float* RoIs_y,
                   const size_t n_RoIs, const vec_track_t tracks, const uint32_t frame_id);

/**
 * Display a frame. If the buffer is not fully filled: display nothing and just copy the current frame to the buffer.
 * @param visu A pointer of previously allocated inner visu data.
 * @param img Input grayscale/RGB image (2D array \f$[\texttt{img\_height}][\texttt{img\_width}]\f$).
 * @param RoIs Last RoIs to bufferize.
 * @param tracks A vector of tracks.
 * @param frame_id the current frame id.
 */
void visu_display(visu_data_t* visu, const uint8_t** img, const RoIs_basic_t* RoIs, const vec_track_t tracks,
                  const uint32_t frame_id);

/**
 * Display all the remaining frames (= flush the the buffer).
 * @param visu A pointer of previously allocated inner visu data.
 * @param tracks A vector of tracks.
 */
void visu_flush(visu_data_t* visu, const vec_track_t tracks);

/**
 * Deallocation of inner visu data.
 * @param visu A pointer of video writer inner data.
 */
void visu_free(visu_data_t* visu);
