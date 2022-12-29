#pragma once

#include <stdint.h>

#include "fmdt/tracking.h"

// rgb8 is defined in NRC2 (nrtype.h), but adding "#include <nrtype.h>" here is overkill
typedef struct { uint8_t r; uint8_t g; uint8_t b; } rgb8_t;

rgb8_t tools_get_color(enum color_e color);
void tools_create_folder(const char* folder_path);
void tools_copy_ui8matrix_ui8matrix(const uint8_t** X, const int i0, const int i1, const int j0, const int j1,
                                    uint8_t** Y);
void tools_convert_ui8matrix_ui32matrix(const uint8_t** X, const int nrl, const int nrh, const int ncl, const int nch,
                                        uint32_t** Y);
void tools_write_PNM_row(const uint8_t* line, const int width, FILE* file);

void tools_linear_2d_nrc_ui8matrix(const uint8_t* X, const int i0, const int i1, const int j0, const int j1,
                                   const uint8_t** Y);
void tools_linear_2d_nrc_ui32matrix(const uint32_t* X, const int i0, const int i1, const int j0, const int j1, 
                                    const uint32_t** Y);
void tools_linear_2d_nrc_f32matrix(const float* X, const int i0, const int i1, const int j0, const int j1,
                                   const float** Y);
void tools_linear_2d_nrc_rgb8matrix(const rgb8_t* X, const int i0, const int i1, const int j0, const int j1,
                                    const rgb8_t** Y);
int tools_is_dir(const char *path);

// ====================================================================================================================
// ====================================================================================================================

typedef struct img_data_t {
    char ext[32];
    char path[1024];
    uint8_t show_id;
    size_t height;
    size_t width;
    void* pixels;
    void* container_2d;
} img_data_t;

img_data_t* tools_gray_img_alloc1(const size_t img_width, const size_t img_height, const char* path, const char* ext,
                                  const uint8_t show_id);
img_data_t* tools_gray_img_alloc2(const size_t img_width, const size_t img_height, const char* ext,
                                  const uint8_t show_id);
void _tools_gray_img_draw_labels(img_data_t* img_data, const uint32_t** labels, const uint32_t* ROI_id,
                                 const uint32_t* ROI_xmax, const uint32_t* ROI_ymin, const uint32_t* ROI_ymax,
                                 const size_t n_ROI);
void tools_gray_img_draw_labels(img_data_t* img_data, const uint32_t** labels, const ROI_t* ROI_array);
uint8_t* tools_gray_img_get_pixels(img_data_t* img_data);
uint8_t** tools_gray_img_get_pixels_2d(img_data_t* img_data);
void tools_gray_img_write1(img_data_t* img_data, const size_t frame);
void tools_gray_img_write2(img_data_t* img_data, const char* filename);
void tools_gray_img_free(img_data_t* img_data);

img_data_t* tools_color_img_alloc1(const size_t img_width, const size_t img_height, const char* path, const char* ext,
                                   const uint8_t show_id);
img_data_t* tools_color_img_alloc2(const size_t img_width, const size_t img_height, const char* ext,
                                   const uint8_t show_id);
void tools_color_img_draw_BB(img_data_t* img_data, const uint8_t** img, const BB_t* BB_list,
                             const enum color_e* BB_list_color, const size_t n_BB, const uint8_t is_gt);
rgb8_t* tools_color_img_get_pixels(img_data_t* img_data);
rgb8_t** tools_color_img_get_pixels_2d(img_data_t* img_data);
void tools_color_img_write1(img_data_t* img_data, const size_t frame);
void tools_color_img_write2(img_data_t* img_data, const char* filename);
void tools_color_img_free(img_data_t* img_data);
