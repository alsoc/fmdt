/**
 * Copyright (c) 2017-2018, Arthur Hennequin, LIP6, UPMC, CNRS
 * Copyright (c) 2020-2020, Lionel Lacassagne, LIP6 Sorbonne University, CNRS
 * Copyright (c) 2020-2021, MILLET Maxime, LIP6 Sorbonne University
 * Copyright (c) 2020-2021, Clara CIOCAN, LIP6 Sorbonne University
 * Copyright (c) 2020-2021, Mathuran KANDEEPAN, LIP6 Sorbonne University
 */

#ifndef __DEBUG_UTILS_H__
#define __DEBUG_UTILS_H__

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <nrutil.h>

#include "CCL.h"
#include "features.h"
#include "threshold.h"
#include "tracking.h"
#include "video.h"
#include "macro_debug.h"

// Timing
#define BENCH(X, n, cpp)                                                                                               \
    do {                                                                                                               \
        struct_timespec t0, t1;                                                                                        \
        double dt;                                                                                                     \
        int iter = 100;                                                                                                \
        t0 = getCycles();                                                                                              \
        for (int k = 0; k < iter; k++) {                                                                               \
            X;                                                                                                         \
        }                                                                                                              \
        t1 = getCycles();                                                                                              \
        dt = diff_ns(t0, t1);                                                                                          \
        cpp = dt / (iter * n * n);                                                                                     \
    } while (0)
#define BENCH_TOTAL(X, n, cpp)                                                                                         \
    do {                                                                                                               \
        struct_timespec t0, t1;                                                                                        \
        double dt;                                                                                                     \
        int iter = 1;                                                                                                  \
        t0 = getCycles();                                                                                              \
        for (int k = 0; k < iter; k++) {                                                                               \
            X;                                                                                                         \
        }                                                                                                              \
        t1 = getCycles();                                                                                              \
        dt = diff_ns(t0, t1);                                                                                          \
        cpp = dt / (iter * n);                                                                                         \
    } while (0)

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) < (b)) ? (b) : (a))
#define clamp(x, a, b) min(max(x, a), b)
#define SWAP_STATS(dest, src, n_src)                                                                                   \
    for (int i = 1; i <= n_src; i++) {                                                                                 \
        dest[i] = src[i];                                                                                              \
    }
#define SWAP_UI8(a, b)                                                                                                 \
    {                                                                                                                  \
        uint8** tmp = a;                                                                                               \
        a = b;                                                                                                         \
        b = tmp;                                                                                                       \
    }

void save_bounding_box(const char* filename, uint16 rx, uint16 ry, uint16 bb_x, uint16 bb_y, int frame);
void plot_bounding_box(rgb8** img, int ymin, int ymax, int xmin, int xmax, int border, rgb8 color);

// Analysing
void filter_speed_binarize(uint32** in, int i0, int i1, int j0, int j1, uint8** out, ROI_t* stats);

// Image
rgb8** load_image_color(const char* filename, long* i0, long* i1, long* j0, long* j1);
void save_frame_threshold(const char* filename, uint8** I0, uint8** I1, int i0, int i1, int j0, int j1);
void save_frame_quad(const char* filename, uint8** I0, uint8** I1, uint32** I2, uint32** I3, int nbLabel,
                     ROI_t* stats, int i0, int i1, int j0, int j1);
void save_frame_ui32matrix(const char* filename, uint32** I, int i0, int i1, int j0, int j1);
void save_frame_ui8matrix(const char* filename, uint8** I, int i0, int i1, int j0, int j1);
void save_max(const char* filename, uint8** I, int i0, int i1, int j0, int j1);
void save_frame_quad_hysteresis(const char* filename, uint8** I0, uint32** SH, uint32** SB, uint32** Y, int i0, int i1,
                                int j0, int j1);
void save_video_frame_tracking(const char* filename, uint8** I, track_t* tracks, int tracks_nb, int i0, int i1, int j0,
                               int j1);
void save_frame_tracking(const char* filename, uint8** I, track_t* tracks, int tracks_nb, int i0, int i1, int j0,
                         int j1);

void create_folder(char* folder_path);

void copy_ui8matrix_ui8matrix(uint8** X, int i0, int i1, int j0, int j1, uint8** Y);
void convert_ui8vector_ui32vector(uint8* X, long nl, long nh, uint32* Y);
void convert_ui8matrix_ui32matrix(uint8** X, int nrl, int nrh, int ncl, int nch, uint32** Y);
void write_PNM_row(uint8* line, int width, FILE* file);

#endif //__DEBUG_UTILS_H__
