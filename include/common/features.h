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
    uint16_t id; // ID
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

typedef struct {
    ROI_t* data;
    size_t size; // current size/utilization of the 'ROI_array_t.data' field
    size_t max_size; // maximum amount of data that can be contained in the 'ROI_array_t.data' field
} ROI_array_t;

typedef struct {
    ROI_array_t* array;
    size_t size; // current size/utilization of the 'ROI_history_t.array' field
    size_t max_size; // maximum amount of data that can be contained in the 'ROI_history_t.array' field
} ROI_history_t;

typedef struct track track_t; // defined in "tracking.h"

ROI_array_t* features_alloc_ROI_array(const size_t max_size);
void features_init_ROI_array(ROI_array_t* ROI_array);
void features_free_ROI_array(ROI_array_t* ROI_array);
ROI_history_t* features_alloc_ROI_history(const size_t max_history_size, const size_t max_ROI_size);
void features_free_ROI_history(ROI_history_t* ROI_hist);
void features_rotate_ROI_history(ROI_history_t* ROI_hist);

void features_init_ROI(ROI_t* stats, int n);
void features_extract(const uint32_t** img, const int i0, const int i1, const int j0, const int j1,
                      const int n_ROI, ROI_array_t* ROI_array);
// void features_filter_surface(ROI_t* stats, int n, uint32_t** img, uint32_t threshold_min, uint32_t threshold_max);
void features_merge_HI_CCL_v2(uint32_t** HI, const uint32_t** M, const int i0, const int i1, const int j0,
                              const int j1, ROI_array_t* ROI_array, const int S_min, const int S_max);
void features_shrink_stats(const ROI_array_t* ROI_array_src, ROI_array_t* ROI_array_dest);
double features_error_moy(const ROI_array_t* stats);
double features_ecart_type(const ROI_array_t* stats, const double errMoy);
void features_motion(ROI_array_t* ROI_array0, ROI_array_t* ROI_array1, double* theta, double* tx, double* ty);
// void features_motion_extraction(ROI_t* stats0, ROI_t* stats1, int nc0, double theta, double tx, double ty);
// int features_analyse_ellipse(ROI_t* stats, int n, float e_threshold);

// void features_print_stats(ROI_t* stats, int n);
// void features_parse_stats(const char* filename, ROI_t* stats, int* n);
// void features_save_stats(const char* filename, const ROI_array_t* ROI_array, const track_t* tracks);
void features_save_stats_file(FILE* f, const ROI_array_t* ROI_array, const track_t* tracks);
// void features_save_stats(const char* filename, ROI_t* stats, int n, track_t* tracks);

// void features_save_motion(const char* filename, double theta, double tx, double ty, int frame);
// void features_save_error(const char* filename, ROI_t* stats, int n);
// void features_save_error_moy(const char* filename, double errMoy, double eType);
// void features_save_motion_extraction(const char* filename, const ROI_array_t* ROI_array, const double theta,
//                                      const double tx, const double ty, const int frame);
