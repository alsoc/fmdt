#pragma once

#include <stdio.h>

#include "fmdt/tracking/tracking_struct.h"

void tracking_track_array_write(FILE* f, const vec_track_t track_array);
void tracking_track_array_write_full(FILE* f, const vec_track_t track_array);
void tracking_track_array_magnitude_write(FILE* f, const vec_track_t track_array);
void tracking_parse_tracks(const char* filename, vec_track_t* track_array);
void tracking_BB_array_write(FILE* f, const vec_BB_t* BB_array, const vec_track_t track_array);

