#pragma once

#include <stdint.h>

#include "fmdt/tracking.h"

// rgb8 is defined in NRC2 (nrtype.h), but adding "#include <nrtype.h>" here is overkill
typedef struct { uint8_t r; uint8_t g; uint8_t b; } rgb8_t;

typedef struct BB_coord_t {
    int track_id;
    int xmin;
    int xmax;
    int ymin;
    int ymax;
    enum color_e color;
} BB_coord_t;

rgb8_t tools_get_color(enum color_e color);
void tools_convert_img_grayscale_to_rgb(const uint8_t** I, rgb8_t** I_bb, int i0, int i1, int j0, int j1);
#ifdef OPENCV_LINK
void tools_draw_text(rgb8_t** img, const int img_width, const int img_height, const BB_coord_t* listBB, const int nBB,
                     int validation, int show_id);
#endif
void tools_max3_ui8matrix(const uint8_t** X, uint8_t**Y, int i0, int i1, int j0, int j1);
void tools_max_reduce(uint8_t** M, int i0, int i1, int j0, int j1, uint8_t** I);
void tools_save_frame_from_ROI(const char* filename, const uint8_t** in, const int i0, const int i1,
                               const int j0, const int j1, ROI_t* ROI_array);

void _tools_save_frame_from_ROI(const char* filename, const uint8_t** in, const int i0, const int i1,
                               const int j0, const int j1, const uint16_t* ROI_xmin,
                               const uint16_t* ROI_xmax, const uint16_t* ROI_ymin, const uint16_t* ROI_ymax,
                               uint32_t* ROI_S, const size_t n_ROI);

void tools_draw_BB(rgb8_t** I_bb, const BB_coord_t* listBB, int n_BB, int w, int h);
void tools_save_frame(const char* filename, const rgb8_t** I_bb, int w, int h);
// void tools_save_bounding_box(const char* filename, uint16 rx, uint16 ry, uint16 bb_x, uint16 bb_y, int frame);
void tools_plot_bounding_box(rgb8_t** img, int ymin, int ymax, int xmin, int xmax, int border, rgb8_t color);
// Analysing
// void tools_filter_speed_binarize(uint32_t** in, int i0, int i1, int j0, int j1, uint8_t** out, ROI_t* stats);
// Image
// rgb8_t** tools_load_image_color(const char* filename, long* i0, long* i1, long* j0, long* j1);
// void tools_save_frame_threshold(const char* filename, uint8_t** I0, uint8_t** I1, int i0, int i1, int j0, int j1);
// void tools_save_frame_quad(const char* filename, uint8_t** I0, uint8_t** I1, uint32_t** I2, uint32_t** I3,
//                            int nbLabel, ROI_t* stats, int i0, int i1, int j0, int j1);
void tools_save_frame_ui32matrix(const char* filename, const uint32_t** I, int i0, int i1, int j0, int j1);
void tools_save_frame_ui8matrix(const char* filename, const uint8_t** I, int i0, int i1, int j0, int j1);
// void tools_save_max(const char* filename, uint8_t** I, int i0, int i1, int j0, int j1);
// void tools_save_frame_quad_hysteresis(const char* filename, uint8_t** I0, uint32_t** SH, uint32_t** SB, uint32_t** Y,
//                                       int i0, int i1, int j0, int j1);
void tools_create_folder(const char* folder_path);
void tools_copy_ui8matrix_ui8matrix(const uint8_t** X, const int i0, const int i1, const int j0, const int j1,
                                    uint8_t** Y);
// void tools_convert_ui8vector_ui32vector(const uint8_t* X, const long nl, const long nh, uint32_t* Y);
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
