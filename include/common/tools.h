/**
 * Copyright (c) 2017-2018, Arthur Hennequin, LIP6, UPMC, CNRS
 * Copyright (c) 2020-2020, Lionel Lacassagne, LIP6 Sorbonne University, CNRS
 * Copyright (c) 2020-2021, MILLET Maxime, LIP6 Sorbonne University
 * Copyright (c) 2020-2021, Clara CIOCAN, LIP6 Sorbonne University
 * Copyright (c) 2020-2021, Mathuran KANDEEPAN, LIP6 Sorbonne University
 */

#pragma once

#include <stdint.h>

#include "tracking.h"

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
                     int validation, int show_ids);
#endif
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
void tools_save_frame_ui32matrix(const char* filename, uint32_t** I, int i0, int i1, int j0, int j1);
// void tools_save_frame_ui8matrix(const char* filename, uint8_t** I, int i0, int i1, int j0, int j1);
// void tools_save_max(const char* filename, uint8_t** I, int i0, int i1, int j0, int j1);
// void tools_save_frame_quad_hysteresis(const char* filename, uint8_t** I0, uint32_t** SH, uint32_t** SB, uint32_t** Y,
//                                       int i0, int i1, int j0, int j1);
// void tools_save_video_frame_tracking(const char* filename, uint8_t** I, track_t* tracks, int tracks_nb, int i0, int i1,
//                                      int j0, int j1);
// void tools_save_frame_tracking(const char* filename, uint8_t** I, track_t* tracks, int tracks_nb, int i0, int i1,
//                                int j0, int j1);

void tools_create_folder(char* folder_path);

void tools_copy_ui8matrix_ui8matrix(uint8_t** X, int i0, int i1, int j0, int j1, uint8_t** Y);
// void tools_convert_ui8vector_ui32vector(uint8_t* X, long nl, long nh, uint32_t* Y);
void tools_convert_ui8matrix_ui32matrix(uint8_t** X, int nrl, int nrh, int ncl, int nch, uint32_t** Y);
void tools_write_PNM_row(uint8_t* line, int width, FILE* file);
