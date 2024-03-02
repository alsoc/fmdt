/*!
 * \file
 * \brief Connected-Component Labeling (CCL) functions.
 */

#pragma once

#include <stdint.h>

#include "fmdt/CCL/CCL_struct.h"
#include "fmdt/features/features_struct.h"

/**
 * Allocation of inner data required to perform Light Speed Labeling (LSL).
 * Arthur HENNEQUIN's LSL implementation.
 *
 * @param i0 The first \f$y\f$ index in the image (included).
 * @param i1 The last \f$y\f$ index in the image (included).
 * @param j0 The first \f$x\f$ index in the image (included).
 * @param j1 The last \f$x\f$ index in the image (included).
 * @return The allocated data.
 */
CCL_data_t* CCL_LSL_alloc_data(int i0, int i1, int j0, int j1);

/**
 * Initialization of the CCL inner data. Set all zeros.
 * Arthur HENNEQUIN's LSL implementation.
 *
 * @param CCL_data Pointer of inner CCL data.
 */
void CCL_LSL_init_data(CCL_data_t* CCL_data);

/**
 * Free the inner data.
 * Arthur HENNEQUIN's LSL implementation.
 *
 * @param CCL_data Inner data.
 */
void CCL_LSL_free_data(CCL_data_t* CCL_data);

/**
 * Compute the Light Speed Labeling (LSL) algorithm.
 * Arthur HENNEQUIN's LSL implementation.
 *
 * @param CCL_data Inner data required to perform the LSL.
 * @param img Input binary image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$, \f$\{0,1\}\f$ has to be coded as
 *            \f$\{0,255\}\f$).
 * @param labels Output labels (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$. The labels are in \f$[1;2^{32} -1]\f$ and
 *               0 value means no label).
 * @param no_init_labels If this boolean is set to `1`, then the \p labels buffer is considered pre-initialized with `0`
 *                       values. Else, if \p no_labels_init parameter is set to `0`, then this function will initialized
 *                       zones that does not correspond to connected-components with `0` value. In doubt, prefer to set
 *                       \p no_labels_init parameter to `0`.
 * @return Number of labels.
 */
uint32_t CCL_LSL_apply(CCL_data_t *CCL_data, const uint8_t** img, uint32_t** labels, const uint8_t no_init_labels);

/**
 * First select pixels according to a threshold, then compute the Light Speed Labeling (LSL) algorithm.
 * Note: this is optimized to be faster than to compute the thresholding and to perform the LSL separately.
 * Arthur HENNEQUIN's LSL implementation.
 *
 * @param CCL_data Inner data required to perform the LSL.
 * @param img Input grayscale image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$, grayscale is in \f$[0;255]\f$ range).
 * @param labels Output labels (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$. The labels are in \f$[1;2^{32} -1]\f$ and
 *               0 value means no label).
 * @param threshold Value (between \f$[0;255]\f$). If the pixel intensity is higher than \p threshold, then the pixel is
 *                  kept for the labeling, else the pixel is ignored.
 * @param no_init_labels If this boolean is set to `1`, then the \p labels buffer is considered pre-initialized with `0`
 *                       values. Else, if \p no_labels_init parameter is set to `0`, then this function will initialized
 *                       zones that does not correspond to connected-components with `0` value. In doubt, prefer to set
 *                       \p no_labels_init parameter to `0`.
 * @return Number of labels.
 */
uint32_t CCL_LSL_threshold_apply(CCL_data_t *CCL_data, const uint8_t** img, uint32_t** labels, const uint8_t threshold,
                                 const uint8_t no_init_labels);

/**
 * First select pixels according to a threshold, then compute the Light Speed Labeling (LSL) algorithm and
 * finally extract basic features.
 * Note: this is optimized to be faster than to compute the thresholding, to perform the LSL and to extract the features
 * separately.
 * Note2: if the returned number of labels is higher than the `*RoIs_basic->_max_size` value, then the features are not
 * filled.
 * Arthur HENNEQUIN's LSL implementation.
 *
 * @param CCL_data Inner data required to perform the LSL.
 * @param img Input grayscale image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$, grayscale is in \f$[0;255]\f$ range).
 * @param labels Output labels (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$. The labels are in \f$[1;2^{32} -1]\f$ and
 *               0 value means no label).
 * @param threshold Value (between \f$[0;255]\f$). If the pixel intensity is higher than \p threshold, then the pixel is
 *                  kept for the labeling, else the pixel is ignored.
 * @param RoIs_basic Basic features.
 * @param max_RoIs_size Maximum capacity of the `RoIs_basic` array.
 * @param no_init_labels If this boolean is set to `1`, then the \p labels buffer is considered pre-initialized with `0`
 *                       values. Else, if \p no_labels_init parameter is set to `0`, then this function will initialized
 *                       zones that does not correspond to connected-components with `0` value. In doubt, prefer to set
 *                       \p no_labels_init parameter to `0`.
 * @return Number of labels.
 */
