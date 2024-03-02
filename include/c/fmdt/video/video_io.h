/*!
 * \file
 * \brief IOs for video.
 */

#pragma once

#include "fmdt/video/video_struct.h"

/**
 * Allocation and initialization of inner data required for a video reader.
 *
 * @param path Path to the video or images.
 * @param start Start frame number (first frame is frame 0).
 * @param end Last frame number (if 0 then the video sequence is entirely read).
 * @param skip Number of frames to skip between two frames (0 means no frame is skipped).
 * @param bufferize Boolean to store the entire video sequence in memory first (this is useful for benchmarks
 *                  but usually the video sequences are too big to be stored in memory).
 * @param n_ffmpeg_threads Number of threads used in FFMPEG to decode the video sequence (0 means FFMPEG will decide).
 * @param codec_type Select the API to use for video codec (`VCDC_FFMPEG_IO` or `VCDC_VCODECS_IO`).
 * @param hwaccel Select Hardware accelerator (`VCDC_HWACCEL_NONE`, `VCDC_HWACCEL_NVDEC`, `VCDC_HWACCEL_VIDEOTOOLBOX`).
 *                A NULL value will default to `VCDC_HWACCEL_NONE`.
 * @param i0 Return the first \f$y\f$ index in the labels (included).
 * @param i1 Return the last \f$y\f$ index in the labels (included).
 * @param j0 Return the first \f$x\f$ index in the labels (included).
 * @param j1 Return the last \f$x\f$ index in the labels (included).
 * @return The allocated data.
 */
video_reader_t* video_reader_alloc_init(const char* path, const size_t start, const size_t end, const size_t skip,
                                        const int bufferize, const size_t n_ffmpeg_threads,
                                        const enum video_codec_e codec_type,
                                        const enum video_codec_hwaccel_e hwaccel, int* i0, int* i1, int* j0, int* j1);

/**
 * Write grayscale image in a given 2D array.
 *
 * @param video A pointer of previously allocated inner video reader data.
 * @param img Output grayscale image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @return The frame id (positive integer) or -1 if there is no more frame to read.
 */
int video_reader_get_frame(video_reader_t* video, uint8_t** img);

/**
 * Deallocation of inner video reader data.
 *
 * @param video A pointer of video reader inner data.
 */
void video_reader_free(video_reader_t* video);

/**
 * Allocation and initialization of inner data required for a video writer.
 *
 * @param path Path to the video or images.
 * @param start Start frame number (first frame is frame 0).
 * @param n_ffmpeg_threads Number of threads used in FFMPEG to encode the video sequence (0 means FFMPEG will decide).
 * @param img_height Images height.
 * @param img_width Images width.
 * @param pixfmt Pixels format (grayscale or RGB).
 * @param codec_type Select the API to use for video codec (`VCDC_FFMPEG_IO` or `VCDC_VCODECS_IO`).
 * @param win_play Boolean, if 0 write into a file, if 1 play in a SDL window.
 * @return The allocated data.
 */
video_writer_t* video_writer_alloc_init(const char* path, const size_t start, const size_t n_ffmpeg_threads,
                                        const size_t img_height, const size_t img_width, const enum pixfmt_e pixfmt,
                                        const enum video_codec_e codec_type, const int win_play);

/**
 * Allocation of inner data required for a video writer.
 *
 * @param video A pointer of previously allocated inner video writer data.
 * @param img Input grayscale/RGB image (2D array \f$[\texttt{img\_height}][\texttt{img\_width}]\f$).
 */
void video_writer_save_frame(video_writer_t* video, const uint8_t** img);

/**
 * Deallocation of inner video writer data.
 *
 * @param video A pointer of video writer inner data.
 */
void video_writer_free(video_writer_t* video);
