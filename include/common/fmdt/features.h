#pragma once

#include <stdint.h>

#include "fmdt/tracking.h"

typedef struct {
    uint16_t* id;
    uint32_t* frame;
    uint16_t* xmin;
    uint16_t* xmax;
    uint16_t* ymin;
    uint16_t* ymax;
    uint32_t* S; // number of points
    uint32_t* Sx; // sum of x properties
    uint32_t* Sy; // sum of y properties
    float* x; // abscisse du centre d'inertie x = Sx / S
    float* y; // ordonnee du centre d'inertie y = Sy / S
    float* dx; // erreur par rapport a l`image recalee
    float* dy; // erreur par rapport a l`image recalee
    float* error;
    int32_t* time;
    int32_t* time_motion;
    int32_t* prev_id; // associated CC from t-1 -> t -> t+1
    int32_t* next_id; // associated CC from t-1 -> t -> t+1
    uint8_t* is_moving;
    uint8_t* is_extrapolated;

    size_t _size; // current size/utilization of the fields
    size_t _max_size; // maximum amount of data that can be contained in the fields
} ROI_t;

// defined in "tracking.h"
typedef struct track track_t;

ROI_t* features_alloc_ROI_array(const size_t max_size);
void features_init_ROI_array(ROI_t* ROI_array);
void features_free_ROI_array(ROI_t* ROI_array);
void features_clear_index_ROI_array(ROI_t* ROI_array, const size_t r);
void features_copy_elmt_ROI_array(const ROI_t* ROI_array_src, ROI_t* ROI_array_dest, const int i_src, const int i_dest);
void features_copy_ROI_array(const ROI_t* ROI_array_src, ROI_t* ROI_array_dest);
void features_init_ROI(ROI_t* stats, int n);
void _features_extract(const uint32_t** img, const int i0, const int i1, const int j0, const int j1, uint16_t* ROI_id,
                       uint16_t* ROI_xmin, uint16_t* ROI_xmax, uint16_t* ROI_ymin, uint16_t* ROI_ymax, uint32_t* ROI_S,
                       uint32_t* ROI_Sx, uint32_t* ROI_Sy, float* ROI_x, float* ROI_y, const size_t n_ROI);
void features_extract(const uint32_t** img, const int i0, const int i1, const int j0, const int j1, const size_t n_ROI,
                      ROI_t* ROI_array);
// void features_filter_surface(ROI_t* ROI_array, uint32_t** img, uint32_t threshold_min, uint32_t threshold_max);
void _features_merge_HI_CCL_v2(const uint32_t** M, const uint8_t** HI_in, uint8_t** HI_out, const int i0, const int i1,
                               const int j0, const int j1, const uint16_t* ROI_id, const uint16_t* ROI_xmin,
                               const uint16_t* ROI_xmax, const uint16_t* ROI_ymin, const uint16_t* ROI_ymax,
                               uint32_t* ROI_S, const size_t n_ROI, const uint32_t S_min, const uint32_t S_max);
void features_merge_HI_CCL_v2(const uint32_t** M, const uint8_t** HI_in, uint8_t** HI_out, const int i0, const int i1,
                              const int j0, const int j1, ROI_t* ROI_array, const uint32_t S_min, const uint32_t S_max);
size_t _features_shrink_ROI_array(const uint16_t* ROI_src_id, const uint16_t* ROI_src_xmin,
                                  const uint16_t* ROI_src_xmax, const uint16_t* ROI_src_ymin,
                                  const uint16_t* ROI_src_ymax, const uint32_t* ROI_src_S, const uint32_t* ROI_src_Sx,
                                  const uint32_t* ROI_src_Sy, const float* ROI_src_x, const float* ROI_src_y,
                                  const size_t n_ROI_src, uint16_t* ROI_dest_id, uint16_t* ROI_dest_xmin,
                                  uint16_t* ROI_dest_xmax, uint16_t* ROI_dest_ymin, uint16_t* ROI_dest_ymax,
                                  uint32_t* ROI_dest_S, uint32_t* ROI_dest_Sx, uint32_t* ROI_dest_Sy, float* ROI_dest_x,
                                  float* ROI_dest_y);
void features_shrink_ROI_array(const ROI_t* ROI_array_src, ROI_t* ROI_array_dest);
double features_compute_mean_error(const ROI_t* stats);
double features_compute_std_deviation(const ROI_t* stats, const double mean_error);
void _features_compute_motion(const int32_t* ROI0_next_id, const float* ROI0_x, const float* ROI0_y, float* ROI0_dx,
                              float* ROI0_dy, float* ROI0_error, uint8_t* ROI0_is_moving, const size_t n_ROI0,
                              const float* ROI1_x, const float* ROI1_y, double* theta, double* tx, double* ty,
                              double* mean_error, double* std_deviation);
void features_compute_motion(const ROI_t* ROI_array1, ROI_t* ROI_array0, double* theta, double* tx,
                             double* ty, double* mean_error, double* std_deviation);
// void features_motion_extraction(ROI_t* stats0, ROI_t* stats1, int nc0, double theta, double tx, double ty);
// void features_print_stats(ROI_t* stats, int n);
// void features_parse_stats(const char* filename, ROI_t* stats, int* n);
// void features_save_stats(const char* filename, const ROI_t* ROI_array, const track_array_t* track_array,
//                          const unsigned age);
void _features_save_stats_file(FILE* f, const uint16_t* ROI_id, const uint16_t* ROI_xmin, const uint16_t* ROI_xmax,
                               const uint16_t* ROI_ymin, const uint16_t* ROI_ymax, const uint32_t* ROI_S,
                               const uint32_t* ROI_Sx, const uint32_t* ROI_Sy, const float* ROI_x, const float* ROI_y,
                               const int32_t* ROI_time, const int32_t* ROI_time_motion, const size_t n_ROI,
                               const track_t* track_array, const unsigned age);
void features_save_stats_file(FILE* f, const ROI_t* ROI_array, const track_t* track_array, const unsigned age);
// void features_save_stats(const char* filename, ROI_t* stats, int n, track_t* tracks);
// void features_save_motion(const char* filename, double theta, double tx, double ty, int frame);
// void features_save_error(const char* filename, const ROI_t* ROI_array);
// void features_save_error_moy(const char* filename, double mean_error, double std_deviation);
// void features_save_motion_extraction(const char* filename, const ROI_array_t* ROI_array, const double theta,
//                                      const double tx, const double ty, const int frame);
