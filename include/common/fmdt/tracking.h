// ------------------
// --- Tracking.h ---
// ------------------

#pragma once

#include <stdint.h>

#include "fmdt/features.h"

/*
 * Maxime Millet
 * Clara Ciocan/ Mathuran Kandeepan
 */

// Enums
enum obj_e { UNKNOWN = 0, METEOR, STAR, NOISE, N_OBJECTS };
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

typedef struct track {
    uint16_t* id;
    ROI_t* begin;
    ROI_t* end;
    float* extrapol_x;
    float* extrapol_y;
    enum state_e* state;
    enum obj_e* obj_type;
    enum change_state_reason_e* change_state_reason;

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
    ROI_t** array;
    size_t _size; // current size/utilization of the 'ROI_history_t.array' field
    size_t _max_size; // maximum amount of data that can be contained in the 'ROI_history_t.array' field
} ROI_history_t;

typedef struct {
    ROI_history_t* ROI_history;
    ROI_t* ROI_list;
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
void tracking_perform(tracking_data_t* tracking_data, const ROI_t* ROI_array0, ROI_t* ROI_array1, track_t* track_array,
                      BB_t** BB_array, size_t frame, double theta, double tx, double ty, double mean_error,
                      double std_deviation, size_t r_extrapol, float angle_max, float diff_dev, int track_all,
                      size_t fra_star_min, size_t fra_meteor_min, size_t fra_meteor_max);
// return the real number of tracks
size_t tracking_count_objects(const track_t* track_array, unsigned* n_stars, unsigned* n_meteors, unsigned* n_noise);
// void tracking_print_array_BB(BB_t** tabBB, int n);
void tracking_print_track_array(FILE* f, const track_t* track_array);
// void tracking_print_buffer(ROIx2_t* buffer, int n);
void tracking_parse_tracks(const char* filename, track_t* track_array);
// void tracking_save_tracks(const char* filename, track_t* tracks, int n);
void tracking_save_array_BB(const char* filename, BB_t** BB_array, track_t* track_array, int N, int track_all);
size_t tracking_get_track_time(const track_t* track_array, const size_t t);
