// ------------------
// --- Tracking.h ---
// ------------------

#include "Features.h"

#ifndef _TRACKING_
#define _TRACKING_
/*
 * Maxime Millet
 * Clara Ciocan/ Mathuran Kandeepan
 */

typedef struct {
        // unsigned state;
        unsigned timestamp;

        MeteorROI begin;
        MeteorROI end;

        float32 x;
        float32 y;

        uint32 time;

        int state;
        // Resultat validation (bad design)
        // uint8 is_meteor;
        // uint8 is_valid;
} Track;

// Tracks states
#define TRACK_NEW            (1)
#define TRACK_FINISHED       (2)
#define TRACK_EXTRAPOLATED   (3)
#define TRACK_UPDATED        (4)
#define TRACK_LOST           (5)

typedef struct {
    MeteorROI stats;
    int frame;
}Buf;

void init_Track(Track *tracks, int n);
void Tracking(MeteorROI *stats0, MeteorROI *stats1, Track *tracks, int nc0, int nc1, int frame, int *last, int *offset, int theta, int tx, int ty);
int TrackStars(MeteorROI *stats0, MeteorROI *stats1, Track *tracks, int nc0, int nc1, int frame, int *last, int *offset);

#endif

