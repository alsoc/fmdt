#pragma once

#include <stdint.h>
#include <stddef.h>

#include "fmdt/motion/motion_struct.h" // motion_t

// Enums
enum obj_e { UNKNOWN = 0, METEOR, STAR, NOISE, N_OBJECTS };
enum state_e { TRACK_UPDATED = 1, TRACK_LOST, TRACK_FINISHED };
// to remember why a 'meteor' object became a 'noise' object
enum change_state_reason_e { REASON_TOO_BIG_ANGLE = 1, REASON_WRONG_DIRECTION, REASON_TOO_LONG_DURATION, N_REASONS };

typedef uint32_t* vec_uint32_t;

typedef struct BB_t {
    uint32_t frame_id;
    uint32_t track_id;
    uint32_t bb_x;
    uint32_t bb_y;
    uint32_t rx;
    uint32_t ry;
    int is_extrapolated;
} BB_t;

typedef BB_t* vec_BB_t;

typedef struct RoI_track {
    uint32_t id;
    uint32_t frame;
    uint32_t xmin;
    uint32_t xmax;
    uint32_t ymin;
    uint32_t ymax;
    uint32_t S;
    float x;
    float y;
    float dx;
    float dy;
    float error;
    uint32_t time;
    uint32_t time_motion;
    uint32_t prev_id;
    uint32_t next_id;
    uint8_t is_extrapolated;
    uint32_t magnitude;
} RoI_track_t;

typedef struct track {
    uint32_t id;
    RoI_track_t begin;
    RoI_track_t end;
    float extrapol_x;
    float extrapol_y;
    float extrapol_u;
    float extrapol_v;
    uint8_t extrapol_order;
    enum state_e state;
    enum obj_e obj_type;
    enum change_state_reason_e change_state_reason;
    vec_uint32_t magnitude;
} track_t;

typedef track_t* vec_track_t;

typedef struct {
    RoI_track_t** array;
    motion_t* motion;
    uint32_t* n_RoIs;
    uint32_t _max_n_RoIs;
    size_t _size; // current size/utilization of the 'RoIs_history_t.array' field
    size_t _max_size; // maximum amount of data that can be contained in the 'RoIs_history_t.array' field
} RoIs_history_t;

typedef struct {
    vec_track_t tracks;
    RoIs_history_t* RoIs_history;
    motion_t* motion_history;
    RoI_track_t* RoIs_list;
} tracking_data_t;

size_t _tracking_get_track_time(const RoI_track_t track_begin, const RoI_track_t track_end);
size_t tracking_get_track_time(const vec_track_t track_array, const size_t t);
size_t tracking_count_objects(const vec_track_t track_array, unsigned* n_stars, unsigned* n_meteors,
                              unsigned* n_noise);
