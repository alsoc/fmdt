/*!
 * \file
 * \brief Functions to apply a threshold on image.
 */

#pragma once

#include <stdint.h>

#include "fmdt/features/features_struct.h"

/**
 * Convert an input image (\f$I_{in}\f$) in grayscale levels into a binary image (\f$I_{out}\f$) depending on a
 * greyscale threshold (\f$T\f$).
 * If \f$ I_{in}^i \geq T\f$ then \f$I_{out}^i = 255 \f$, else \f$I_{out}^i = 0\f$.
 * @param img_in Input greyscale image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$, \f$\{0,1\}\f$).
 * @param img_out Output binary image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$, \f$\{0,1\}\f$, \f$\{0,1\}\f$ is coded
 *                as \f$\{0,255\}\f$).
 * @param i0 First \f$y\f$ index in the image (included).
 * @param i1 Last \f$y\f$ index in the image (included).
 * @param j0 First \f$x\f$ index in the image (included).
 * @param j1 Last \f$x\f$ index in the image (included).
 * @param threshold Value that define if the pixel is kept in the output binary image or not.
 */
void threshold(const uint8_t** img_in, uint8_t** img_out, const int i0, const int i1, const int j0, const int j1,
               const uint8_t threshold);

/**
 * Filter (= select / keep) the RoIs ellipses that have a ratio (a/b) superior to \f$min_ratio\f$.
 * 
 * @param RoIs_a Semi-major ellipse axis.
 * @param RoIs_b Semi-minor ellipse axis.
 * @param n_RoIs Number of connected-components (= number of RoIs).
 * @param RoIs_id Array of RoI unique identifiers.
 * @param min_ratio Value that define if the RoI is kept or not.
 */
void _threshold_ellipse_ratio(const float* RoIs_a, const float* RoIs_b, const size_t n_RoIs, uint32_t* RoIs_id,
                              const float min_ratio);

/**
 * Filter (= select / keep) the RoIs ellipses that have a ratio (a/b) superior to \f$min_ratio\f$.
 *
 * @param RoIs_misc Miscellaneous features (including the `a` and `b` features).
 * @param min_ratio Value that define if the RoI is kept or not.
 */
void threshold_ellipse_ratio(RoIs_misc_t* RoIs_misc, const float min_ratio);
