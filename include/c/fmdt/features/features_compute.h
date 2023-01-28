#pragma once

#include "fmdt/features/features_struct.h"

ROI_t* features_alloc_ROI(const size_t max_size);
void features_init_ROI(ROI_t* ROI_array);
void features_free_ROI(ROI_t* ROI_array);

ROI_basic_t* features_alloc_ROI_basic(const size_t max_size, uint32_t* ROI_id);
void features_init_ROI_basic(ROI_basic_t* ROI_basic_array, const uint8_t init_id);
void features_free_ROI_basic(ROI_basic_t* ROI_basic_array, const uint8_t free_id);

ROI_asso_t* features_alloc_ROI_asso(const size_t max_size, uint32_t* ROI_id);
void features_init_ROI_asso(ROI_asso_t* ROI_asso_array, const uint8_t init_id);
void features_free_ROI_asso(ROI_asso_t* ROI_asso_array, const uint8_t free_id);

ROI_motion_t* features_alloc_ROI_motion(const size_t max_size, uint32_t* ROI_id);
void features_init_ROI_motion(ROI_motion_t* ROI_motion_array, const uint8_t init_id);
void features_free_ROI_motion(ROI_motion_t* ROI_motion_array, const uint8_t free_id);

ROI_misc_t* features_alloc_ROI_misc(const size_t max_size, uint32_t* ROI_id);
void features_init_ROI_misc(ROI_misc_t* ROI_misc_array, const uint8_t init_id);
void features_free_ROI_misc(ROI_misc_t* ROI_misc_array, const uint8_t free_id);

void _features_extract(const uint32_t** img, const int i0, const int i1, const int j0, const int j1, uint32_t* ROI_id,
                       uint32_t* ROI_xmin, uint32_t* ROI_xmax, uint32_t* ROI_ymin, uint32_t* ROI_ymax, uint32_t* ROI_S,
                       uint32_t* ROI_Sx, uint32_t* ROI_Sy, float* ROI_x, float* ROI_y, const size_t n_ROI);
void features_extract(const uint32_t** img, const int i0, const int i1, const int j0, const int j1, const size_t n_ROI,
                      ROI_basic_t* ROI_basic_array);
// void features_filter_surface(ROI_t* ROI_array, uint32_t** img, uint32_t threshold_min, uint32_t threshold_max);
void _features_merge_CCL_HI_v2(const uint32_t** in_labels, const uint8_t** img_HI, uint32_t** out_labels, const int i0,
                               const int i1, const int j0, const int j1, uint32_t* ROI_id, const uint32_t* ROI_xmin,
                               const uint32_t* ROI_xmax, const uint32_t* ROI_ymin, const uint32_t* ROI_ymax,
                               const uint32_t* ROI_S, const size_t n_ROI, const uint32_t S_min, const uint32_t S_max);
void features_merge_CCL_HI_v2(const uint32_t** in_labels, const uint8_t** img_HI, uint32_t** out_labels, const int i0,
                              const int i1, const int j0, const int j1, ROI_basic_t* ROI_basic_array,
                              const uint32_t S_min, const uint32_t S_max);
size_t _features_shrink_ROI_array(const uint32_t* ROI_src_id, const uint32_t* ROI_src_xmin,
                                  const uint32_t* ROI_src_xmax, const uint32_t* ROI_src_ymin,
                                  const uint32_t* ROI_src_ymax, const uint32_t* ROI_src_S, const uint32_t* ROI_src_Sx,
                                  const uint32_t* ROI_src_Sy, const float* ROI_src_x, const float* ROI_src_y,
                                  const size_t n_ROI_src, uint32_t* ROI_dest_id, uint32_t* ROI_dest_xmin,
                                  uint32_t* ROI_dest_xmax, uint32_t* ROI_dest_ymin, uint32_t* ROI_dest_ymax,
                                  uint32_t* ROI_dest_S, uint32_t* ROI_dest_Sx, uint32_t* ROI_dest_Sy, float* ROI_dest_x,
                                  float* ROI_dest_y);
void features_shrink_ROI_array(const ROI_basic_t* ROI_basic_array_src, ROI_basic_t* ROI_basic_array_dest);
void _features_compute_magnitude(const uint8_t** img, const uint32_t img_width, const uint32_t img_height,
                                 const uint32_t** labels, const uint32_t* ROI_xmin, const uint32_t* ROI_xmax,
                                 const uint32_t* ROI_ymin, const uint32_t* ROI_ymax, const uint32_t* ROI_S,
                                 uint32_t* ROI_magnitude, const size_t n_ROI);
void features_compute_magnitude(const uint8_t** img, const uint32_t img_width, const uint32_t img_height,
                                const uint32_t** labels, const ROI_basic_t* ROI_basic_array, ROI_misc_t* ROI_misc_array);
