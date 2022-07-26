/**
 * Copyright (c) 2017-2018, Arthur Hennequin, LIP6, UPMC, CNRS
 * Copyright (c) 2020-2020, Lionel Lacassagne, LIP6 Sorbonne University, CNRS
 * Copyright (c) 2020-2021, MILLET Maxime, LIP6 Sorbonne University
 * Copyright (c) 2020-2021, Clara CIOCAN, LIP6 Sorbonne University
 * Copyright (c) 2020-2021, Mathuran KANDEEPAN, LIP6 Sorbonne University
 */
#include "nrutil.h"
#include "Args.h"
#include "Video.h"
#include "CCL.h"
#include "Features.h"
#include "Threshold.h"
#include "Tracking.h"
#include "macro_debug.h"

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdint.h>
#ifndef __DEBUGUTIL_H__
#define __DEBUGUTIL_H__


// Timing

#define BENCH(X, n, cpp)  do { struct_timespec t0, t1; double dt; int iter = 100; t0=getCycles();  for(int k=0; k<iter; k++) { X; } t1=getCycles(); dt=diff_ns(t0, t1);  cpp = dt/(iter*n*n);  } while(0)
#define BENCH_TOTAL(X, n, cpp)  do { struct_timespec t0, t1; double dt; int iter = 1; t0=getCycles();  for(int k=0; k<iter; k++) { X; } t1=getCycles(); dt=diff_ns(t0, t1); cpp = dt/(iter*n);  } while(0)

// #define BENCH(X, n, cpp)  do { double t0, t1, dt, tmin=1e20; int run = 10; int iter = 10; for(int r=0; r<run; r++) { t0=_rdtsc();  for(int k=0; k<iter; k++) { X; } t1=_rdtsc(); dt=t1-t0; dt /= (double)iter; if(dt<tmin) tmin = dt;} cpp = tmin/(n*n); } while(0)
// #define BENCH_TOTAL(X, n, cpp)  do { double t0, t1, dt, tmin=1e20; int run = 1; int iter = 1; for(int r=0; r<run; r++) { t0=_rdtsc();  for(int k=0; k<iter; k++) { X; } t1=_rdtsc(); dt=t1-t0; dt /= (double)iter; if(dt<tmin) tmin = dt;} cpp = tmin/(n); } while(0)


#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)<(b))?(b):(a))
#define clamp(x,a,b) min(max(x,a),b)
#define SWAP_STATS(dest, src, n_src) for( int i = 1; i <= n_src; i++){dest[i] = src[i];}
#define SWAP_UI8(a,b) {uint8 **tmp = a; a = b; b = tmp;}

void printTabBB(elemBB **tabBB, int n);
void printTracks(Track* tracks, int last);
void printStats(MeteorROI* stats, int n);
void printBuffer(Buf *buffer, int n);

void parseStats(const char*filename, MeteorROI* stats, int* n);
void parseTracks(const char*filename, Track* tracks, int* n);

void saveMotion(const char*filename, double theta, double tx, double ty, int frame);
void saveStats(const char*filename, MeteorROI* stats, int n);
void saveTracks(const char*filename, Track* tracks, int n);
void saveTabBB(const char *filename, elemBB **tabBB, int n);
void saveBoundingBox(const char*filename, uint16 rx, uint16 ry, uint16 bb_x, uint16 bb_y, int frame);
void saveAsso(const char*filename, uint32 **Nearest, float32 **distances, int nc0, MeteorROI *stats);
void saveAsso_VT(const char*filename, int nc0, MeteorROI *stats, int frame);
void saveConflicts(const char*filename, uint32 *conflicts, uint32 **Nearest, float32 **distances, int n_asso, int n_conflict);
void saveAssoConflicts(const char*filename, int frame, uint32 *conflicts, uint32 **Nearest, float32 **distances, int n_asso, int n_conflict, MeteorROI *stats0, MeteorROI *stats1);
void saveError(const char *filename, MeteorROI *stats, int n);
void saveErrorMoy(const char *filename, double errMoy, double eType);
void saveMotionExtraction(char *filename, MeteorROI *stats0, MeteorROI *stats1, int nc0, double theta, double tx, double ty, int frame);

int sursegmentation(MeteorROI *stats, int n);
void plot_bouding_box(rgb8** img, int ymin, int ymax, int xmin, int xmax, int border, rgb8 color);

// Analysing
void filter_speed_binarize(uint32** in, int i0, int i1, int j0, int j1, uint8** out, MeteorROI* stats);

// Image
rgb8** load_image_color(const char* filename, long* i0, long* i1, long* j0, long* j1);
void saveFrame_threshold(const char*filename, uint8**I0, uint8**I1, int i0, int i1, int j0, int j1);
void saveFrame_quad(const char*filename, uint8**I0, uint8**I1, uint32**I2, uint32**I3, int nbLabel, MeteorROI* stats, int i0, int i1, int j0, int j1);
void saveFrame_ui32matrix(const char*filename, uint32**I, int i0, int i1, int j0, int j1);
void saveFrame_ui8matrix(const char*filename, uint8**I, int i0, int i1, int j0, int j1);
void saveMax(const char*filename, uint8**I, int i0, int i1, int j0, int j1);
void saveFrame_quad_hysteresis(const char*filename, uint8**I0, uint32**SH, uint32**SB, uint32**Y, int i0, int i1, int j0, int j1);
void saveVideoFrame_tracking(const char*filename, uint8** I, Track* tracks, int tracks_nb, int i0, int i1, int j0, int j1);
void saveFrame_tracking(const char*filename, uint8** I, Track* tracks, int tracks_nb, int i0, int i1, int j0, int j1);

void split_path_file(char** p, char** f, char *pf);
void get_data_from_tracks_path(char* path, int *light_min, int *light_max, char **filename);
void get_bouding_box_path_from_tracks_path(char* path_tracks, char* filename);

void create_debug_dir(char *output_stats, char *filename, int light_min, int light_max);
void create_frames_dir(char *dest_path, char *filename, int light_min, int light_max);
void create_tracks_dir(char *output_dest, char *filename, int light_min, int light_max);
void create_video_dir(char *dest_path, char *filename, int light_min, int light_max);
void create_debug_files(int frame);
void create_frames_files(int frame);
void create_videos_files(char* filename);

void copy_ui8matrix_ui8matrix(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y);
void convert_ui8vector_ui32vector(uint8 *X, long nl, long nh, uint32 *Y);
void convert_ui8matrix_ui32matrix(uint8 **X, int nrl, int nrh, int ncl, int nch, uint32 **Y);
void WritePNMrow(uint8  *line, int width, FILE  *file); // defined in NRC but private...

#endif //__DEBUGUTIL_H__
