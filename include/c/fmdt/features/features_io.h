#pragma once

#include "fmdt/features/features_struct.h"

// defined in "tracking.h"
typedef struct track track_t;
typedef track_t* vec_track_t;

void _features_ROI_write(FILE* f, const int frame, const uint32_t* ROI_id, const uint32_t* ROI_xmin,
                         const uint32_t* ROI_xmax, const uint32_t* ROI_ymin, const uint32_t* ROI_ymax,
                         const uint32_t* ROI_S, const uint32_t* ROI_Sx, const uint32_t* ROI_Sy, const float* ROI_x,
                         const float* ROI_y, const uint32_t* ROI_magnitude, const size_t n_ROI,
                         const vec_track_t track_array, const unsigned age);
void features_ROI_write(FILE* f, const int frame, const ROI_t* ROI_array, const vec_track_t track_array,
                        const unsigned age);
void _features_ROI0_ROI1_write(FILE* f, const int prev_frame, const int cur_frame, const uint32_t* ROI0_id,
                               const uint32_t* ROI0_xmin, const uint32_t* ROI0_xmax, const uint32_t* ROI0_ymin,
                               const uint32_t* ROI0_ymax, const uint32_t* ROI0_S, const uint32_t* ROI0_Sx,
                               const uint32_t* ROI0_Sy, const float* ROI0_x, const float* ROI0_y,
                               const uint32_t* ROI0_magnitude, const size_t n_ROI0, const uint32_t* ROI1_id,
                               const uint32_t* ROI1_xmin, const uint32_t* ROI1_xmax, const uint32_t* ROI1_ymin,
                               const uint32_t* ROI1_ymax, const uint32_t* ROI1_S, const uint32_t* ROI1_Sx,
                               const uint32_t* ROI1_Sy, const float* ROI1_x, const float* ROI1_y,
                               const uint32_t* ROI1_magnitude, const size_t n_ROI1, const vec_track_t track_array);
void features_ROI0_ROI1_write(FILE* f, const int prev_frame, const int cur_frame, const ROI_t* ROI_array0,
                              const ROI_t* ROI_array1, const vec_track_t track_array);
void features_motion_write(FILE* f, const motion_t* motion_est1, const motion_t* motion_est2);
