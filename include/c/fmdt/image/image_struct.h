/*!
 * \file
 * \brief Enumerations and structures related to images.
 */

#pragma once

#include <stdint.h>
#include <stdlib.h>

/**
 *  Enumeration for colors.
 */
enum color_e { COLOR_MISC = 0, /*!< Miscellaneous color (= uninitialized). */
               COLOR_GRAY, /*!< Gray color. */
               COLOR_GREEN, /*!< Green color. */
               COLOR_RED, /*!< Red color. */
               COLOR_PURPLE, /*!< Purple color. */
               COLOR_ORANGE, /*!< Orange color. */
               COLOR_BLUE, /*!< Blue color. */
               COLOR_YELLOW, /*!< Yellow color. */
               N_COLORS /*!< Number of colors. */
};

/**
 *  Vector of colors, to use with C vector lib.
 */
typedef enum color_e* vec_color_e;

/**
 *  Red Green Blue (RGB) structure.
 */
typedef struct {
    uint8_t r; /*!< Red color component. */
    uint8_t g; /*!< Green color component. */
    uint8_t b; /*!< Blue color component. */
} rgb8_t;

/**
 *  Image data structure. Used for storing images according to different libraries (OpenCV / NRC).
 *  Note that this container can be used for grayscale and color images because it relies on opaque types.
 */
typedef struct img_data_t {
    size_t height; /*!< Image height. */
    size_t width; /*!< Image width. */
    void* pixels; /*!< Opaque type, contains image data (= the pixels). */
    void* container_2d; /*!< Opaque type, contains 2D image container. */
} img_data_t;
