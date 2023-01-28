#pragma once

#include "fmdt/features/features_struct.h"
#include "fmdt/motion/motion_struct.h"

float _motion_compute_mean_error(const float* ROI_error, const uint32_t* ROI_prev_id, const uint8_t* ROI_is_moving,
                                 const size_t n_ROI);
float motion_compute_mean_error(const ROI_asso_t* ROI_asso_array, const ROI_motion_t* ROI_motion_array);
float _motion_compute_std_deviation(const float* ROI_error, const uint32_t* ROI_prev_id, const uint8_t* ROI_is_moving,
                                    const size_t n_ROI, const float mean_error);
float motion_compute_std_deviation(const ROI_asso_t* ROI_asso_array, const ROI_motion_t* ROI_motion_array,
                                     const float mean_error);
void _motion_compute(const float* ROI0_x, const float* ROI0_y, const float* ROI1_x, const float* ROI1_y, float* ROI1_dx,
                     float* ROI1_dy, float* ROI1_error, const uint32_t* ROI1_prev_id, uint8_t* ROI1_is_moving,
                     const size_t n_ROI1, motion_t* motion_est1, motion_t* motion_est2);
void motion_compute(const ROI_basic_t* ROI_basic_array0, const ROI_basic_t* ROI_basic_array1,
                    const ROI_asso_t* ROI_asso_array1, ROI_motion_t* ROI_motion_array1, motion_t* motion_est1,
                    motion_t* motion_est2);
