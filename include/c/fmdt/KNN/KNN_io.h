#pragma once

#include <stdio.h>
#include <stdint.h>

#include "fmdt/features/features_struct.h"
#include "fmdt/KNN/KNN_struct.h"

void _KNN_asso_conflicts_write(FILE* f, const uint32_t** KNN_data_nearest, const float** KNN_data_distances,
                               const uint32_t* KNN_data_conflicts, const uint32_t* ROI0_id, const uint32_t* ROI0_next_id,
                               const size_t n_ROI, const float* ROI1_dx, const float* ROI1_dy, const float* ROI1_error,
                               const uint8_t* ROI1_is_moving, const size_t n_ROI1);
void KNN_asso_conflicts_write(FILE* f, const KNN_data_t* KNN_data, const ROI_asso_t* ROI_asso_array0,
                              const ROI_asso_t* ROI_asso_array1, const ROI_motion_t* ROI_motion_array1);
