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
void _KPPV_match(KKPV_data_t* data, const uint16_t* ROI0_id, const float* ROI0_x, const float* ROI0_y,
                 int32_t* ROI0_next_id, const size_t n_ROI0, const uint16_t* ROI1_id, const float* ROI1_x,
                 const float* ROI1_y, int32_t* ROI1_prev_id, const size_t n_ROI1, const int k);
void KPPV_match(KKPV_data_t* data, ROI_t* ROI_array0, ROI_t* ROI_array1, const int k);
void KPPV_free_data(KKPV_data_t* data);

// void KPPV_save_asso(const char* filename, const uint32_t** nearest, const float** distances, ROI_array_t* ROI_array);
// void KPPV_save_asso_VT(const char* filename, int nc0, ROI_t* stats, int frame);
// void KPPV_save_conflicts(const char* filename, uint32_t* conflicts, uint32_t** nearest, float** distances,
//                          int n_asso, int n_conflict);
void KPPV_save_asso_conflicts(const char* filename, const int frame, const KKPV_data_t* data, const ROI_t* ROI_array0,
                              const ROI_t* ROI_array1, const track_t* tracks);
