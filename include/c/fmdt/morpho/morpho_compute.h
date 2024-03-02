/*!
 * \file
 * \brief Morphology compute functions.
 */

#pragma once

#include "fmdt/morpho/morpho_struct.h"

/**
 * Allocation of inner data required to perform morphology.
 *
 * @param i0 The first \f$y\f$ index in the image (included).
 * @param i1 The last \f$y\f$ index in the image (included).
 * @param j0 The first \f$x\f$ index in the image (included).
 * @param j1 The last \f$x\f$ index in the image (included).
 * @return The allocated data.
 */
morpho_data_t* morpho_alloc_data(const int i0, const int i1, const int j0, const int j1);

/**
 * Initialization of inner data required to perform morphology.
 *
 * @param morpho_data Pointer of inner morpho data.
 */
void morpho_init_data(morpho_data_t* morpho_data);

/**
 * Free the inner data.
 *
 * @param morpho_data Inner data.
 */
void morpho_free_data(morpho_data_t* morpho_data);

/**
 * This function performs an erosion (3x3 convolution). This function does not compute the borders.
 *
 * @param img_in Input 2D binary image (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param img_out Output 2D binary image (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$). Note that \p img_in and \p img_out have to
 *                point to different frames (in-place computing is NOT supported).
 * @param i0 First \f$y\f$ index in the labels (included).
 * @param i1 Last \f$y\f$ index in the labels (included).
 * @param j0 First \f$x\f$ index in the labels (included).
 * @param j1 Last \f$x\f$ index in the labels (included).
 */
void morpho_compute_erosion3(const uint8_t** img_in, uint8_t** img_out, const int i0, const int i1, const int j0,
                             const int j1);

/**
 * This function performs a dilatation (3x3 convolution). This function does not compute the borders.
 *
 * @param img_in Input 2D binary image (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param img_out Output 2D binary image (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$). Note that \p img_in and \p img_out have to
 *                point to different frames (in-place computing is NOT supported).
 * @param i0 First \f$y\f$ index in the labels (included).
 * @param i1 Last \f$y\f$ index in the labels (included).
 * @param j0 First \f$x\f$ index in the labels (included).
 * @param j1 Last \f$x\f$ index in the labels (included).
 */
void morpho_compute_dilatation3(const uint8_t** img_in, uint8_t** img_out, const int i0, const int i1, const int j0,
                                const int j1);

/**
 * This function performs an opening (3x3 convolution). This function does not compute the borders.
 *
 * @param morpho_data Pointer of inner morpho data.
 * @param img_in Input 2D binary image (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param img_out Output 2D binary image (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$). Note that \p img_in and \p img_out can be
 *        the same frame (in-place computing is supported).
 * @param i0 First \f$y\f$ index in the labels (included).
 * @param i1 Last \f$y\f$ index in the labels (included).
 * @param j0 First \f$x\f$ index in the labels (included).
 * @param j1 Last \f$x\f$ index in the labels (included).
 */
void morpho_compute_opening3(morpho_data_t* morpho_data, const uint8_t** img_in, uint8_t** img_out, const int i0,
                             const int i1, const int j0, const int j1);

/**
 * This function performs a closing (3x3 convolution). This function does not compute the borders.
 *
 * @param morpho_data Pointer of inner morpho data.
 * @param img_in Input 2D binary image (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param img_out Output 2D binary image (\f$[i1 - i0 + 1][j1 - j0 + 1]\f$). Note that \p img_in and \p img_out can be
 *                the same frame (in-place computing is supported)..
 * @param i0 First \f$y\f$ index in the labels (included).
 * @param i1 Last \f$y\f$ index in the labels (included).
 * @param j0 First \f$x\f$ index in the labels (included).
 * @param j1 Last \f$x\f$ index in the labels (included).
 */
void morpho_compute_closing3(morpho_data_t* morpho_data, const uint8_t** img_in, uint8_t** img_out, const int i0,
                             const int i1, const int j0, const int j1);
