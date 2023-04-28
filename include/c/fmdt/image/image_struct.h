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
               N_COLORS /*!< Number of colors in the enumeration. */
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
typedef struct {
    size_t height; /*!< Image height. */
    size_t width; /*!< Image width. */
    void* pixels; /*!< Opaque type, contains image data (= the pixels). */
    void* container_2d; /*!< Opaque type, contains 2D image container. */
} img_data_t;

/**
 *  Bounding box structure. Used to represent the bounding box around a RoI.
 */
typedef struct {
    uint32_t frame_id; /*!< Frame id corresponding to the bounding box. */
    uint32_t track_id; /*!< Track id corresponding to the bounding box. */
    uint32_t bb_x; /*!< Center \f$x\f$ of the bounding box. */
    uint32_t bb_y; /*!< Center \f$y\f$ of the bounding box. */
    uint32_t rx; /*!< Radius \f$x\f$ of the bounding box. */
    uint32_t ry; /*!< Radius \f$y\f$ of the bounding box. */
    int is_extrapolated; /*!< Boolean that defines if the bounding box is a real bounding box (from a
                              connected-component) or if it has been extrapolated in the tracking. */
} BB_t;

/**
 *  Vector of `BB_t`, to use with C vector lib.
 */
typedef BB_t* vec_BB_t;
