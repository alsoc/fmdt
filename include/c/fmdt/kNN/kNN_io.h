#pragma once

#include <stdio.h>
#include <stdint.h>

#include "fmdt/features/features_struct.h"
#include "fmdt/kNN/kNN_struct.h"

void _kNN_asso_conflicts_write(FILE* f, const uint32_t** kNN_data_nearest, const float** kNN_data_distances,
                               const uint32_t* kNN_data_conflicts, const uint32_t* RoIs0_id,
                               const uint32_t* RoIs0_next_id, const size_t n_RoIs0, const float* RoIs1_dx,
                               const float* RoIs1_dy, const float* RoIs1_error, const uint8_t* RoIs1_is_moving,
                               const size_t n_RoIs1);
void kNN_asso_conflicts_write(FILE* f, const kNN_data_t* kNN_data, const RoIs_asso_t* RoIs0_asso,
                              const RoIs_asso_t* RoIs1_asso, const RoIs_motion_t* RoIs1_motion);
