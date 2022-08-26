// ------------------
// --- Tracking.h ---
// ------------------

#pragma once

#include <stdint.h>

#include "features.h"

/*
 * Maxime Millet
 * Clara Ciocan/ Mathuran Kandeepan
 */

// Enums
enum obj_e { UNKNOWN = 0, METEOR, STAR, NOISE, N_OBJECTS };
enum color_e { MISC = 0, GRAY, GREEN, RED, PURPLE, ORANGE, BLUE, YELLOW, N_COLORS };
enum state_e { TRACK_NEW = 1, TRACK_UPDATED, TRACK_EXTRAPOLATED, TRACK_LOST, TRACK_FINISHED };

#define METEOR_COLOR GREEN
#define STAR_COLOR PURPLE
#define NOISE_COLOR ORANGE
#define UNKNOWN_COLOR GRAY

#define METEOR_STR "meteor"
#define STAR_STR "star"
#define NOISE_STR "noise"
#define UNKNOWN_STR "unknown"

typedef struct track {
    unsigned timestamp;
    uint16_t id;
    ROI_t begin;
    ROI_t end;
    float x;
    float y;
    uint32_t time;
    uint16_t bb_x;
    uint16_t bb_y;
    uint16_t rx;
    uint16_t ry;
    float xmin;
    float xmax;
    float ymin;
    float ymax;
    enum state_e state;
    enum obj_e obj_type;
    // resultat validation (bad design)
    uint8_t is_valid;
} track_t;

typedef struct {
    track_t* data;
    size_t size; // current size/utilization of the 'track_array_t.data' field
    size_t max_size; // maximum amount of data that can be contained in the 'track_array_t.data' field
    size_t offset;
} track_array_t;

typedef struct BB_t {
    uint16_t bb_x;
    uint16_t bb_y;
    uint16_t rx;
    uint16_t ry;
    uint16_t track_id;
    struct BB_t* next;
} BB_t;

extern enum color_e g_obj_to_color[N_OBJECTS];
extern char g_obj_to_string[N_OBJECTS][64];
extern char g_obj_to_string_with_spaces[N_OBJECTS][64];

void tracking_init_global_data();
enum obj_e tracking_string_to_obj_type(const char* string);
track_array_t* tracking_alloc_track_array(const size_t max_size);
void tracking_init_track_array(track_array_t* track_array);
void tracking_free_track_array(track_array_t* track_array);
void tracking_init_tracks(track_t* tracks, int n);
void tracking_init_BB_array(BB_t** BB_array);
void tracking_free_BB_array(BB_t** BB_array);
void tracking_perform(ROI_history_t* ROI_hist, track_array_t* track_array, BB_t** BB_array, int frame, int theta,
                      int tx, int ty, int r_extrapol, float angle_max, float diff_dev, int track_all, int fra_star_min,
                      int fra_meteor_min, int fra_meteor_max);

// return the real number of tracks
size_t tracking_count_objects(const track_array_t* track_array, unsigned* n_stars, unsigned* n_meteors,
                              unsigned* n_noise);

// void tracking_print_array_BB(BB_t** tabBB, int n);
void tracking_print_tracks(FILE* f, const track_t* tracks, const int n);
// void tracking_print_buffer(ROIx2_t* buffer, int n);
void tracking_parse_tracks(const char* filename, track_t* tracks, size_t* n);
// void tracking_save_tracks(const char* filename, track_t* tracks, int n);
void tracking_save_array_BB(const char* filename, BB_t** BB_array, track_t* tracks, int n, int track_all);
