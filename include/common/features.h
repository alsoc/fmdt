/**
 * Copyright (c) 2017-2018, Arthur Hennequin
 * LIP6, UPMC, CNRS
 */

#ifndef __FEATURES_H__
#define __FEATURES_H__

#include <math.h>
#include <nrutil.h>

typedef struct {
    uint16 xmin;
    uint16 xmax;
    uint16 ymin;
    uint16 ymax;
    uint32 S; // number of points
    uint16 ID; // ID
    int track_id;
    float32 x; // abscisse du centre d'inertie x = Sx / S
    float32 y; // ordonnee du centre d'inertie y = Sy / S
    uint32 Sx; // sum of x properties
    uint32 Sy; // sum of y properties
    uint64 Sx2;
    uint64 Sxy;
    uint64 Sy2;
    float32 dx; // erreur par rapport a l`image recalee
    float32 dy; // erreur par rapport a l`image recalee
    float32 error;
    int time;
    int time_motion;
    int prev; // associated CC from t-1 -> t -> t+1
    int next; // associated CC from t-1 -> t -> t+1
    uint8 motion; // debug
    uint8 state;
} ROI_t;

typedef struct track track_t; // defined in "tracking.h"

void features_init_ROI(ROI_t* stats, int n);
void features_extract(uint32** img, int i0, int i1, int j0, int j1, ROI_t* stats, int n);
// void features_filter_surface(ROI_t* stats, int n, uint32** img, uint32 threshold_min, uint32 threshold_max);
void features_merge_HI_CCL_v2(uint32** HI, uint32** M, int i0, int i1, int j0, int j1, ROI_t* stats, int n, int S_min,
                              int S_max);
int features_shrink_stats(ROI_t* stats_src, ROI_t* stats_dest, int n);
double features_ecart_type(ROI_t* stats, int n, double errMoy);
double features_error_moy(ROI_t* stats, int n);
void features_motion(ROI_t* stats0, ROI_t* stats1, int n0, int n1, double* theta, double* tx, double* ty);
// void features_motion_extraction(ROI_t* stats0, ROI_t* stats1, int nc0, double theta, double tx, double ty);
// int features_analyse_ellipse(ROI_t* stats, int n, float e_threshold);

// void features_print_stats(ROI_t* stats, int n);
// void features_parse_stats(const char* filename, ROI_t* stats, int* n);
// void features_save_stats(const char* filename, ROI_t* stats, int n, track_t* tracks);
void features_save_stats_file(FILE* f, ROI_t* stats, int n, track_t* tracks);
// void features_save_stats(const char* filename, ROI_t* stats, int n, track_t* tracks);

// void features_save_motion(const char* filename, double theta, double tx, double ty, int frame);
// void features_save_error(const char* filename, ROI_t* stats, int n);
// void features_save_error_moy(const char* filename, double errMoy, double eType);
// void features_save_motion_extraction(char* filename, ROI_t* stats0, ROI_t* stats1, int nc0, double theta, double tx,
//                                      double ty, int frame);

#endif // __FEATURES_H__
