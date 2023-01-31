#pragma once

#include <stdio.h>

#include "fmdt/features/features_struct.h"

// defined in "tracking.h"
typedef struct track track_t;
typedef track_t* vec_track_t;

void _features_RoI_write(FILE* f, const int frame, const uint32_t* RoI_id, const uint32_t* RoI_xmin,
                         const uint32_t* RoI_xmax, const uint32_t* RoI_ymin, const uint32_t* RoI_ymax,
                         const uint32_t* RoI_S, const uint32_t* RoI_Sx, const uint32_t* RoI_Sy, const float* RoI_x,
                         const float* RoI_y, const uint32_t* RoI_magnitude, const size_t n_RoI,
                         const vec_track_t track_array, const unsigned age);
void features_RoI_write(FILE* f, const int frame, const RoI_basic_t* RoI_basic_array, const RoI_misc_t* RoI_misc_array,
                        const vec_track_t track_array, const unsigned age);
void _features_RoI0_RoI1_write(FILE* f, const int prev_frame, const int cur_frame, const uint32_t* RoI0_id,
                               const uint32_t* RoI0_xmin, const uint32_t* RoI0_xmax, const uint32_t* RoI0_ymin,
                               const uint32_t* RoI0_ymax, const uint32_t* RoI0_S, const uint32_t* RoI0_Sx,
                               const uint32_t* RoI0_Sy, const float* RoI0_x, const float* RoI0_y,
                               const uint32_t* RoI0_magnitude, const size_t n_RoI0, const uint32_t* RoI1_id,
                               const uint32_t* RoI1_xmin, const uint32_t* RoI1_xmax, const uint32_t* RoI1_ymin,
                               const uint32_t* RoI1_ymax, const uint32_t* RoI1_S, const uint32_t* RoI1_Sx,
                               const uint32_t* RoI1_Sy, const float* RoI1_x, const float* RoI1_y,
                               const uint32_t* RoI1_magnitude, const size_t n_RoI1, const vec_track_t track_array);
void features_RoI0_RoI1_write(FILE* f, const int prev_frame, const int cur_frame, const RoI_basic_t* RoI_basic_array0,
                              const RoI_misc_t* RoI_misc_array0, const RoI_basic_t* RoI_basic_array1,
                              const RoI_misc_t* RoI_misc_array1, const vec_track_t track_array);
