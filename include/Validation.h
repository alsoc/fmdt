// --------------------
// --- Validation.h ---
// --------------------
/*
 * Maxime MILLET
 */

#include "Features.h"
#include "Tracking.h"

#ifndef _VALIDATION_
#define _VALIDATION_

#define VALIDATION_MIN_STDDEV       2.0
#define VALIDATION_MAX_STDDEV       30.0
#define VALIDATION_MIN_NORME        2.0
#define VALIDATION_MAX_NORME        20.0
#define VALIDATION_MIN_SURFACE      18
#define VALIDATION_MAX_SURFACE      2500
#define VALIDATION_MIN_TRACK_UPDATE 3

typedef struct input {
    //Debut
    sint16 t0;
    sint16 x0;
    sint16 y0;

    // Fin
    sint16 t1;
    sint16 x1;
    sint16 y1;

    sint16 t0_min;
    sint16 t1_max;


    
    // Rectangle englobant
    sint16 bb_x0, bb_x1;
    sint16 bb_y0, bb_y1;

    sint16 bb_x0_m, bb_x1_m;
    sint16 bb_y0_m, bb_y1_m;
    sint16 bb_x0_p, bb_x1_p;
    sint16 bb_y0_p, bb_y1_p;

    // Interpolation lineaire (remplacer par interpo bilineaire sur grille 2d ?)
    // on a (xp, yp) qui vient de la piste et on a verifier si a*xp + b est proche de yp
    float32 a;
    float32 b;
    uint8 dirX; // x1 >= x0
    uint8 dirY; // y1 >= <0
    
    //infos piste
    Track* track;
    unsigned track_id;
    sint16 xt;
    sint16 yt;
    
    //compteurs
    uint16 nb_tracks;
    uint16 hits;
    uint16 is_valid;
    uint16 is_valid_last;
} ValidationInput;

int Validation(char* inputs_file, Track* _tracks_pool, unsigned _tracks_nb, char* _dst_path);
void Validation_free(void);
void Validation_step(unsigned timestamp);
void Validation_final(void);

#endif

