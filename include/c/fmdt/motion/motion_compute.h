#pragma once

#include "fmdt/features/features_struct.h"
#include "fmdt/motion/motion_struct.h"

float _motion_compute_mean_error(const float* RoIs_error, const uint32_t* RoIs_prev_id, const uint8_t* RoIs_is_moving,
                                 const size_t n_RoI);
float motion_compute_mean_error(const RoIs_asso_t* RoIs_asso, const RoIs_motion_t* RoIs_motion);
float _motion_compute_std_deviation(const float* RoIs_error, const uint32_t* RoIs_prev_id, const uint8_t* RoIs_is_moving,
                                    const size_t n_RoI, const float mean_error);
float motion_compute_std_deviation(const RoIs_asso_t* RoIs_asso, const RoIs_motion_t* RoIs_motion,
                                     const float mean_error);
void _motion_compute(const float* RoIs0_x, const float* RoIs0_y, const float* RoIs1_x, const float* RoIs1_y,
                     float* RoIs1_dx, float* RoIs1_dy, float* RoIs1_error, const uint32_t* RoIs1_prev_id,
                     uint8_t* RoIs1_is_moving, const size_t n_RoIs1, motion_t* motion_est1, motion_t* motion_est2);
void motion_compute(const RoIs_basic_t* RoIs0_basic, const RoIs_basic_t* RoIs1_basic, const RoIs_asso_t* RoIs1_asso,
                    RoIs_motion_t* RoIs1_motion, motion_t* motion_est1, motion_t* motion_est2);
