/*!
 * \file
 * \brief Functions to perform the ground truth validation.
 */

#pragma once

#include <stdint.h>

#include "fmdt/validation/validation_struct.h"

/**
 * From a file path, allocate the data required to perform the validation.
 * Note that this function allocates data in global data: allocates the `g_val_objects` buffer and initializes it from
 * the input file + initializes the `g_n_val_objects` global variable.
 * @param val_objects_file Path to an input file of ground truth tracks to parse.
 * @return Number of ground truth allocated objects.
 */
int validation_init(const char* val_objects_file);

/**
 * From a given vector of tracks, estimates the correctness compared to the ground truth (stored in global data).
 * Read `g_val_objects` and `g_n_val_objects`.
 * Write `g_val_objects`, `g_is_valid_track`, `g_true_positive`, `g_false_positive`, `g_true_negative`,
 * `g_false_negative`.
 * @param track_array Vector of tracks.
 */
void validation_process(const vec_track_t track_array);


/**
 * Free the validation global data.
 */
void validation_free(void);

/**
 * Compute the number of objects in a `validation_obj_t` array.
 * @param val_objects Array of validation objects.
 * @param n_val_objects Number of validation objects in \p val_objects.
 * @param n_stars Return the number of star objects.
 * @param n_meteors Return the number of meteor objects.
 * @param n_noise Return the number of noise objects.
 * @return Total number of objects (stars + meteors + noises).
 */
unsigned validation_count_objects(const validation_obj_t* val_objects, const unsigned n_val_objects, unsigned* n_stars,
                                  unsigned* n_meteors, unsigned* n_noise);
