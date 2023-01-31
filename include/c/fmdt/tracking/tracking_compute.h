#pragma once

#include "fmdt/features/features_struct.h"
#include "fmdt/tracking/tracking_struct.h"

tracking_data_t* tracking_alloc_data(const size_t max_history_size, const size_t max_RoIs_size);
void tracking_init_data(tracking_data_t* tracking_data);
void tracking_free_data(tracking_data_t* tracking_data);

void _tracking_perform(tracking_data_t* tracking_data, const uint32_t* RoIs_id, const uint32_t* RoIs_xmin,
                       const uint32_t* RoIs_xmax, const uint32_t* RoIs_ymin, const uint32_t* RoIs_ymax,
                       const uint32_t* RoIs_S, const float* RoIs_x, const float* RoIs_y, const float* RoIs_error,
                       const uint32_t* RoIs_prev_id, const uint32_t* RoIs_magnitude, const size_t n_RoIs,
                       vec_BB_t** BBs, const size_t frame, const motion_t* motion_est, const size_t r_extrapol,
                       const float angle_max, const float diff_dev, const int track_all, const size_t fra_star_min,
                       const size_t fra_meteor_min, const size_t fra_meteor_max, const int magnitude,
                       const uint8_t extrapol_order_max, const float min_extrapol_ratio_S);
void tracking_perform(tracking_data_t* tracking_data, const RoIs_t* RoIs, vec_BB_t** BBs, size_t frame,
                      const motion_t* motion_est, const size_t r_extrapol, const float angle_max, const float diff_dev,
                      const int track_all, const size_t fra_star_min, const size_t fra_meteor_min,
                      const size_t fra_meteor_max, const int magnitude, const uint8_t extrapol_order_max,
                      const float min_extrapol_ratio_S);
