#pragma once

#include <stdint.h>

#include "fmdt/features.h"

typedef struct {
    int i0, i1, j0, j1;
    uint32_t** nearest;
    float** distances;
    uint32_t* conflicts;
} KKPV_data_t;

KKPV_data_t* KPPV_alloc_and_init_data(int i0, int i1, int j0, int j1);
void _KPPV_match(uint32_t** data_nearest, float** data_distances, uint32_t* data_conflicts, const uint16_t* ROI0_id,
                 const float* ROI0_x, const float* ROI0_y, int32_t* ROI0_next_id, const size_t n_ROI0,
                 const uint16_t* ROI1_id, const float* ROI1_x, const float* ROI1_y, int32_t* ROI1_prev_id, const
                 size_t n_ROI1, const int k);
void KPPV_match(KKPV_data_t* data, ROI_t* ROI_array0, ROI_t* ROI_array1, const int k);
void KPPV_free_data(KKPV_data_t* data);
void _KPPV_asso_conflicts_write(FILE* f, const uint32_t** KPPV_data_nearest, const float** KPPV_data_distances,
                                const uint16_t* ROI_id, const uint32_t* ROI_S, const float* ROI_dx, const float* ROI_dy,
                                const float* ROI_error, const int32_t* ROI_next_id, const size_t n_ROI);
void KPPV_asso_conflicts_write(FILE* f, const KKPV_data_t* KPPV_data, const ROI_t* ROI_array);
