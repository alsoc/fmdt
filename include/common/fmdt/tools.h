#pragma once

#include <stdint.h>

#include "fmdt/tracking.h"

// rgb8 is defined in NRC2 (nrtype.h), but adding "#include <nrtype.h>" here is overkill
typedef struct { uint8_t r; uint8_t g; uint8_t b; } rgb8_t;

rgb8_t tools_get_color(enum color_e color);
void tools_convert_img_grayscale_to_rgb(const uint8_t** I, rgb8_t** I_bb, int i0, int i1, int j0, int j1);
#ifdef OPENCV_LINK
void tools_draw_text(rgb8_t** img, const int img_width, const int img_height, const BB_t* BB_list,
                     const enum color_e* BB_list_color, const int n_BB, int validation, int show_id);
#endif
void tools_draw_BB(rgb8_t** I_bb, const BB_t* BB_list, const enum color_e* BB_list_color, int n_BB, int w, int h);
void tools_save_frame(const char* filename, const rgb8_t** I_bb, int w, int h);
void tools_plot_bounding_box(rgb8_t** img, int ymin, int ymax, int xmin, int xmax, int border, rgb8_t color,
                             int is_dashed);
void _tools_save_frame_ui32matrix(const char* filename, const uint32_t** I, int i0, int i1, int j0, int j1,
                                  uint8_t** img);
void tools_save_frame_ui32matrix(const char* filename, const uint32_t** I, int i0, int i1, int j0, int j1);
void tools_save_frame_ui8matrix(const char* filename, const uint8_t** I, int i0, int i1, int j0, int j1);
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
int tools_is_dir(const char *path);
