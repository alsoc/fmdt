/*!
 * \file
 * \brief Global data related to the tracks validation.
 */

#pragma once

#include <stdint.h>

#include "fmdt/validation/validation_struct.h"

/**
 *  Maximum number of tracks to evaluate in the validation process.
 */
#define MAX_TRACKS_SIZE 10000

/**
 *  Array that contains `1` or `2` value. `1` means that the current track is a true positive, `2` means that the
 *  current track is a false positive.
 */
extern uint8_t g_is_valid_track[MAX_TRACKS_SIZE];

/**
 *  Number of tracks from the ground truth.
 */
extern unsigned g_n_val_objects;

/**
 *  Array of ground truth tracks.
 */
extern validation_obj_t* g_val_objects;

/**
 *  Counters of true positive tracks depending on the object types.
 */
extern int g_true_positive[N_OBJECTS];

/**
 *  Counters of false positive tracks depending on the object types.
 */
extern int g_false_positive[N_OBJECTS];

/**
 *  Counters of true negative tracks depending on the object types.
 */
extern int g_true_negative[N_OBJECTS];

/**
 *  Counters of false negative tracks depending on the object types.
 */
extern int g_false_negative[N_OBJECTS];
