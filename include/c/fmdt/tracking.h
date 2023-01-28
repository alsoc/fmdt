#pragma once

#include <stdint.h>

#include "fmdt/features.h"

// Enums
// enum obj_e { UNKNOWN = 0, METEOR, STAR, NOISE, N_OBJECTS }; // <= now defined in "fmdt/features.h"
enum color_e { MISC = 0, GRAY, GREEN, RED, PURPLE, ORANGE, BLUE, YELLOW, N_COLORS };
enum state_e { TRACK_UPDATED = 1, TRACK_LOST, TRACK_FINISHED };
// to remember why a 'meteor' object became a 'noise' object
enum change_state_reason_e { REASON_TOO_BIG_ANGLE = 1, REASON_WRONG_DIRECTION, REASON_TOO_LONG_DURATION, N_REASONS };

typedef enum color_e* vec_color_e;

#define METEOR_COLOR GREEN
#define STAR_COLOR PURPLE
#define NOISE_COLOR ORANGE
#define UNKNOWN_COLOR GRAY

#define METEOR_STR "meteor"
#define STAR_STR "star"
#define NOISE_STR "noise"
#define UNKNOWN_STR "unknown"

#define TOO_BIG_ANGLE_STR "too big angle"
#define WRONG_DIRECTION_STR "wrong direction"
#define TOO_LONG_DURATION_STR "too long duration"

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

typedef struct ROI_track {
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
} ROI_track_t;

typedef struct track {
    uint32_t id;
    ROI_track_t begin;
    ROI_track_t end;
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
    ROI_track_t** array;
    motion_t* motion;
    uint32_t* n_ROI;
    uint32_t _max_n_ROI;
    size_t _size; // current size/utilization of the 'ROI_history_t.array' field
    size_t _max_size; // maximum amount of data that can be contained in the 'ROI_history_t.array' field
} ROI_history_t;

typedef struct {
    vec_track_t tracks;
    ROI_history_t* ROI_history;
    motion_t* motion_history;
    ROI_track_t* ROI_list;
} tracking_data_t;

extern enum color_e g_obj_to_color[N_OBJECTS];
extern char g_obj_to_string[N_OBJECTS][64];
extern char g_obj_to_string_with_spaces[N_OBJECTS][64];
extern char g_change_state_to_string[N_REASONS][64];
extern char g_change_state_to_string_with_spaces[N_REASONS][64];

tracking_data_t* tracking_alloc_data(const size_t max_history_size, const size_t max_ROI_size);
void tracking_init_data(tracking_data_t* tracking_data);
void tracking_free_data(tracking_data_t* tracking_data);

void tracking_init_global_data();
enum obj_e tracking_string_to_obj_type(const char* string);
void _tracking_perform(tracking_data_t* tracking_data, const uint32_t* ROI_id, const uint32_t* ROI_xmin,
                       const uint32_t* ROI_xmax, const uint32_t* ROI_ymin, const uint32_t* ROI_ymax,
                       const uint32_t* ROI_S, const float* ROI_x, const float* ROI_y, const float* ROI_error,
                       const uint32_t* ROI_prev_id, const uint32_t* ROI_magnitude, const size_t n_ROI1,
                       vec_BB_t** BB_array, const size_t frame, const motion_t* motion_est, const size_t r_extrapol,
                       const float angle_max, const float diff_dev, const int track_all, const size_t fra_star_min,
                       const size_t fra_meteor_min, const size_t fra_meteor_max, const int magnitude,
                       const uint8_t extrapol_order_max, const float min_extrapol_ratio_S);
void tracking_perform(tracking_data_t* tracking_data, const ROI_t* ROI_array, vec_BB_t** BB_array, size_t frame,
                      const motion_t* motion_est, const size_t r_extrapol, const float angle_max, const float diff_dev,
                      const int track_all, const size_t fra_star_min, const size_t fra_meteor_min,
                      const size_t fra_meteor_max, const int magnitude, const uint8_t extrapol_order_max,
                      const float min_extrapol_ratio_S);
// return the real number of tracks
size_t tracking_count_objects(const vec_track_t track_array, unsigned* n_stars, unsigned* n_meteors, unsigned* n_noise);
void tracking_track_array_write(FILE* f, const vec_track_t track_array);
void tracking_track_array_write_full(FILE* f, const vec_track_t track_array);
void tracking_track_array_magnitude_write(FILE* f, const vec_track_t track_array);
void tracking_parse_tracks(const char* filename, vec_track_t* track_array);
void tracking_BB_array_write(FILE* f, const vec_BB_t* BB_array, const vec_track_t track_array);
size_t tracking_get_track_time(const vec_track_t track_array, const size_t t);
