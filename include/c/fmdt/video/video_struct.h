/*!
 * \file
 * \brief Video structures.
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

/**
 *  Video codec enumeration
 */
enum video_codec_e { VCDC_FFMPEG_IO = 0, /*!< Library calling the `ffmpeg` executable. The communication is made through
                                              system pipes. */
                     VCDC_VCODECS_IO, /*!< Library based on `AVCodec` library calls. It should be faster than
                                           `VCDC_FFMPEG_IO`. */
};

/**
 * Video codec hardware acceleration enumeration
 */
enum video_codec_hwaccel_e {
    VCDC_HWACCEL_NONE = 0, /*!< No hardware acceleration, use the CPU. */
    VCDC_HWACCEL_NVDEC, /*!< Use NVDec from Nvidia GPUs. */
    VCDC_HWACCEL_VIDEOTOOLBOX, /*!< Use Videotoolbox on Apple devices. */
};

/**
 *  Video reader structure.
 */
typedef struct {
    enum video_codec_e codec_type; /*!< Video decoder type (`VCDC_FFMPEG_IO` or `VCDC_VCODECS_IO`). */
    void* metadata; /*!< Internal metadata used by the video decoder. */
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
    enum video_codec_e codec_type; /*!< Video encoder type (`VCDC_FFMPEG_IO` or `VCDC_VCODECS_IO`). */
    void* metadata; /*!< Internal metadata used by the video encoder. */
    char path[2048]; /*!< Path to the video or images. */
    int win_play; /*!< Boolean: if 0 write into a file, if 1 play in a SDL window. */
} video_writer_t;

/**
 * Convert a string into an `video_codec_e` enum value
 * @param str String that can be "FFMPEG-IO" or "VCODEC-IO" (if the code has been linked with vcodecs-io library)
 * @return Corresponding enum value.
 */
enum video_codec_e video_str_to_enum(const char* str);


/**
 * Convert a string into an `video_codec_hwaccel_e` enum value
 * @param str String that can be "NONE", "CUDA" or "VIDEOTOOLBOX" 
 * @return Corresponding enum value.
 */
enum video_codec_hwaccel_e video_hwaccel_str_to_enum(const char* str);
