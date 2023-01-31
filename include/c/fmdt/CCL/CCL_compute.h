/*!
 * \file
 * \brief Connected-Component Labeling (CCL) functions.
 */

#pragma once

#include <stdint.h>

#include "fmdt/CCL/CCL_struct.h"

/**
 * Allocation and initialization of inner data required to perform Light Speed Labeling (LSL).
 * @param i0 The first \f$y\f$ index in the image (included).
 * @param i1 The last \f$y\f$ index in the image (included).
 * @param j0 The first \f$x\f$ index in the image (included).
 * @param j1 The last \f$x\f$ index in the image (included).
 * @return The allocated and initialized data.
 */
CCL_data_t* CCL_LSL_alloc_and_init_data(int i0, int i1, int j0, int j1);

/**
 * Compute the Light Speed Labeling (LSL) algorithm.
 * @param data_er Relative labels (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param data_era Relative <-> absolute labels equivalences (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param data_rlc Run-length coding (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$).
 * @param data_eq Table of equivalence (1D array \f$[(i1 - i0 + 1) * (j1 - j0 + 1)]\f$).
 * @param data_ner Number of relative labels (1D array \f$[i1 - i0 + 1]\f$).
 * @param img Input binary image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$, \f$\{0,1\}\f$ has to be coded as
 *            \f$\{0,255\}\f$).
 * @param labels Output labels (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$. The labels are in \f$[1;2^{32} -1]\f$ and
 *               0 value means no label).
 * @param i0 First \f$y\f$ index in the image (included).
 * @param i1 Last \f$y\f$ index in the image (included).
 * @param j0 First \f$x\f$ index in the image (included).
 * @param j1 Last \f$x\f$ index in the image (included).
 * @return Number of labels.
 */
uint32_t _CCL_LSL_apply(uint32_t** data_er, uint32_t** data_era, uint32_t** data_rlc, uint32_t* data_eq,
                        uint32_t* data_ner, const uint8_t** img, uint32_t** labels, const int i0, const int i1,
                        const int j0, const int j1);

/**
 * Compute the Light Speed Labeling (LSL) algorithm.
 * @param data Inner data required to perform the LSL.
 * @param img Input binary image (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$, \f$\{0,1\}\f$ has to be coded as
 *            \f$\{0,255\}\f$).
 * @param labels Output labels (2D array \f$[i1 - i0 + 1][j1 - j0 + 1]\f$. The labels are in \f$[1;2^{32} -1]\f$ and
 *               0 value means no label).
 * @return Number of labels.
 */
uint32_t CCL_LSL_apply(CCL_data_t *data, const uint8_t** img, uint32_t** labels);

/**
 * Free the inner data.
 * @param data Inner data.
 */
void CCL_LSL_free_data(CCL_data_t* data);
