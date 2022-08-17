/**
 * Copyright (c) 2017-2018, Arthur Hennequin
 * LIP6, UPMC, CNRS
 */

#pragma once

#include <stdint.h>

#include "tracking.h"

typedef struct {
    uint16_t xmin;
    uint16_t xmax;
    uint16_t ymin;
    uint16_t ymax;
    uint32_t S; // number of points
    uint16_t ID; // ID
    int32_t track_id;
    float x; // abscisse du centre d'inertie x = Sx / S
    float y; // ordonnee du centre d'inertie y = Sy / S
    uint32_t Sx; // sum of x properties
    uint32_t Sy; // sum of y properties
    uint64_t Sx2;
    uint64_t Sxy;
    uint64_t Sy2;
    float dx; // erreur par rapport a l`image recalee
    float dy; // erreur par rapport a l`image recalee
    float error;
    int32_t time;
    int32_t time_motion;
    int32_t prev; // associated CC from t-1 -> t -> t+1
    int32_t next; // associated CC from t-1 -> t -> t+1
    uint8_t motion; // debug
    uint8_t state;
} ROI_t;

typedef struct track track_t; // defined in "tracking.h"

void features_init_ROI(ROI_t* stats, int n);
void features_extract(uint32_t** img, int i0, int i1, int j0, int j1, ROI_t* stats, int n);
// void features_filter_surface(ROI_t* stats, int n, uint32_t** img, uint32_t threshold_min, uint32_t threshold_max);
void features_merge_HI_CCL_v2(uint32_t** HI, uint32_t** M, int i0, int i1, int j0, int j1, ROI_t* stats, int n, int S_min,
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
