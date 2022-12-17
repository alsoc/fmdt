#pragma once

#include <stdint.h>

#include "fmdt/features.h"

// Enums
// enum obj_e { UNKNOWN = 0, METEOR, STAR, NOISE, N_OBJECTS }; // <= now defined in "fmdt/features.h"
enum color_e { MISC = 0, GRAY, GREEN, RED, PURPLE, ORANGE, BLUE, YELLOW, N_COLORS };
enum state_e { TRACK_NEW = 1, TRACK_UPDATED, TRACK_LOST, TRACK_FINISHED };
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
    uint16_t track_id;
    uint16_t bb_x;
    uint16_t bb_y;
    uint16_t rx;
    uint16_t ry;
    int is_extrapolated;
} BB_t;

typedef BB_t* vec_BB_t;

typedef struct ROI_light {
    uint16_t id;
    uint32_t frame;
    uint16_t xmin;
    uint16_t xmax;
    uint16_t ymin;
    uint16_t ymax;
    float x;
    float y;
    float dx;
    float dy;
    float error;
    int32_t time;
    int32_t time_motion;
    int32_t prev_id;
    int32_t next_id;
    uint8_t is_extrapolated;
    uint32_t magnitude;
} ROI_light_t;

typedef struct track {
    uint16_t id;
    ROI_light_t begin;
    ROI_light_t end;
    float extrapol_x;
    float extrapol_y;
    enum state_e state;
    enum obj_e obj_type;
    enum change_state_reason_e change_state_reason;
    vec_uint32_t magnitude;
} track_t;

typedef track_t* vec_track_t;

typedef struct {
    ROI_light_t** array;
    uint32_t* n_ROI;
    uint32_t _max_n_ROI;
    size_t _size; // current size/utilization of the 'ROI_history_t.array' field
    size_t _max_size; // maximum amount of data that can be contained in the 'ROI_history_t.array' field
} ROI_history_t;

typedef struct {
    vec_track_t tracks;
    ROI_history_t* ROI_history;
    ROI_light_t* ROI_list;
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
void _tracking_perform(tracking_data_t* tracking_data, const uint16_t* ROI_id, const uint16_t* ROI_xmin,
                       const uint16_t* ROI_xmax, const uint16_t* ROI_ymin, const uint16_t* ROI_ymax, const float* ROI_x,
                       const float* ROI_y, const float* ROI_error, const int32_t* ROI_prev_id,
                       const uint32_t* ROI_magnitude, const size_t n_ROI1, vec_BB_t** BB_array, size_t frame,
                       double theta, double tx, double ty, double mean_error, double std_deviation, size_t r_extrapol,
                       float angle_max, float diff_dev, int track_all, size_t fra_star_min, size_t fra_meteor_min,
                       size_t fra_meteor_max, int magnitude);
void tracking_perform(tracking_data_t* tracking_data, const ROI_t* ROI_array, vec_BB_t** BB_array, size_t frame,
                      double theta, double tx, double ty, double mean_error, double std_deviation, size_t r_extrapol,
                      float angle_max, float diff_dev, int track_all, size_t fra_star_min, size_t fra_meteor_min,
                      size_t fra_meteor_max, int magnitude);
// return the real number of tracks
size_t tracking_count_objects(const vec_track_t track_array, unsigned* n_stars, unsigned* n_meteors, unsigned* n_noise);
void tracking_track_array_write(FILE* f, const vec_track_t track_array);
void tracking_track_array_write_full(FILE* f, const vec_track_t track_array);
void tracking_track_array_magnitude_write(FILE* f, const vec_track_t track_array);
void tracking_parse_tracks(const char* filename, vec_track_t* track_array);
void tracking_BB_array_write(FILE* f, const vec_BB_t* BB_array, const vec_track_t track_array);
size_t tracking_get_track_time(const vec_track_t track_array, const size_t t);
