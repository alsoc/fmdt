#pragma once

#include "fmdt/tracking/tracking_struct.h"

tracking_data_t* tracking_alloc_data(const size_t max_history_size, const size_t max_ROI_size);
void tracking_init_data(tracking_data_t* tracking_data);
void tracking_free_data(tracking_data_t* tracking_data);

void _tracking_perform(tracking_data_t* tracking_data, const uint32_t* ROI_id, const uint32_t* ROI_xmin,
                       const uint32_t* ROI_xmax, const uint32_t* ROI_ymin, const uint32_t* ROI_ymax,
                       const uint32_t* ROI_S, const float* ROI_x, const float* ROI_y, const float* ROI_error,
                       const uint32_t* ROI_prev_id, const uint32_t* ROI_magnitude, const size_t n_ROI1,
                       vec_BB_t** BB_array, const size_t frame, const motion_t* motion_est, const size_t r_extrapol,
                       const float angle_max, const float diff_dev, const int track_all, const size_t fra_star_min,
                       const size_t fra_meteor_min, const size_t fra_meteor_max, const int magnitude,
                       const uint8_t extrapol_order_max, const float min_extrapol_ratio_S);
void tracking_perform(tracking_data_t* tracking_data, const ROI_t* ROI_array, vec_BB_t** BB_array, size_t frame,
                      const motion_t* motion_est, const size_t r_extrapol, const float angle_max, const float diff_dev,
                      const int track_all, const size_t fra_star_min, const size_t fra_meteor_min,
                      const size_t fra_meteor_max, const int magnitude, const uint8_t extrapol_order_max,
                      const float min_extrapol_ratio_S);
