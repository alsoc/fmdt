/*!
 * \file
 * \brief IOs for tracks.
 */

#pragma once

#include <stdio.h>

#include "fmdt/tracking/tracking_struct.h"

/**
 * Print a table of tracks (dedicated to the terminal).
 * @param f File descriptor (in write mode).
 * @param tracks A vector of tracks.
 */
void tracking_tracks_write(FILE* f, const vec_track_t tracks);

/**
 * Print a table of tracks (dedicated to the logs).
 * @param f File descriptor (in write mode).
 * @param tracks A vector of tracks.
 */
void tracking_tracks_write_full(FILE* f, const vec_track_t tracks);

/**
 * Print a list of magnitudes per track. Each line corresponds to a track.
 * @param f File descriptor (in write mode).
 * @param tracks A vector of tracks.
 */
void tracking_tracks_RoIs_id_write(FILE* f, const vec_track_t tracks);

/**
 * From a given path, parse the corresponding file and fill a vector of tracks.
 * @param filename The path of the file to parse.
 * @param tracks A vector of tracks.
 */
void tracking_parse_tracks(const char* filename, vec_track_t* tracks);

/**
 * Print list of bounding boxes. Each line corresponds to a bounding boxes.
 * @param f File descriptor (in write mode).
 * @param BBs A 2D vector of bounding boxes (first dimension is the frames, second dimensions is the bounding boxes).
 * @param tracks A vector of tracks.
 */
void tracking_BBs_write(FILE* f, const vec_BB_t* BBs, const vec_track_t tracks);
