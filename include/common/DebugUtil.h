/**
 * Copyright (c) 2017-2018, Arthur Hennequin, LIP6, UPMC, CNRS
 * Copyright (c) 2020-2020, Lionel Lacassagne, LIP6 Sorbonne University, CNRS
 * Copyright (c) 2020-2021, MILLET Maxime, LIP6 Sorbonne University
 * Copyright (c) 2020-2021, Clara CIOCAN, LIP6 Sorbonne University
 * Copyright (c) 2020-2021, Mathuran KANDEEPAN, LIP6 Sorbonne University
 */

#ifndef __DEBUGUTIL_H__
#define __DEBUGUTIL_H__

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Args.h"
#include "CCL.h"
#include "Features.h"
#include "Threshold.h"
#include "Tracking.h"
#include "Video.h"
#include "macro_debug.h"
#include "nrutil.h"

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

void printTabBB(BB_t** tabBB, int n);
void print_tracks(track_t* tracks, int last);
void print_tracks2(FILE* f, track_t* tracks, int n);
void printStats(ROI_t* stats, int n);
void printBuffer(ROIx2_t* buffer, int n);

void parseStats(const char* filename, ROI_t* stats, int* n);
void parse_tracks(const char* filename, track_t* tracks, int* n);

void saveMotion(const char* filename, double theta, double tx, double ty, int frame);
void saveStats(const char* filename, ROI_t* stats, int n, track_t* tracks);
void save_tracks(const char* filename, track_t* tracks, int n);
void saveTabBB(const char* filename, BB_t** tabBB, track_t* tracks, int n, int track_all);
void saveBoundingBox(const char* filename, uint16 rx, uint16 ry, uint16 bb_x, uint16 bb_y, int frame);
void saveAsso(const char* filename, uint32** Nearest, float32** distances, int nc0, ROI_t* stats);
void saveAsso_VT(const char* filename, int nc0, ROI_t* stats, int frame);
void saveConflicts(const char* filename, uint32* conflicts, uint32** Nearest, float32** distances, int n_asso,
                   int n_conflict);
void saveAssoConflicts(const char* filename, int frame, uint32* conflicts, uint32** Nearest, float32** distances,
                       int n_asso, int n_conflict, ROI_t* stats0, ROI_t* stats1, track_t* tracks, int n_tracks);
void saveError(const char* filename, ROI_t* stats, int n);
void saveErrorMoy(const char* filename, double errMoy, double eType);
void saveMotionExtraction(char* filename, ROI_t* stats0, ROI_t* stats1, int nc0, double theta, double tx,
                          double ty, int frame);

int sursegmentation(ROI_t* stats, int n);
void plot_bounding_box(rgb8** img, int ymin, int ymax, int xmin, int xmax, int border, rgb8 color);

// Analysing
void filter_speed_binarize(uint32** in, int i0, int i1, int j0, int j1, uint8** out, ROI_t* stats);

// Image
rgb8** load_image_color(const char* filename, long* i0, long* i1, long* j0, long* j1);
void saveFrame_threshold(const char* filename, uint8** I0, uint8** I1, int i0, int i1, int j0, int j1);
void saveFrame_quad(const char* filename, uint8** I0, uint8** I1, uint32** I2, uint32** I3, int nbLabel,
                    ROI_t* stats, int i0, int i1, int j0, int j1);
void saveFrame_ui32matrix(const char* filename, uint32** I, int i0, int i1, int j0, int j1);
void saveFrame_ui8matrix(const char* filename, uint8** I, int i0, int i1, int j0, int j1);
void saveMax(const char* filename, uint8** I, int i0, int i1, int j0, int j1);
void saveFrame_quad_hysteresis(const char* filename, uint8** I0, uint32** SH, uint32** SB, uint32** Y, int i0, int i1,
                               int j0, int j1);
void saveVideoFrame_tracking(const char* filename, uint8** I, track_t* tracks, int tracks_nb, int i0, int i1, int j0,
                             int j1);
void saveFrame_tracking(const char* filename, uint8** I, track_t* tracks, int tracks_nb, int i0, int i1, int j0, int j1);

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
void WritePNMrow(uint8* line, int width, FILE* file); // defined in NRC but private...

#endif //__DEBUGUTIL_H__
