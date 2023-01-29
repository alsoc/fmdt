#pragma once

#include <stdint.h>

#include "fmdt/validation/validation_struct.h"

#define MAX_TRACKS_SIZE 10000
extern uint8_t g_is_valid_track[MAX_TRACKS_SIZE];
extern unsigned g_n_val_objects;
extern validation_obj_t* g_val_objects;
extern int g_true_positive[N_OBJECTS];
extern int g_false_positive[N_OBJECTS];
extern int g_true_negative[N_OBJECTS];
extern int g_false_negative[N_OBJECTS];
