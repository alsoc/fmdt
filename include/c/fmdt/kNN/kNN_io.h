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
 * @param f File descriptor (in write mode).
 * @param kNN_data_distances 2D array of euclidean distances.
 * @param kNN_data_nearest 2D array of ranks.
 * @param kNN_data_conflicts 1D array of conflicts. The conflicts are printed only if the FMDT_ENABLE_DEBUG macro is
 *                           defined.
 * @param RoIs0_id Array of RoI unique identifiers (at \f$t -1\f$).
 * @param RoIs0_next_id Array of RoI identifiers at \f$t -1 + 1 = t\f$.
 * @param n_RoIs0 Number of connected-components (= number of RoIs) (at \f$t - 1\f$).
 * @param RoIs1_dx Array of \f$x\f$ components of the distance between centroids at \f$t - 1\f$ and \f$t\f$.
 * @param RoIs1_dy Array of \f$y\f$ components of the distance between centroids at \f$t - 1\f$ and \f$t\f$.
 * @param RoIs1_error Array of velocity norms (if `is_moving` == 1) or error (if `is_moving` == 0).
 * @param RoIs1_is_moving Array of booleans that define if the RoI is moving.
 * @param n_RoIs1 Number of connected-components (= number of RoIs) (at \f$t\f$).
 */
void _kNN_asso_conflicts_write(FILE* f, const float** kNN_data_distances, const uint32_t** kNN_data_nearest,
                               const uint32_t* kNN_data_conflicts, const uint32_t* RoIs0_id,
                               const uint32_t* RoIs0_next_id, const size_t n_RoIs0, const float* RoIs1_dx,
                               const float* RoIs1_dy, const float* RoIs1_error, const uint8_t* RoIs1_is_moving,
                               const size_t n_RoIs1);

/**
 * @param f File descriptor (in write mode).
 * @param kNN_data Inner kNN data.
 * @param RoIs0_asso Association features at \f$t - 1\f$.
 * @param RoIs1_asso Association features at \f$t\f$.
 * @param RoIs1_motion Motion features at \f$t\f$.
 * @see _kNN_asso_conflicts_write for the explanations about the nature of the processing.
 */
void kNN_asso_conflicts_write(FILE* f, const kNN_data_t* kNN_data, const RoIs_asso_t* RoIs0_asso,
                              const RoIs_asso_t* RoIs1_asso, const RoIs_motion_t* RoIs1_motion);
