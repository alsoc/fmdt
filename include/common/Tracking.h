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
enum Obj_type { UNKNOWN = 0, STAR, METEOR, NOISE, N_OBJ_TYPES };

typedef struct {
    // unsigned state;
    unsigned timestamp;

    uint16 id;

    MeteorROI begin;
    MeteorROI end;

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
    enum Obj_type obj_type;
    // Resultat validation (bad design)
    uint8 is_valid;

    // Juste pour afficher les vitesses
    float32 vitesse[300];
    int cur;
} Track;

// Tracks states
#define TRACK_NEW (1)
#define TRACK_FINISHED (2)
#define TRACK_EXTRAPOLATED (3)
#define TRACK_UPDATED (4)
#define TRACK_LOST (5)

#define NB_FRAMES 10000

typedef struct {
    MeteorROI stats0;
    MeteorROI stats1;
    int frame;

} Buf;

typedef struct elemBB {
    uint16 bb_x;
    uint16 bb_y;
    uint16 rx;
    uint16 ry;
    uint16 track_id;
    struct elemBB* next;
} elemBB;

void init_Track(Track* tracks, int n);
void initTabBB();
void Tracking(MeteorROI* stats0, MeteorROI* stats1, Track* tracks, int nc0, int nc1, int frame, int* tracks_cnt,
              int* offset, int theta, int tx, int ty, int r_extrapol, int d_line, float diff_deviation, int track_all,
              int frame_star);

// return the real number of tracks
unsigned track_count_objects(const Track* tracks, const int n_tracks, unsigned* n_stars, unsigned* n_meteors,
                             unsigned* n_noise);

#endif // __TRACKING_H__
