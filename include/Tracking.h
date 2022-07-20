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

        float32 dx;
        float32 dy;


        uint32 time;

        uint16 bb_x;
        uint16 bb_y;

        uint16 rx;
        uint16 ry;

        //equation de la droite : y = ax + b
        float32 a; 
        float32 b;

        float32 xmin;
        float32 xmax;
        float32 ymin;
        float32 ymax;

        int state;
        // Resultat validation (bad design)
        uint8 is_meteor;
        uint8 is_valid;

        // Juste pour afficher les vitesses
        float32 vitesse[300];
        int cur;
} Track;

// Tracks states
#define TRACK_NEW            (1)
#define TRACK_FINISHED       (2)
#define TRACK_EXTRAPOLATED   (3)
#define TRACK_UPDATED        (4)
#define TRACK_LOST           (5)

#define NB_FRAMES 10000


typedef struct {
    MeteorROI stats0;
    MeteorROI stats1;
    int frame;

}Buf;

typedef struct elemBB{
    uint16 bb_x;
    uint16 bb_y;
    uint16 rx;
    uint16 ry;
    struct elemBB *next;
} elemBB;


void init_Track(Track *tracks, int n);
void initTabBB();
void Tracking(MeteorROI *stats0, MeteorROI *stats1, Track *tracks, int nc0, int nc1, int frame, int *last, int *offset, int theta, int tx, int ty, int r_extrapol, int d_line, int diff_deviation);
void TrackStars(MeteorROI *stats0, MeteorROI *stats1, Track *tracks, int nc0, int nc1, int frame, int *last, int *offset);

#endif

