/*!
 * \file
 * \brief k-Nearest Neighbors (kNN) matching algorithm.
 */

#pragma once

#include <stdint.h>

#include "fmdt/features/features_struct.h"
#include "fmdt/kNN/kNN_struct.h"

/**
 * Allocation of inner kNN data.
 * The `conflicts` field is allocated only if the `FMDT_ENABLE_DEBUG` macro is
 * defined.
 * @param max_size Maximum number of RoIs that can considered for associations.
 * @return Pointer of kNN data.
 */
kNN_data_t* kNN_alloc_data(const size_t max_size);

/**
 * Initialization of the kNN inner data. Set all zeros.
 * @param kNN_data Pointer of inner kNN data.
 */
void kNN_init_data(kNN_data_t* kNN_data);

/**
 * Compute associations between RoIs at \f$t - 1\f$ and RoIs at \f$t\f$.
 * @param data_distances 2D array of euclidean distances.
 * @param data_nearest 2D array of ranks.
 * @param data_conflicts 1D array of conflicts. The conflicts are filled only if the FMDT_ENABLE_DEBUG macro is defined.
 * @param RoIs0_id Array of RoI unique identifiers (at \f$t -1\f$).
 * @param RoIs0_S Array of RoI surfaces (at \f$t -1\f$).
 * @param RoIs0_x Array of centroids abscissa (at \f$t -1\f$).
 * @param RoIs0_y Array of centroids ordinate (at \f$t -1\f$).
 * @param RoIs0_next_id Array of RoI identifiers at \f$t -1 + 1 = t\f$.
 * @param n_RoIs0 Number of connected-components (= number of RoIs) (at \f$t -1\f$).
 * @param RoIs1_id Array of RoI unique identifiers (at \f$t\f$).
 * @param RoIs1_S Array of RoI surfaces (at \f$t\f$).
 * @param RoIs1_x Array of centroids abscissa (at \f$t\f$).
 * @param RoIs1_y Array of centroids ordinate (at \f$t\f$).
 * @param RoIs1_prev_id Array of RoI identifiers at \f$t - 1\f$.
 * @param n_RoIs1 Number of connected-components (= number of RoIs) (at \f$t\f$).
 * @param k Number of ranks considered for RoI associations.
 * @param max_dist Maximum distance between 2 RoIs to make the association.
 * @param min_ratio_S Minimum ratio between two RoIs. \f$ r_S = RoI_{S}^j / RoI_{S}^i\f$, if \f$r_S < r_S^{min}\f$
 *                    then the association is not made.
 * @return The number of associations.
 */
uint32_t _kNN_match(float** data_distances, uint32_t** data_nearest, uint32_t* data_conflicts, const uint32_t* RoIs0_id,
                    const uint32_t* RoIs0_S, const float* RoIs0_x, const float* RoIs0_y, uint32_t* RoIs0_next_id,
                    const size_t n_RoIs0, const uint32_t* RoIs1_id, const uint32_t* RoIs1_S, const float* RoIs1_x,
                    const float* RoIs1_y, uint32_t* RoIs1_prev_id, const size_t n_RoIs1, const int k,
                    const uint32_t max_dist, const float min_ratio_S);

/**
 * @param kNN_data Inner kNN data.
 * @param RoIs0_basic Basic features (at \f$t -1\f$).
 * @param RoIs1_basic Basic features (at \f$t\f$).
 * @param RoIs0_asso Association features (at \f$t -1\f$).
 * @param RoIs1_asso Association features (at \f$t\f$).
 * @param k Number of ranks considered for RoI associations.
 * @param max_dist Maximum distance between 2 RoIs to make the association.
 * @param min_ratio_S Minimum ratio between two RoIs. \f$ r_S = RoI_{S}^j / RoI_{S}^i\f$, if \f$r_S < r_S^{min}\f$
 *                    then the association is not made.
 * @return The number of associations.
 * @see _kNN_match for the explanations about the nature of the processing.
 */
uint32_t kNN_match(kNN_data_t* kNN_data, const RoIs_basic_t* RoIs0_basic, const RoIs_basic_t* RoIs1_basic,
                   RoIs_asso_t* RoIs0_asso, RoIs_asso_t* RoIs1_asso, const int k, const uint32_t max_dist,
                   const float min_ratio_S);

/**
 * Deallocation of inner kNN data.
 * @param kNN_data A pointer of kNN inner data.
 */
void kNN_free_data(kNN_data_t* kNN_data);
