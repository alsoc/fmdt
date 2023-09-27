/*!
 * \file
 * \brief Sigma-Delta structure.
 */

#pragma once

/**
 *  Inner Sigma-Delta data required to perform the Sigma-Delta algorithm.
 */
typedef struct {
    int i0; /**< First \f$y\f$ index in the image (included). */
    int i1; /**< Last \f$y\f$ index in the image (included). */
    int j0; /**< First \f$x\f$ index in the image (included). */
    int j1; /**< Last \f$x\f$ index in the image (included). */
    uint8_t vmin; /**< Minimum value for the saturation. */
    uint8_t vmax; /**< Maximum value for the saturation. */
    uint8_t **O; /**< Difference image (in grayscale). */
    uint8_t **M; /**< Background image (= Mean image). */
    uint8_t **V; /**< Variance image. */
} sigma_delta_data_t;
