#pragma once

#include <stdint.h>

#include "fmdt/features.h"

// Enums
// enum obj_e { UNKNOWN = 0, METEOR, STAR, NOISE, N_OBJECTS }; // <= now defined in "fmdt/features.h"
enum color_e { MISC = 0, GRAY, GREEN, RED, PURPLE, ORANGE, BLUE, YELLOW, N_COLORS };
enum state_e { TRACK_NEW = 1, TRACK_UPDATED, TRACK_EXTRAPOLATED, TRACK_LOST, TRACK_FINISHED };
// to remember why a 'meteor' object became a 'noise' object
enum change_state_reason_e { REASON_TOO_BIG_ANGLE = 1, REASON_WRONG_DIRECTION, REASON_TOO_LONG_DURATION };

#define METEOR_COLOR GREEN
#define STAR_COLOR PURPLE
#define NOISE_COLOR ORANGE
#define UNKNOWN_COLOR GRAY

#define METEOR_STR "meteor"
#define STAR_STR "star"
#define NOISE_STR "noise"
#define UNKNOWN_STR "unknown"

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
    int32_t time;
    int32_t time_motion;
    int32_t prev_id;
    int32_t next_id;
    uint8_t is_extrapolated;
    uint32_t magnitude;
} ROI_light_t;

typedef struct track {
    uint16_t* id;
    ROI_light_t* begin;
    ROI_light_t* end;
    float* extrapol_x;
    float* extrapol_y;
    enum state_e* state;
    enum obj_e* obj_type;
    enum change_state_reason_e* change_state_reason;
    uint32_t** magnitude;

    size_t _size; // current size/utilization of the fields
    size_t _max_size; // maximum amount of data that can be contained in the fields
    size_t _offset;
} track_t;

typedef struct BB_t {
    uint16_t bb_x;
    uint16_t bb_y;
    uint16_t rx;
    uint16_t ry;
    uint16_t track_id;
    struct BB_t* next;
} BB_t;

typedef struct {
    ROI_light_t** array;
    uint32_t* n_ROI;
    uint32_t _max_n_ROI;
    size_t _size; // current size/utilization of the 'ROI_history_t.array' field
    size_t _max_size; // maximum amount of data that can be contained in the 'ROI_history_t.array' field
} ROI_history_t;

typedef struct {
    ROI_history_t* ROI_history;
    ROI_light_t* ROI_list;
} tracking_data_t;

extern enum color_e g_obj_to_color[N_OBJECTS];
extern char g_obj_to_string[N_OBJECTS][64];
extern char g_obj_to_string_with_spaces[N_OBJECTS][64];

tracking_data_t* tracking_alloc_data(const size_t max_history_size, const size_t max_ROI_size);
void tracking_init_data(tracking_data_t* tracking_data);
void tracking_free_data(tracking_data_t* tracking_data);

void tracking_init_global_data();
enum obj_e tracking_string_to_obj_type(const char* string);
track_t* tracking_alloc_track_array(const size_t max_size);
void tracking_init_track_array(track_t* track_array);
void tracking_free_track_array(track_t* track_array);
void tracking_clear_index_track_array(track_t* track_array, const size_t t);
// void tracking_init_tracks(track_t* tracks, int n);
void tracking_init_BB_array(BB_t** BB_array);
void tracking_free_BB_array(BB_t** BB_array);
void _tracking_perform(tracking_data_t* tracking_data, const uint16_t* ROI0_id, const uint16_t* ROI0_xmin,
                       const uint16_t* ROI0_xmax, const uint16_t* ROI0_ymin, const uint16_t* ROI0_ymax,
                       const float* ROI0_x, const float* ROI0_y, const float* ROI0_error, const int32_t* ROI0_prev_id, 
                       const int32_t* ROI0_next_id, const uint32_t* ROI0_magnitude, const size_t n_ROI0,
                       const uint16_t* ROI1_id, const uint16_t* ROI1_xmin, const uint16_t* ROI1_xmax,
                       const uint16_t* ROI1_ymin, const uint16_t* ROI1_ymax, const float* ROI1_x, const float* ROI1_y,
                       const int32_t* ROI1_prev_id, const uint32_t* ROI1_magnitude, const size_t n_ROI1,
                       uint16_t* track_id, ROI_light_t* track_begin, ROI_light_t* track_end, float* track_extrapol_x,
                       float* track_extrapol_y, enum state_e* track_state, enum obj_e* track_obj_type,
                       enum change_state_reason_e* track_change_state_reason, uint32_t** track_magnitude,
                       size_t* offset_tracks, size_t* n_tracks, BB_t** BB_array, size_t frame, double theta, double tx,
                       double ty, double mean_error, double std_deviation, size_t r_extrapol, float angle_max,
                       float diff_dev, int track_all, size_t fra_star_min, size_t fra_meteor_min,
                       size_t fra_meteor_max);
void tracking_perform(tracking_data_t* tracking_data, const ROI_t* ROI_array0, ROI_t* ROI_array1, track_t* track_array,
                      BB_t** BB_array, size_t frame, double theta, double tx, double ty, double mean_error,
                      double std_deviation, size_t r_extrapol, float angle_max, float diff_dev, int track_all,
                      size_t fra_star_min, size_t fra_meteor_min, size_t fra_meteor_max);
size_t _tracking_count_objects(const uint16_t* track_id, const enum obj_e* track_obj_type, unsigned* n_stars,
                               unsigned* n_meteors, unsigned* n_noise, const size_t n_tracks);
// return the real number of tracks
size_t tracking_count_objects(const track_t* track_array, unsigned* n_stars, unsigned* n_meteors, unsigned* n_noise);
// void tracking_print_array_BB(BB_t** tabBB, int n);
void _tracking_track_array_write(FILE* f, const uint16_t* track_id, const ROI_light_t* track_begin,
                                 const ROI_light_t* track_end, const enum obj_e* track_obj_type,
                                 const size_t n_tracks);
void tracking_track_array_write(FILE* f, const track_t* track_array);

void _tracking_track_array_magnitude_write(FILE* f, const uint16_t* track_id, const enum obj_e* track_obj_type,
                                           const uint32_t** track_magnitude, const size_t n_tracks);
void tracking_track_array_magnitude_write(FILE* f, const track_t* track_array);
// void tracking_print_buffer(ROIx2_t* buffer, int n);
void tracking_parse_tracks(const char* filename, track_t* track_array);
// void tracking_save_tracks(const char* filename, track_t* tracks, int n);
void tracking_save_array_BB(const char* filename, BB_t** BB_array, track_t* track_array, int N, int track_all);
size_t tracking_get_track_time(const track_t* track_array, const size_t t);
