#pragma once

#include <stdint.h>

#include "fmdt/features/features_struct.h"
#include "fmdt/KNN/KNN_struct.h"

KNN_data_t* KNN_alloc_and_init_data(const size_t max_size);
void _KNN_match(uint32_t** data_nearest, float** data_distances, uint32_t* data_conflicts, const uint32_t* ROI0_id,
                const uint32_t* ROI0_S, const float* ROI0_x, const float* ROI0_y, uint32_t* ROI0_next_id,
                const size_t n_ROI0, const uint32_t* ROI1_id, const uint32_t* ROI1_S, const float* ROI1_x,
                const float* ROI1_y, uint32_t* ROI1_prev_id, const size_t n_ROI1, const int k, const uint32_t max_dist,
                const float min_ratio_S);
void KNN_match(KNN_data_t* data, ROI_t* ROI_array0, ROI_t* ROI_array1, const int k, const uint32_t max_dist,
               const float min_ratio_S);
void KNN_free_data(KNN_data_t* data);
