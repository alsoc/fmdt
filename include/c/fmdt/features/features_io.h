/*!
 * \file
 * \brief IOs functions for Regions of Interest (RoIs).
 */

#pragma once

#include <stdio.h>

#include "fmdt/features/features_struct.h"
#include "fmdt/tracking/tracking_struct.h"

/**
 * Print a table of RoIs.
 *
 * @param f File descriptor (write mode).
 * @param frame Frame id corresponding to the RoIs.
 * @param RoIs_basic Basic features.
 * @param RoIs_magn Magnitude features (can be NULL).
 * @param RoIs_elli Ellipse features (can be NULL).
 * @param n_RoIs Number of connected-components (= number of RoIs) in the 2D array of `labels`.
 * @param tracks Vector of tracks. It enables to match RoIs with corresponding track in the table of RoIs. Can be NULL,
 *               then the corresponding tracks are not shown.
 * @param age 0 if `frame` is the current frame, 1 if `frame` is the \f$t - 1\f$ frame. This is mandatory to find the
 *            corresponding track (if any). If `tracks == NULL` then this argument is useless.
 *
 * @see RoI_basic_t for more explanations about the features.
 * @see RoI_magn_t for more explanations about the features.
 * @see RoI_elli_t for more explanations about the features.
 */
void features_RoIs_write(FILE* f, const int frame, const RoI_basic_t* RoIs_basic, const RoI_magn_t* RoIs_magn,
                         const RoI_elli_t* RoIs_elli, const size_t n_RoIs, const vec_track_t tracks,
                         const unsigned age);

/**
 * Print two tables of RoIs, one at \f$t - 1\f$ and one at \f$t\f$.
 *
 * @param f File descriptor (in write mode).
 * @param prev_frame Frame id corresponding to the RoIs at \f$t -1\f$.
 * @param cur_frame Frame id corresponding to the RoIs at \f$t\f$.
 * @param RoIs0_basic Basic features (at \f$t -1\f$).
 * @param RoIs0_magn Magnitude features (at \f$t -1\f$, can be NULL).
 * @param RoIs0_elli Ellipse features (at \f$t -1\f$, can be NULL).
 * @param n_RoIs0 Number of connected-components (= number of RoIs) in the 2D array of `labels` (at \f$t -1\f$).
 * @param RoIs1_basic Basic features (at \f$t\f$).
 * @param RoIs1_magn Magnitude features (at \f$t\f$, can be NULL).
 * @param RoIs1_elli Ellipse features (at \f$t\f$, can be NULL).
 * @param n_RoIs1 Number of connected-components (= number of RoIs) in the 2D array of `labels` (at \f$t\f$).
 * @param tracks Vector of tracks. It enables to match RoIs with corresponding track in the table of RoIs. Can be NULL,
 *               then the corresponding tracks are not shown.
 *
 * @see RoI_basic_t for more explanations about the features.
 * @see RoI_magn_t for more explanations about the features.
 * @see RoI_elli_t for more explanations about the features.
 */
void features_RoIs0_RoIs1_write(FILE* f, const int prev_frame, const int cur_frame, const RoI_basic_t* RoIs0_basic,
                                const RoI_magn_t* RoIs0_magn, const RoI_elli_t* RoIs0_elli, const size_t n_RoIs0,
                                const RoI_basic_t* RoIs1_basic, const RoI_magn_t* RoIs1_magn,
                                const RoI_elli_t* RoIs1_elli, const size_t n_RoIs1, const vec_track_t tracks);
