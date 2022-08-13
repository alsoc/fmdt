// ------------------
// --- Tracking.h ---
// ------------------

#ifndef __TRACKING_H__
#define __TRACKING_H__

#include "Features.h"

/*
 * Maxime Millet
 * Clara Ciocan/ Mathuran Kandeepan
 */

// Enums
enum obj_e { UNKNOWN = 0, STAR, METEOR, NOISE, N_OBJECTS };
enum color_e { MISC = 0, GRAY, GREEN, RED, PURPLE, ORANGE, BLUE, YELLOW, N_COLORS };

#define METEOR_COLOR GREEN
#define STAR_COLOR PURPLE
#define NOISE_COLOR ORANGE
#define UNKNOWN_COLOR GRAY

#define METEOR_STR "meteor"
#define STAR_STR "star"
#define NOISE_STR "noise"
#define UNKNOWN_STR "unknown"

extern enum color_e g_obj_type_to_color[N_OBJECTS];
extern char g_obj_type_to_string[N_OBJECTS][64];
extern char g_obj_type_to_string_with_spaces[N_OBJECTS][64];

typedef struct {
    // unsigned state;
    unsigned timestamp;
    uint16 id;
    ROI_t begin;
    ROI_t end;
    float32 x;
    float32 y;
    float32 dx;
    float32 dy;
    uint32 time;
    uint16 bb_x;
    uint16 bb_y;
    uint16 rx;
    uint16 ry;
    // equation de la droite : y = ax + b
    float32 a;
    float32 b;
    float32 xmin;
    float32 xmax;
    float32 ymin;
    float32 ymax;
    int state;
    enum obj_e obj_type;
    // Resultat validation (bad design)
    uint8 is_valid;
    // Juste pour afficher les vitesses
    float32 vitesse[300];
    int cur;
} track_t;

// track_ts states
#define TRACK_NEW (1)
#define TRACK_FINISHED (2)
#define TRACK_EXTRAPOLATED (3)
#define TRACK_UPDATED (4)
#define TRACK_LOST (5)

#define NB_FRAMES 10000

typedef struct {
    ROI_t stats0;
    ROI_t stats1;
    int frame;
} ROIx2_t;

typedef struct BB_t {
    uint16 bb_x;
    uint16 bb_y;
    uint16 rx;
    uint16 ry;
    uint16 track_id;
    struct BB_t* next;
} BB_t;

void tracking_init_global_data();
enum obj_e tracking_string_to_obj_type(const char* string);
void tracking_init_tracks(track_t* tracks, int n);
void tracking_init_array_BB();
void tracking_perform(ROI_t* stats0, ROI_t* stats1, track_t* tracks, int nc0, int nc1, int frame, int* tracks_cnt,
                      int* offset, int theta, int tx, int ty, int r_extrapol, int d_line, float diff_deviation,
                      int track_all, int frame_star);

// return the real number of tracks
unsigned tracking_count_objects(const track_t* tracks, const int n_tracks, unsigned* n_stars, unsigned* n_meteors,
                                 unsigned* n_noise);

#endif // __TRACKING_H__
