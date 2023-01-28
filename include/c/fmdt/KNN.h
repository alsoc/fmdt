#pragma once

#include <stdint.h>

#include "fmdt/features.h"

typedef struct {
    size_t max_size;
    uint32_t** nearest;
    float** distances;
    uint32_t* conflicts;
} KNN_data_t;

KNN_data_t* KNN_alloc_and_init_data(const size_t max_size);
void _KNN_match(uint32_t** data_nearest, float** data_distances, uint32_t* data_conflicts, const uint32_t* ROI0_id,
                const uint32_t* ROI0_S, const float* ROI0_x, const float* ROI0_y, uint32_t* ROI0_next_id,
                const size_t n_ROI0, const uint32_t* ROI1_id, const uint32_t* ROI1_S, const float* ROI1_x,
                const float* ROI1_y, uint32_t* ROI1_prev_id, const size_t n_ROI1, const int k, const uint32_t max_dist,
                const float min_ratio_S);
void KNN_match(KNN_data_t* data, ROI_t* ROI_array0, ROI_t* ROI_array1, const int k, const uint32_t max_dist,
               const float min_ratio_S);
void KNN_free_data(KNN_data_t* data);
void _KNN_asso_conflicts_write(FILE* f, const uint32_t** KNN_data_nearest, const float** KNN_data_distances,
                               const uint32_t* KNN_data_conflicts, const uint32_t* ROI0_id, const uint32_t* ROI0_next_id,
                               const size_t n_ROI, const float* ROI1_dx, const float* ROI1_dy, const float* ROI1_error,
                               const uint8_t* ROI1_is_moving, const size_t n_ROI1);
void KNN_asso_conflicts_write(FILE* f, const KNN_data_t* KNN_data, const ROI_t* ROI_array0, const ROI_t* ROI_array1);
