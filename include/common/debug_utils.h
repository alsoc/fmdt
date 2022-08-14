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

void print_array_BB(BB_t** tabBB, int n);
void print_tracks(track_t* tracks, int last);
void print_tracks2(FILE* f, track_t* tracks, int n);
void print_stats(ROI_t* stats, int n);
void print_buffer(ROIx2_t* buffer, int n);

void parse_stats(const char* filename, ROI_t* stats, int* n);
void parse_tracks(const char* filename, track_t* tracks, int* n);

void save_motion(const char* filename, double theta, double tx, double ty, int frame);
void saveStats(const char* filename, ROI_t* stats, int n, track_t* tracks);
void save_tracks(const char* filename, track_t* tracks, int n);
void save_array_BB(const char* filename, BB_t** tabBB, track_t* tracks, int n, int track_all);
void save_bounding_box(const char* filename, uint16 rx, uint16 ry, uint16 bb_x, uint16 bb_y, int frame);
void save_asso(const char* filename, uint32** Nearest, float32** distances, int nc0, ROI_t* stats);
void save_asso_VT(const char* filename, int nc0, ROI_t* stats, int frame);
void save_conflicts(const char* filename, uint32* conflicts, uint32** Nearest, float32** distances, int n_asso,
                    int n_conflict);
void save_asso_conflicts(const char* filename, int frame, uint32* conflicts, uint32** Nearest, float32** distances,
                         int n_asso, int n_conflict, ROI_t* stats0, ROI_t* stats1, track_t* tracks, int n_tracks);
void save_error(const char* filename, ROI_t* stats, int n);
void save_error_moy(const char* filename, double errMoy, double eType);
void save_motion_extraction(char* filename, ROI_t* stats0, ROI_t* stats1, int nc0, double theta, double tx,
                            double ty, int frame);

int sursegmentation(ROI_t* stats, int n);
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
void saveMax(const char* filename, uint8** I, int i0, int i1, int j0, int j1);
void save_frame_quad_hysteresis(const char* filename, uint8** I0, uint32** SH, uint32** SB, uint32** Y, int i0, int i1,
                                int j0, int j1);
void save_video_frame_tracking(const char* filename, uint8** I, track_t* tracks, int tracks_nb, int i0, int i1, int j0,
                               int j1);
void save_frame_tracking(const char* filename, uint8** I, track_t* tracks, int tracks_nb, int i0, int i1, int j0,
                         int j1);

void split_path_file(char** p, char** f, char* pf);
void get_data_from_tracks_path(char* path, int* light_min, int* light_max, char** filename);
void get_bouding_box_path_from_tracks_path(char* path_tracks);

void create_debug_dir(char* output_stats);
void create_frames_dir(char* dest_path);
void create_video_dir(char* dest_path);
void create_tracks_dir(char* output_dest);
void create_debug_files(int frame);
void create_frames_files(int frame);
void create_videos_files(char* filename);
void create_bb_file(char* filename);

void copy_ui8matrix_ui8matrix(uint8** X, int i0, int i1, int j0, int j1, uint8** Y);
void convert_ui8vector_ui32vector(uint8* X, long nl, long nh, uint32* Y);
void convert_ui8matrix_ui32matrix(uint8** X, int nrl, int nrh, int ncl, int nch, uint32** Y);
void write_PNM_row(uint8* line, int width, FILE* file); // defined in NRC but private...

#endif //__DEBUG_UTILS_H__
