#pragma once

#include <stdint.h>

#include "fmdt/features/features_struct.h"
#include "fmdt/kNN/kNN_struct.h"

kNN_data_t* kNN_alloc_and_init_data(const size_t max_size);
void _kNN_match(uint32_t** data_nearest, float** data_distances, uint32_t* data_conflicts, const uint32_t* RoI0_id,
                const uint32_t* RoI0_S, const float* RoI0_x, const float* RoI0_y, uint32_t* RoI0_next_id,
                const size_t n_RoI0, const uint32_t* RoI1_id, const uint32_t* RoI1_S, const float* RoI1_x,
                const float* RoI1_y, uint32_t* RoI1_prev_id, const size_t n_RoI1, const int k, const uint32_t max_dist,
                const float min_ratio_S);
void kNN_match(kNN_data_t* data, const RoI_basic_t* RoI_basic_array0, const RoI_basic_t* RoI_basic_array1,
               RoI_asso_t* RoI_asso_array0, RoI_asso_t* RoI_asso_array1, const int k, const uint32_t max_dist,
               const float min_ratio_S);
void kNN_free_data(kNN_data_t* data);
