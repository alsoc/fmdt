/*!
 * \file
 * \brief Input / output functions for Regions of Interest (RoIs).
 */

#pragma once

#include <stdio.h>

#include "fmdt/features/features_struct.h"

// defined in "tracking.h"
typedef struct track track_t;
typedef track_t* vec_track_t;

void _features_RoIs_write(FILE* f, const int frame, const uint32_t* RoIs_id, const uint32_t* RoIs_xmin,
                         const uint32_t* RoIs_xmax, const uint32_t* RoIs_ymin, const uint32_t* RoIs_ymax,
                         const uint32_t* RoIs_S, const uint32_t* RoIs_Sx, const uint32_t* RoIs_Sy, const float* RoIs_x,
                         const float* RoIs_y, const uint32_t* RoIs_magnitude, const size_t n_RoI,
                         const vec_track_t track_array, const unsigned age);

void features_RoIs_write(FILE* f, const int frame, const RoIs_basic_t* RoIs_basic_array, const RoIs_misc_t* RoIs_misc_array,
                         const vec_track_t track_array, const unsigned age);

void _features_RoIs0_RoIs1_write(FILE* f, const int prev_frame, const int cur_frame, const uint32_t* RoI0_id,
                                 const uint32_t* RoI0_xmin, const uint32_t* RoI0_xmax, const uint32_t* RoI0_ymin,
                                 const uint32_t* RoI0_ymax, const uint32_t* RoI0_S, const uint32_t* RoI0_Sx,
                                 const uint32_t* RoI0_Sy, const float* RoI0_x, const float* RoI0_y,
                                 const uint32_t* RoI0_magnitude, const size_t n_RoI0, const uint32_t* RoI1_id,
                                 const uint32_t* RoI1_xmin, const uint32_t* RoI1_xmax, const uint32_t* RoI1_ymin,
                                 const uint32_t* RoI1_ymax, const uint32_t* RoI1_S, const uint32_t* RoI1_Sx,
                                 const uint32_t* RoI1_Sy, const float* RoI1_x, const float* RoI1_y,
                                 const uint32_t* RoI1_magnitude, const size_t n_RoI1, const vec_track_t track_array);

void features_RoIs0_RoIs1_write(FILE* f, const int prev_frame, const int cur_frame, const RoIs_basic_t* RoIs_basic_array0,
                                const RoIs_misc_t* RoIs_misc_array0, const RoIs_basic_t* RoIs_basic_array1,
                                const RoIs_misc_t* RoIs_misc_array1, const vec_track_t track_array);
