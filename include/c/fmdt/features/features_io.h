/*!
 * \file
 * \brief IO functions for Regions of Interest (RoIs).
 */

#pragma once

#include <stdio.h>

#include "fmdt/features/features_struct.h"
#include "fmdt/tracking/tracking_struct.h"

/**
 * Print a table of RoIs.
 * @param f File descriptor (in write mode).
 * @param frame Frame id corresponding to the RoIs.
 * @param RoIs_id Array of RoI unique identifiers.
 * @param RoIs_xmin Array of minimum \f$x\f$ coordinates of the bounding box.
 * @param RoIs_xmax Array of maximum \f$x\f$ coordinates of the bounding box.
 * @param RoIs_ymin Array of minimum \f$y\f$ coordinates of the bounding box.
 * @param RoIs_ymax Array of maximum \f$y\f$ coordinates of the bounding box.
 * @param RoIs_S Array of RoI surfaces.
 * @param RoIs_Sx Array of sums of \f$x\f$ properties.
 * @param RoIs_Sy Array of sums of \f$y\f$ properties.
 * @param RoIs_x Array of centroids abscissa.
 * @param RoIs_y Array of centroids ordinate.
 * @param RoIs_magnitude Array of RoI magnitudes.
 * @param n_RoIs Number of connected-components (= number of RoIs) in the 2D array of `labels`.
 * @param tracks Vector of tracks. It enables to match RoIs with corresponding track in the table of RoIs.
 * @param age 0 if `frame` is the current frame, 1 if `frame` is the \f$t - 1\f$ frame. This is mandatory to find the
 *            corresponding track (if any).
 * @see RoIs_basic_t for more explanations about the features.
 * @see RoIs_misc_t for more explanations about the features.
 */
void _features_RoIs_write(FILE* f, const int frame, const uint32_t* RoIs_id, const uint32_t* RoIs_xmin,
                         const uint32_t* RoIs_xmax, const uint32_t* RoIs_ymin, const uint32_t* RoIs_ymax,
                         const uint32_t* RoIs_S, const uint32_t* RoIs_Sx, const uint32_t* RoIs_Sy, const float* RoIs_x,
                         const float* RoIs_y, const uint32_t* RoIs_magnitude, const size_t n_RoIs,
                         const vec_track_t tracks, const unsigned age);

/**
 * @param f File descriptor (write mode).
 * @param frame Frame id corresponding to the RoIs.
 * @param RoIs_basic Basic features.
 * @param RoIs_misc Miscellaneous features.
 * @param tracks Vector of tracks. It enables to match RoIs with corresponding track in the table of RoIs.
 * @param age 0 if `frame` is the current frame, 1 if `frame` is the \f$t - 1\f$ frame. This is mandatory to find the
 *            corresponding track (if any).
 * @see _features_RoIs_write for the explanations about the nature of the processing.
 * @see RoIs_basic_t for more explanations about the features.
 * @see RoIs_misc_t for more explanations about the features.
 */
void features_RoIs_write(FILE* f, const int frame, const RoIs_basic_t* RoIs_basic, const RoIs_misc_t* RoIs_misc,
                         const vec_track_t tracks, const unsigned age);

