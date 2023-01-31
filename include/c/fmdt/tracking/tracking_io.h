#pragma once

#include <stdio.h>

#include "fmdt/tracking/tracking_struct.h"

void tracking_tracks_write(FILE* f, const vec_track_t tracks);
void tracking_tracks_write_full(FILE* f, const vec_track_t tracks);
void tracking_tracks_magnitudes_write(FILE* f, const vec_track_t tracks);
void tracking_parse_tracks(const char* filename, vec_track_t* tracks);
void tracking_BBs_write(FILE* f, const vec_BB_t* BBs, const vec_track_t tracks);
