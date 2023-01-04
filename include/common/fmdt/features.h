#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#define MAX_ROI_SIZE_BEFORE_SHRINK 65535
#define MAX_ROI_SIZE 400

enum obj_e { UNKNOWN = 0, METEOR, STAR, NOISE, N_OBJECTS };

typedef struct {
    uint32_t* id; // ROI unique identifier
    uint32_t* xmin; // xmin bounding box
    uint32_t* xmax; // xmax bounding box
    uint32_t* ymin; // ymin bounding box
    uint32_t* ymax; // ymax bounding box
    uint32_t* S; // number of points
    uint32_t* Sx; // sum of x properties
    uint32_t* Sy; // sum of y properties
    float* x; // abscisse du centre d'inertie x = Sx / S
    float* y; // ordonnee du centre d'inertie y = Sy / S
    float* dx; // erreur par rapport a l`image recalee
    float* dy; // erreur par rapport a l`image recalee
    float* error; // error after motion estimation (= velocity if is_moving = 1)
    uint32_t* prev_id; // associated CC from t-1 -> t -> t+1
    uint32_t* next_id; // associated CC from t-1 -> t -> t+1
    uint8_t* is_moving;
    uint32_t* magnitude;

    size_t _size; // current size/utilization of the fields
    size_t _max_size; // maximum amount of data that can be contained in the fields
} ROI_t;

typedef struct {
    float theta; // rotation angle
    float tx; // translation vector x
    float ty; // translation vector y
    float mean_error;
    float std_deviation;
} motion_t;

// defined in "tracking.h"
typedef struct track track_t;
typedef track_t* vec_track_t;
typedef struct ROI_light ROI_light_t;

ROI_t* features_alloc_ROI_array(const size_t max_size);
void features_init_ROI_array(ROI_t* ROI_array);
void features_free_ROI_array(ROI_t* ROI_array);
void features_clear_index_ROI_array(ROI_t* ROI_array, const size_t r);
void features_copy_elmt_ROI_array(const ROI_t* ROI_array_src, ROI_t* ROI_array_dest, const int i_src, const int i_dest);
void features_copy_ROI_array(const ROI_t* ROI_array_src, ROI_t* ROI_array_dest);
void features_init_ROI(ROI_t* stats, int n);
void _features_extract(const uint32_t** img, const int i0, const int i1, const int j0, const int j1, uint32_t* ROI_id,
                       uint32_t* ROI_xmin, uint32_t* ROI_xmax, uint32_t* ROI_ymin, uint32_t* ROI_ymax, uint32_t* ROI_S,
                       uint32_t* ROI_Sx, uint32_t* ROI_Sy, float* ROI_x, float* ROI_y, const size_t n_ROI);
void features_extract(const uint32_t** img, const int i0, const int i1, const int j0, const int j1, const size_t n_ROI,
                      ROI_t* ROI_array);
// void features_filter_surface(ROI_t* ROI_array, uint32_t** img, uint32_t threshold_min, uint32_t threshold_max);
void _features_merge_CCL_HI_v2(const uint32_t** in_labels, const uint8_t** img_HI, uint32_t** out_labels, const int i0,
                               const int i1, const int j0, const int j1, uint32_t* ROI_id, const uint32_t* ROI_xmin,
                               const uint32_t* ROI_xmax, const uint32_t* ROI_ymin, const uint32_t* ROI_ymax,
                               const uint32_t* ROI_S, const size_t n_ROI, const uint32_t S_min, const uint32_t S_max);
void features_merge_CCL_HI_v2(const uint32_t** in_labels, const uint8_t** img_HI, uint32_t** out_labels, const int i0,
                              const int i1, const int j0, const int j1, ROI_t* ROI_array, const uint32_t S_min,
                              const uint32_t S_max);
