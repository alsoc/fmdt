// --------------------
// --- Validation.h ---
// --------------------
/*
 * Maxime MILLET
 */

#ifndef __VALIDATION_H__
#define __VALIDATION_H__

#include "features.h"
#include "tracking.h"

#define VALIDATION_MIN_STDDEV 2.0
#define VALIDATION_MAX_STDDEV 30.0
#define VALIDATION_MIN_NORME 2.0
#define VALIDATION_MAX_NORME 20.0
#define VALIDATION_MIN_SURFACE 18
#define VALIDATION_MAX_SURFACE 2500
#define VALIDATION_MIN_TRACK_UPDATE 3

typedef struct validation_obj {
    // Debut
    sint16 t0;
    float32 x0;
    float32 y0;
    // Fin
    sint16 t1;
    float32 x1;
    float32 y1;
    sint16 t0_min;
    sint16 t1_max;
#ifdef ENABLE_DEBUG
    int track_t0;
    int track_t1;
    float32 track_y0;
    float32 track_x0;
    float32 track_x1;
    float32 track_y1;
#endif
    // Rectangle englobant
    float32 bb_x0, bb_x1;
    float32 bb_y0, bb_y1;
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
    // infos piste
    track_t* track;
    unsigned track_id;
    float32 xt;
    float32 yt;
    // compteurs
    uint16 nb_tracks;
    uint16 hits;
    uint16 is_valid;
    uint16 is_valid_last;
    enum obj_e obj_type;
} validation_obj_t;

int validation_init(char* val_objects_file);
void validation_print(const track_t* tracks, const int n_tracks);
void validation_process(track_t* tracks, int n_tracks);
void validation_free(void);
unsigned validation_count_objects(const validation_obj_t* val_objects, const unsigned n_val_objects, unsigned* n_stars,
                                  unsigned* n_meteors, unsigned* n_noise);

#endif // __VALIDATION_H__
