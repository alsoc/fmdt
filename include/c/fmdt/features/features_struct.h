#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#define MAX_ROI_SIZE_BEFORE_SHRINK 65535
#define MAX_ROI_SIZE 400

enum obj_e { UNKNOWN = 0, METEOR, STAR, NOISE, N_OBJECTS };

typedef struct {
    uint32_t* id; // ROI unique identifier
    uint32_t* xmin; // xmin bounding box
    uint32_t* xmax; // xmax bounding box
    uint32_t* ymin; // ymin bounding box
    uint32_t* ymax; // ymax bounding box
    uint32_t* S; // number of points
    uint32_t* Sx; // sum of x properties
    uint32_t* Sy; // sum of y properties
    float* x; // abscisse du centre d'inertie x = Sx / S
    float* y; // ordonnee du centre d'inertie y = Sy / S
    float* dx; // erreur par rapport a l`image recalee
    float* dy; // erreur par rapport a l`image recalee
    float* error; // error after motion estimation (= velocity if is_moving = 1)
    uint32_t* prev_id; // associated CC from t-1 -> t -> t+1
    uint32_t* next_id; // associated CC from t-1 -> t -> t+1
    uint8_t* is_moving;
    uint32_t* magnitude;

    size_t _size; // current size/utilization of the fields
    size_t _max_size; // maximum amount of data that can be contained in the fields
} ROI_t;

typedef struct {
    float theta; // rotation angle
    float tx; // translation vector x
    float ty; // translation vector y
    float mean_error;
    float std_deviation;
} motion_t;