uint32_t CCL_LSL_threshold_features_apply(CCL_data_t *CCL_data, const uint8_t** img, uint32_t** labels,
                                          const uint8_t threshold, RoI_basic_t* RoIs_basic, const size_t max_RoIs_size,
                                          const uint8_t no_init_labels);

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

/**
 * Allocation of inner data required to perform Connected-Components Labeling (CCL).
 * Generic CCL implementation.
 *
 * @param impl Selected implementation (LSLH or LSLM).
 * @param i0 The first \f$y\f$ index in the image (included).
 * @param i1 The last \f$y\f$ index in the image (included).
 * @param j0 The first \f$x\f$ index in the image (included).
 * @param j1 The last \f$x\f$ index in the image (included).
 * @return The allocated and initialized data.
 */
CCL_gen_data_t* CCL_alloc_data(const enum ccl_impl_e impl, const int i0, const int i1, const int j0, const int j1);

/**
 * Initialization of the CCL inner data. Set all zeros.
 * Generic CCL implementation.
 *
 * @param CCL_data Pointer of inner CCL data.
 */
void CCL_init_data(CCL_gen_data_t* CCL_data);

/**
 * Free the inner data.
 * Generic CCL implementation.
 *
 * @param CCL_data Inner data.
 */
void CCL_free_data(CCL_gen_data_t* CCL_data);

/**
 * Compute a Connected-Components Labeling algorithm.
 * Generic CCL implementation.
 *
 * @param CCL_data Inner data required to perform the CCL.
 * @param img Input binary image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$, \f$\{0,1\}\f$ has to be coded as
 *            \f$\{0,255\}\f$).
 * @param labels Output labels (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$. The labels are in \f$[1;2^{32} -1]\f$ and
 *               0 value means no label).
 * @param no_init_labels If this boolean is set to `1`, then the \p labels buffer is considered pre-initialized with `0`
 *                       values. Else, if \p no_labels_init parameter is set to `0`, then this function will initialized
 *                       zones that does not correspond to connected-components with `0` value. In doubt, prefer to set
 *                       \p no_labels_init parameter to `0`.
 * @return Number of labels.
 */
uint32_t CCL_apply(CCL_gen_data_t* CCL_data, const uint8_t** img, uint32_t** labels, const uint8_t no_init_labels);

/**
 * First select pixels according to a threshold, then compute a Connected-Components Labeling algorithm.
 * Note: this is optimized to be faster than to compute the thresholding and to perform the CCL separately.
 * Generic CCL implementation.
 *
 * @param CCL_data Inner data required to perform the CCL.
 * @param img Input grayscale image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$, grayscale is in \f$[0;255]\f$ range).
 * @param labels Output labels (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$. The labels are in \f$[1;2^{32} -1]\f$ and
 *               0 value means no label).
 * @param threshold Value (between \f$[0;255]\f$). If the pixel intensity is higher than \p threshold, then the pixel is
 *                  kept for the labeling, else the pixel is ignored.
 * @param no_init_labels If this boolean is set to `1`, then the \p labels buffer is considered pre-initialized with `0`
 *                       values. Else, if \p no_labels_init parameter is set to `0`, then this function will initialized
 *                       zones that does not correspond to connected-components with `0` value. In doubt, prefer to set
 *                       \p no_labels_init parameter to `0`.
 * @return Number of labels.
 */
uint32_t CCL_threshold_apply(CCL_gen_data_t* CCL_data, const uint8_t** img, uint32_t** labels, const uint8_t threshold,
                             const uint8_t no_init_labels);

/**
 * First select pixels according to a threshold, then compute a Connected-Components Labeling algorithm and
 * finally extract basic features.
 * Note: this is optimized to be faster than to compute the thresholding, to perform the CCL and to extract the features
 * separately.
 * Note2: if the returned number of labels is higher than the `*RoIs_basic->_max_size` value, then the features are not
 * filled.
 * Generic CCL implementation.
 *
 * @param CCL_data Inner data required to perform the CCL.
 * @param img Input grayscale image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$, grayscale is in \f$[0;255]\f$ range).
 * @param labels Output labels (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$. The labels are in \f$[1;2^{32} -1]\f$ and
 *               0 value means no label).
 * @param threshold Value (between \f$[0;255]\f$). If the pixel intensity is higher than \p threshold, then the pixel is
 *                  kept for the labeling, else the pixel is ignored.
 * @param RoIs_basic Basic features.
 * @param max_RoIs_size Maximum capacity of the `RoIs_basic` array.
 * @param no_init_labels If this boolean is set to `1`, then the \p labels buffer is considered pre-initialized with `0`
 *                       values. Else, if \p no_labels_init parameter is set to `0`, then this function will initialized
 *                       zones that does not correspond to connected-components with `0` value. In doubt, prefer to set
 *                       \p no_labels_init parameter to `0`.
 * @return Number of labels.
 */
uint32_t CCL_threshold_features_apply(CCL_gen_data_t *CCL_data, const uint8_t** img, uint32_t** labels,
                                      const uint8_t threshold, RoI_basic_t* RoIs_basic, const size_t max_RoIs_size,
                                      const uint8_t no_init_labels);