size_t _features_shrink_ROI_array(const uint32_t* ROI_src_id, const uint32_t* ROI_src_xmin,
                                  const uint32_t* ROI_src_xmax, const uint32_t* ROI_src_ymin,
                                  const uint32_t* ROI_src_ymax, const uint32_t* ROI_src_S, const uint32_t* ROI_src_Sx,
                                  const uint32_t* ROI_src_Sy, const float* ROI_src_x, const float* ROI_src_y,
                                  const size_t n_ROI_src, uint32_t* ROI_dest_id, uint32_t* ROI_dest_xmin,
                                  uint32_t* ROI_dest_xmax, uint32_t* ROI_dest_ymin, uint32_t* ROI_dest_ymax,
                                  uint32_t* ROI_dest_S, uint32_t* ROI_dest_Sx, uint32_t* ROI_dest_Sy, float* ROI_dest_x,
                                  float* ROI_dest_y);
void features_shrink_ROI_array(const ROI_t* ROI_array_src, ROI_t* ROI_array_dest);
float features_compute_mean_error(const ROI_t* stats);
float features_compute_std_deviation(const ROI_t* stats, const float mean_error);
void _features_compute_motion(const float* ROI0_x, const float* ROI0_y, const float* ROI1_x, const float* ROI1_y,
                              float* ROI1_dx, float* ROI1_dy, float* ROI1_error, const uint32_t* ROI1_prev_id,
                              uint8_t* ROI1_is_moving, const size_t n_ROI1, motion_t* motion_est1,
                              motion_t* motion_est2);
void features_compute_motion(const ROI_t* ROI_array0, ROI_t* ROI_array1, motion_t* motion_est1, motion_t* motion_est2);
void _features_compute_magnitude(const uint8_t** img, const uint32_t img_width, const uint32_t img_height,
                                 const uint32_t** labels, const uint32_t* ROI_xmin, const uint32_t* ROI_xmax,
                                 const uint32_t* ROI_ymin, const uint32_t* ROI_ymax, const uint32_t* ROI_S,
                                 uint32_t* ROI_magnitude, const size_t n_ROI);
void features_compute_magnitude(const uint8_t** img, const uint32_t img_width, const uint32_t img_height,
                                const uint32_t** labels, ROI_t* ROI_array);
void _features_ROI_write(FILE* f, const int frame, const uint32_t* ROI_id, const uint32_t* ROI_xmin,
                         const uint32_t* ROI_xmax, const uint32_t* ROI_ymin, const uint32_t* ROI_ymax,
                         const uint32_t* ROI_S, const uint32_t* ROI_Sx, const uint32_t* ROI_Sy, const float* ROI_x,
                         const float* ROI_y, const uint32_t* ROI_magnitude, const size_t n_ROI,
                         const vec_track_t track_array, const unsigned age);
void features_ROI_write(FILE* f, const int frame, const ROI_t* ROI_array, const vec_track_t track_array,
                        const unsigned age);
void _features_ROI0_ROI1_write(FILE* f, const int prev_frame, const int cur_frame, const uint32_t* ROI0_id,
                               const uint32_t* ROI0_xmin, const uint32_t* ROI0_xmax, const uint32_t* ROI0_ymin,
                               const uint32_t* ROI0_ymax, const uint32_t* ROI0_S, const uint32_t* ROI0_Sx,
                               const uint32_t* ROI0_Sy, const float* ROI0_x, const float* ROI0_y,
                               const uint32_t* ROI0_magnitude, const size_t n_ROI0, const uint32_t* ROI1_id,
                               const uint32_t* ROI1_xmin, const uint32_t* ROI1_xmax, const uint32_t* ROI1_ymin,
                               const uint32_t* ROI1_ymax, const uint32_t* ROI1_S, const uint32_t* ROI1_Sx,
                               const uint32_t* ROI1_Sy, const float* ROI1_x, const float* ROI1_y,
                               const uint32_t* ROI1_magnitude, const size_t n_ROI1, const vec_track_t track_array);
void features_ROI0_ROI1_write(FILE* f, const int prev_frame, const int cur_frame, const ROI_t* ROI_array0,
                              const ROI_t* ROI_array1, const vec_track_t track_array);
void features_motion_write(FILE* f, const motion_t* motion_est1, const motion_t* motion_est2);
