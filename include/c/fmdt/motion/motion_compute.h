#pragma once

#include "fmdt/features/features_struct.h"
#include "fmdt/motion/motion_struct.h"

float _motion_compute_mean_error(const float* RoI_error, const uint32_t* RoI_prev_id, const uint8_t* RoI_is_moving,
                                 const size_t n_RoI);
float motion_compute_mean_error(const RoI_asso_t* RoI_asso_array, const RoI_motion_t* RoI_motion_array);
float _motion_compute_std_deviation(const float* RoI_error, const uint32_t* RoI_prev_id, const uint8_t* RoI_is_moving,
                                    const size_t n_RoI, const float mean_error);
float motion_compute_std_deviation(const RoI_asso_t* RoI_asso_array, const RoI_motion_t* RoI_motion_array,
                                     const float mean_error);
void _motion_compute(const float* RoI0_x, const float* RoI0_y, const float* RoI1_x, const float* RoI1_y, float* RoI1_dx,
                     float* RoI1_dy, float* RoI1_error, const uint32_t* RoI1_prev_id, uint8_t* RoI1_is_moving,
                     const size_t n_RoI1, motion_t* motion_est1, motion_t* motion_est2);
void motion_compute(const RoI_basic_t* RoI_basic_array0, const RoI_basic_t* RoI_basic_array1,
                    const RoI_asso_t* RoI_asso_array1, RoI_motion_t* RoI_motion_array1, motion_t* motion_est1,
                    motion_t* motion_est2);
