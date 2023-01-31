#pragma once

#include <stdio.h>
#include <stdint.h>

#include "fmdt/features/features_struct.h"
#include "fmdt/kNN/kNN_struct.h"

void _kNN_asso_conflicts_write(FILE* f, const uint32_t** kNN_data_nearest, const float** kNN_data_distances,
                               const uint32_t* kNN_data_conflicts, const uint32_t* RoI0_id, const uint32_t* RoI0_next_id,
                               const size_t n_RoI, const float* RoI1_dx, const float* RoI1_dy, const float* RoI1_error,
                               const uint8_t* RoI1_is_moving, const size_t n_RoI1);
void kNN_asso_conflicts_write(FILE* f, const kNN_data_t* kNN_data, const RoI_asso_t* RoI_asso_array0,
                              const RoI_asso_t* RoI_asso_array1, const RoI_motion_t* RoI_motion_array1);
