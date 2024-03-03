/*!
 * \file
 * \brief IOs for k-Nearest Neighbors (kNN) matching algorithm.
 */

#pragma once

#include <stdio.h>
#include <stdint.h>

#include "fmdt/features/features_struct.h"
#include "fmdt/kNN/kNN_struct.h"

/**
 * Print a table of RoIs association features plus the corresponding RoIs motion features.
 *
 * @param f File descriptor (in write mode).
 * @param kNN_data Inner kNN data.
 * @param RoIs0_basic Basic features (at \f$t -1\f$).
 * @param RoIs0_asso Association features at \f$t - 1\f$.
 * @param n_RoIs0 Number of connected-components (= number of RoIs) (at \f$t - 1\f$).
 * @param RoIs1_motion Motion features at \f$t\f$ (can be NULL).
 * @param n_RoIs1 Number of connected-components (= number of RoIs) (at \f$t\f$).
 */
void kNN_asso_conflicts_write(FILE* f, const kNN_data_t* kNN_data, const RoI_basic_t* RoIs0_basic,
                              const RoI_asso_t* RoIs0_asso, const size_t n_RoIs0, const RoI_motion_t* RoIs1_motion,
                              const size_t n_RoIs1);
