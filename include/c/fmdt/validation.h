#pragma once

#include <stdint.h>

#include "fmdt/tracking.h"

typedef struct validation_obj {
    // Debut
    int16_t t0;
    float x0;
    float y0;
    // Fin
    int16_t t1;
    float x1;
    float y1;
    int16_t t0_min;
    int16_t t1_max;
#ifdef ENABLE_DEBUG
    int track_t0;
    int track_t1;
    float track_y0;
    float track_x0;
    float track_x1;
    float track_y1;
#endif
    // Rectangle englobant
    float bb_x0, bb_x1;
    float bb_y0, bb_y1;
    int16_t bb_x0_m, bb_x1_m;
    int16_t bb_y0_m, bb_y1_m;
    int16_t bb_x0_p, bb_x1_p;
    int16_t bb_y0_p, bb_y1_p;
    // Interpolation lineaire (remplacer par interpo bilineaire sur grille 2d ?)
    // on a (xp, yp) qui vient de la piste et on a verifier si a*xp + b est proche de yp
    float a;
    float b;
    uint8_t dirX; // x1 >= x0
    uint8_t dirY; // y1 >= <0
    // infos piste
    track_t* track;
    unsigned track_id;
    float xt;
    float yt;
    // compteurs
    uint16_t nb_tracks;
    uint16_t hits;
    uint16_t is_valid;
    uint16_t is_valid_last;
    enum obj_e obj_type;
} validation_obj_t;

#define MAX_TRACKS_SIZE 10000
extern uint8_t g_is_valid_track[MAX_TRACKS_SIZE];

int validation_init(const char* val_objects_file);
void validation_print(const vec_track_t track_array);
void validation_process(const vec_track_t track_array);
void validation_free(void);
unsigned validation_count_objects(const validation_obj_t* val_objects, const unsigned n_val_objects, unsigned* n_stars,
                                  unsigned* n_meteors, unsigned* n_noise);
