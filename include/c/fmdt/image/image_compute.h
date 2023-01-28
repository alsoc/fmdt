#pragma once

#include "fmdt/features/features_struct.h"
#include "fmdt/tracking/tracking_struct.h"
#include "fmdt/image/image_struct.h"

rgb8_t image_get_color(enum color_e color);

img_data_t* image_gs_alloc(const size_t img_width, const size_t img_height);
void _image_gs_draw_labels(img_data_t* img_data, const uint32_t** labels, const uint32_t* ROI_id,
                           const uint32_t* ROI_xmax, const uint32_t* ROI_ymin, const uint32_t* ROI_ymax,
                           const size_t n_ROI, const uint8_t show_id);
void image_gs_draw_labels(img_data_t* img_data, const uint32_t** labels, const ROI_t* ROI_array,
                          const uint8_t show_id);
uint8_t* image_gs_get_pixels(img_data_t* img_data);
uint8_t** image_gs_get_pixels_2d(img_data_t* img_data);
void image_gs_free(img_data_t* img_data);

img_data_t* image_color_alloc(const size_t img_width, const size_t img_height);
void image_color_draw_BB(img_data_t* img_data, const uint8_t** img, const BB_t* BB_list,
                         const enum color_e* BB_list_color, const size_t n_BB, const uint8_t show_id,
                         const uint8_t is_gt);
rgb8_t* image_color_get_pixels(img_data_t* img_data);
rgb8_t** image_color_get_pixels_2d(img_data_t* img_data);
void image_color_free(img_data_t* img_data);
