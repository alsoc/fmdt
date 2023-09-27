/*!
 * \file
 * \brief Morphology structure.
 */

#pragma once

/**
 *  Inner data required to perform morphology.
 */
typedef struct {
    int i0; /**< First \f$y\f$ index in the image (included). */
    int i1; /**< Last \f$y\f$ index in the image (included). */
    int j0; /**< First \f$x\f$ index in the image (included). */
    int j1; /**< Last \f$x\f$ index in the image (included). */
    uint8_t **IB; /**< Temporary binary image. */
} morpho_data_t;
