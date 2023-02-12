/*!
 * \file
 * \brief Video structures.
 */

#pragma once

#include <stdint.h>
#include <ffmpeg-io/common.h>

/**
 *  Video reader structure.
 */
typedef struct {
    ffmpeg_options ffmpeg_opts; /*!< FFMPEG options. */
    ffmpeg_handle ffmpeg; /*!< FFMPEG handle. */
    size_t frame_start; /*!< Start frame number (first frame is frame 0). */
    size_t frame_end; /*!< Last frame number. */
    size_t frame_skip; /*!< Number of frames to skip between two frames (0 means no frame is skipped). */
    size_t frame_current; /*!< Current frame number (always starts to 0, even if `frame_start` > 0). */
    char path[2048]; /*!< Path to the video or images. */

    uint8_t*** fra_buffer; /*!< Buffer containing the all frames in memory (may be allocated or not depending on the
                                implementation). */
    size_t fra_count; /*!< Number of frames in `fra_buffer` array. */
    size_t loop_size; /*!< Number of times the video sequence should be played in loop (1 means that the video sequence
                           is played once). */
    size_t cur_loop; /*!< Current loop. */
} video_reader_t;

/**
 *  Pixel formats enumeration.
 */
enum pixfmt_e { PIXFMT_RGB24 = 0, /*!< 24 bits Red-Green-Blue. */
                PIXFMT_GRAY /*!< 8 bits grayscale. */
};

/**
 *  Video writer structure.
 */
typedef struct {
    ffmpeg_options ffmpeg_opts; /*!< FFMPEG options. */
    ffmpeg_handle ffmpeg; /*!< FFMPEG handle. */
    char path[2048]; /*!< Path to the video or images. */
} video_writer_t;
