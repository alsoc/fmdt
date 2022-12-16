#pragma once

#include <stdint.h>

#include "fmdt/features.h"

typedef struct {
    int i0, i1, j0, j1;
    uint32_t** nearest;
    float** distances;
    uint32_t* conflicts;
} KNN_data_t;

KNN_data_t* KNN_alloc_and_init_data(int i0, int i1, int j0, int j1);
void _KNN_match(uint32_t** data_nearest, float** data_distances, uint32_t* data_conflicts, const uint16_t* ROI0_id,
                const float* ROI0_x, const float* ROI0_y, int32_t* ROI0_next_id, const size_t n_ROI0,
                const uint16_t* ROI1_id, const float* ROI1_x, const float* ROI1_y, int32_t* ROI1_prev_id, const
                size_t n_ROI1, const int k, const uint32_t max_dist_square);
void KNN_match(KNN_data_t* data, ROI_t* ROI_array0, ROI_t* ROI_array1, const int k, const uint32_t max_dist_square);
void KNN_free_data(KNN_data_t* data);
void _KNN_asso_conflicts_write(FILE* f, const uint32_t** KNN_data_nearest, const float** KNN_data_distances,
                               const uint32_t* KNN_data_conflicts, const uint16_t* ROI_id, const int32_t* ROI_next_id,
                               const size_t n_ROI, const size_t n_conflicts);
void KNN_asso_conflicts_write(FILE* f, const KNN_data_t* KNN_data, const ROI_t* ROI_array, const size_t n_conflicts);
