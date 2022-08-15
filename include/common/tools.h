/**
 * Copyright (c) 2017-2018, Arthur Hennequin, LIP6, UPMC, CNRS
 * Copyright (c) 2020-2020, Lionel Lacassagne, LIP6 Sorbonne University, CNRS
 * Copyright (c) 2020-2021, MILLET Maxime, LIP6 Sorbonne University
 * Copyright (c) 2020-2021, Clara CIOCAN, LIP6 Sorbonne University
 * Copyright (c) 2020-2021, Mathuran KANDEEPAN, LIP6 Sorbonne University
 */

#ifndef __TOOLS_H__
#define __TOOLS_H__

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <nrutil.h>
#include <nrc2.h>

#include "CCL.h"
#include "features.h"
#include "threshold.h"
#include "tracking.h"
#include "video.h"
#include "macros.h"

typedef struct BB_coord_t {
    int track_id;
    int xmin;
    int xmax;
    int ymin;
    int ymax;
    enum color_e color;
} BB_coord_t;

rgb8 tools_get_color(enum color_e color);
void tools_convert_img_grayscale_to_rgb(const uint8** I, rgb8** I_bb, int i0, int i1, int j0, int j1);
#ifdef OPENCV_LINK
void tools_draw_text(rgb8** img, const int img_width, const int img_height, const BB_coord_t* listBB, const int nBB,
                     int validation, int show_ids);
#endif
void tools_draw_BB(rgb8** I_bb, const BB_coord_t* listBB, int n_BB);
void tools_save_frame(const char* filename, const rgb8** I_bb, int w, int h);

// void tools_save_bounding_box(const char* filename, uint16 rx, uint16 ry, uint16 bb_x, uint16 bb_y, int frame);
void tools_plot_bounding_box(rgb8** img, int ymin, int ymax, int xmin, int xmax, int border, rgb8 color);

// Analysing
// void tools_filter_speed_binarize(uint32** in, int i0, int i1, int j0, int j1, uint8** out, ROI_t* stats);

// Image
// rgb8** tools_load_image_color(const char* filename, long* i0, long* i1, long* j0, long* j1);
// void tools_save_frame_threshold(const char* filename, uint8** I0, uint8** I1, int i0, int i1, int j0, int j1);
// void tools_save_frame_quad(const char* filename, uint8** I0, uint8** I1, uint32** I2, uint32** I3, int nbLabel,
//                            ROI_t* stats, int i0, int i1, int j0, int j1);
void tools_save_frame_ui32matrix(const char* filename, uint32** I, int i0, int i1, int j0, int j1);
// void tools_save_frame_ui8matrix(const char* filename, uint8** I, int i0, int i1, int j0, int j1);
// void tools_save_max(const char* filename, uint8** I, int i0, int i1, int j0, int j1);
// void tools_save_frame_quad_hysteresis(const char* filename, uint8** I0, uint32** SH, uint32** SB, uint32** Y, int i0,
//                                       int i1, int j0, int j1);
// void tools_save_video_frame_tracking(const char* filename, uint8** I, track_t* tracks, int tracks_nb, int i0, int i1,
//                                      int j0, int j1);
// void tools_save_frame_tracking(const char* filename, uint8** I, track_t* tracks, int tracks_nb, int i0, int i1,
//                                int j0, int j1);

void tools_create_folder(char* folder_path);

void tools_copy_ui8matrix_ui8matrix(uint8** X, int i0, int i1, int j0, int j1, uint8** Y);
// void tools_convert_ui8vector_ui32vector(uint8* X, long nl, long nh, uint32* Y);
void tools_convert_ui8matrix_ui32matrix(uint8** X, int nrl, int nrh, int ncl, int nch, uint32** Y);
void tools_write_PNM_row(uint8* line, int width, FILE* file);

#endif //__TOOLS_H__
