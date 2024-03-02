/*!
 * \file
 * \brief Sigma-Delta compute functions.
 */

#pragma once

#include "fmdt/sigma_delta/sigma_delta_struct.h"

/**
 * Allocation of inner data required to perform Sigma-Delta algorithm.
 *
 * @param i0 The first \f$y\f$ index in the image (included).
 * @param i1 The last \f$y\f$ index in the image (included).
 * @param j0 The first \f$x\f$ index in the image (included).
 * @param j1 The last \f$x\f$ index in the image (included).
 * @param vmin Minimum value for the saturation.
 * @param vmax Minimum value for the saturation.
 * @return The allocated data.
 */
sigma_delta_data_t* sigma_delta_alloc_data(const int i0, const int i1, const int j0, const int j1, const uint8_t vmin,
                                           const uint8_t vmax);

/**
 * Initialization of inner data required to perform Sigma-Delta algorithm.
 *
 * @param sd_data Pointer of inner CCL data.
 * @param img Input greyscale image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$, \f$\{0,1\}\f$).
 * @param i0 The first \f$y\f$ index in the image (included).
 * @param i1 The last \f$y\f$ index in the image (included).
 * @param j0 The first \f$x\f$ index in the image (included).
 * @param j1 The last \f$x\f$ index in the image (included).
 */
void sigma_delta_init_data(sigma_delta_data_t* sd_data, const uint8_t** img, const int i0, const int i1, const int j0,
                           const int j1);

/**
 * Free the inner data.
 *
 * @param sd_data Inner data.
 */
void sigma_delta_free_data(sigma_delta_data_t* sd_data);

/**
 * Sigma-Delta algorithm. Per-pixel computes if a pixel intensity changed over time.
 *
 * @param sd_data Pointer of inner CCL data.
 * @param img_in Input greyscale image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$, \f$\{0,1\}\f$).
 * @param img_out Output binary image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$, \f$\{0,1\}\f$ has to be coded as
 *                \f$\{0,255\}\f$). If a pixel moved then its value is 1, otherwise it is zero.
 * @param i0 The first \f$y\f$ index in the image (included).
 * @param i1 The last \f$y\f$ index in the image (included).
 * @param j0 The first \f$x\f$ index in the image (included).
 * @param j1 The last \f$x\f$ index in the image (included).
 * @param N The Sigma-Delta parameter.
 */
void sigma_delta_compute(sigma_delta_data_t *sd_data, const uint8_t** img_in, uint8_t** img_out, const int i0,
                         const int i1, const int j0, const int j1, const uint8_t N);
