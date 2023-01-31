#pragma once

#include <stdint.h>

#include "fmdt/features/features_struct.h"
#include "fmdt/kNN/kNN_struct.h"

kNN_data_t* kNN_alloc_and_init_data(const size_t max_size);
void _kNN_match(uint32_t** data_nearest, float** data_distances, uint32_t* data_conflicts, const uint32_t* RoIs0_id,
                const uint32_t* RoIs0_S, const float* RoIs0_x, const float* RoIs0_y, uint32_t* RoIs0_next_id,
                const size_t n_RoIs0, const uint32_t* RoIs1_id, const uint32_t* RoIs1_S, const float* RoIs1_x,
                const float* RoIs1_y, uint32_t* RoIs1_prev_id, const size_t n_RoIs1, const int k, const uint32_t max_dist,
                const float min_ratio_S);
void kNN_match(kNN_data_t* data, const RoIs_basic_t* RoIs0_basic, const RoIs_basic_t* RoIs1_basic,
               RoIs_asso_t* RoIs0_asso, RoIs_asso_t* RoIs1_asso, const int k, const uint32_t max_dist,
               const float min_ratio_S);
void kNN_free_data(kNN_data_t* data);
