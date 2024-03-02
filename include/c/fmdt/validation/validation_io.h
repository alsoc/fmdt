/*!
 * \file
 * \brief IOs for tracks validation.
 */

#pragma once

#include "fmdt/tracking/tracking_struct.h"

/**
 * Print a validation table into `stdout`.
 * Note that this function uses global data to print the table.
 *
 * @param track_array Vector of tracks.
 */
void validation_print(const vec_track_t track_array);