/**
 * Print two tables of RoIs, one at \f$t - 1\f$ and one at \f$t\f$.
 * @param f File descriptor (in write mode).
 * @param prev_frame Frame id corresponding to the RoIs at \f$t -1\f$.
 * @param cur_frame Frame id corresponding to the RoIs at \f$t\f$.
 * @param RoIs0_id Array of RoI unique identifiers (at \f$t -1\f$).
 * @param RoIs0_xmin Array of minimum \f$x\f$ coordinates of the bounding box (at \f$t -1\f$).
 * @param RoIs0_xmax Array of maximum \f$x\f$ coordinates of the bounding box (at \f$t -1\f$).
 * @param RoIs0_ymin Array of minimum \f$y\f$ coordinates of the bounding box (at \f$t -1\f$).
 * @param RoIs0_ymax Array of maximum \f$y\f$ coordinates of the bounding box (at \f$t -1\f$).
 * @param RoIs0_S Array of RoI surfaces (at \f$t -1\f$).
 * @param RoIs0_Sx Array of sums of \f$x\f$ properties (at \f$t -1\f$).
 * @param RoIs0_Sy Array of sums of \f$y\f$ properties (at \f$t -1\f$).
 * @param RoIs0_x Array of centroids abscissa (at \f$t -1\f$).
 * @param RoIs0_y Array of centroids ordinate (at \f$t -1\f$).
 * @param RoIs0_magnitude Array of RoI magnitudes (at \f$t -1\f$).
 * @param n_RoIs0 Number of connected-components (= number of RoIs) in the 2D array of `labels` (at \f$t -1\f$).
 * @param RoIs1_id Array of RoI unique identifiers (at \f$t\f$).
 * @param RoIs1_xmin Array of minimum \f$x\f$ coordinates of the bounding box (at \f$t\f$).
 * @param RoIs1_xmax Array of maximum \f$x\f$ coordinates of the bounding box (at \f$t\f$).
 * @param RoIs1_ymin Array of minimum \f$y\f$ coordinates of the bounding box (at \f$t\f$).
 * @param RoIs1_ymax Array of maximum \f$y\f$ coordinates of the bounding box (at \f$t\f$).
 * @param RoIs1_S Array of RoI surfaces (at \f$t\f$).
 * @param RoIs1_Sx Array of sums of \f$x\f$ properties (at \f$t\f$).
 * @param RoIs1_Sy Array of sums of \f$y\f$ properties (at \f$t\f$).
 * @param RoIs1_x Array of centroids abscissa (at \f$t\f$).
 * @param RoIs1_y Array of centroids ordinate (at \f$t\f$).
 * @param RoIs1_magnitude Array of RoI magnitudes (at \f$t\f$).
 * @param n_RoIs1 Number of connected-components (= number of RoIs) in the 2D array of `labels` (at \f$t\f$).
 * @param tracks Vector of tracks. It enables to match RoIs with corresponding track in the table of RoIs.
 * @see RoIs_basic_t for more explanations about the features.
 * @see RoIs_misc_t for more explanations about the features.
 */
void _features_RoIs0_RoIs1_write(FILE* f, const int prev_frame, const int cur_frame, const uint32_t* RoIs0_id,
                                 const uint32_t* RoIs0_xmin, const uint32_t* RoIs0_xmax, const uint32_t* RoIs0_ymin,
                                 const uint32_t* RoIs0_ymax, const uint32_t* RoIs0_S, const uint32_t* RoIs0_Sx,
                                 const uint32_t* RoIs0_Sy, const float* RoIs0_x, const float* RoIs0_y,
                                 const uint32_t* RoIs0_magnitude, const size_t n_RoIs0, const uint32_t* RoIs1_id,
                                 const uint32_t* RoIs1_xmin, const uint32_t* RoIs1_xmax, const uint32_t* RoIs1_ymin,
                                 const uint32_t* RoIs1_ymax, const uint32_t* RoIs1_S, const uint32_t* RoIs1_Sx,
                                 const uint32_t* RoIs1_Sy, const float* RoIs1_x, const float* RoIs1_y,
                                 const uint32_t* RoIs1_magnitude, const size_t n_RoIs1, const vec_track_t tracks);

/**
 * Print two tables of RoIs, one at \f$t - 1\f$ and one at \f$t\f$.
 * @param f File descriptor (in write mode).
 * @param prev_frame Frame id corresponding to the RoIs at \f$t -1\f$.
 * @param cur_frame Frame id corresponding to the RoIs at \f$t\f$.
 * @param RoIs0_basic Basic features (at \f$t -1\f$).
 * @param RoIs0_misc Miscellaneous features (at \f$t -1\f$).
 * @param RoIs1_basic Basic features (at \f$t\f$).
 * @param RoIs1_misc Miscellaneous features (at \f$t\f$)..
 * @param tracks Vector of tracks. It enables to match RoIs with corresponding track in the table of RoIs.
 * @see _features_RoIs0_RoIs1_write for the explanations about the nature of the processing.
 * @see RoIs_basic_t for more explanations about the features.
 * @see RoIs_misc_t for more explanations about the features.
 */
void features_RoIs0_RoIs1_write(FILE* f, const int prev_frame, const int cur_frame, const RoIs_basic_t* RoIs0_basic,
                                const RoIs_misc_t* RoIs0_misc, const RoIs_basic_t* RoIs1_basic,
                                const RoIs_misc_t* RoIs1_misc, const vec_track_t tracks);
