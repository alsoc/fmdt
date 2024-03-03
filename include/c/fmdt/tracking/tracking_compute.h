/*!
 * \file
 * \brief Functions to compute the tracks.
 */

#pragma once

#include "fmdt/features/features_struct.h"
#include "fmdt/tracking/tracking_struct.h"

/**
 * Allocation of inner data required to perform the tracking.
 *
 * @param max_history_size The maximum size of the history window (number of frames memorized in the history of RoIs).
 * @param max_RoIs_size The maximum number of RoIs per frame.
 * @return The allocated data.
 */
tracking_data_t* tracking_alloc_data(const size_t max_history_size, const size_t max_RoIs_size);

/**
 * Zero initialization of inner data required to perform the tracking.
 *
 * @param tracking_data Pointer of tracking inner data.
 */
void tracking_init_data(tracking_data_t* tracking_data);

/**
 * Free the tracking inner data.
 *
 * @param tracking_data Pointer of tracking inner data.
 */
void tracking_free_data(tracking_data_t* tracking_data);

/**
 * Create, update and finalize tracks. This function also performs the classification of the tracks.
 *
 * @param tracking_data Inner data.
 * @param RoIs RoIs features (at \f$t\f$).
 * @param frame Current frame number.
 * @param motion_est Motion estimation at \f$t\f$.
 * @param r_extrapol Accepted range for extrapolation.
 * @param angle_max Maximum angle that the 3 last positions of a same track can form (if the angle is higher than
 *                  \p angle_max then the track is classified as noise).
 * @param diff_dev Multiplication factor in the motion detection criterion.
 *                 Motion criterion is: \f$ |e_k - \bar{e_t}| > \texttt{diff\_dev} * \sigma_t, \f$ where \f$e_k\f$ is
 *                 the compensation error of the CC/RoI number \f$k\f$, \f$\bar{e_t}\f$ the average error of
 *                 compensation of all CCs of image \f$I_t\f$, and \f$\sigma_t\f$ the standard deviation of the error.
 * @param track_all Boolean that defines if the tracking should track other objects than only meteors.
 * @param fra_star_min Minimum number of CC/RoI associations before creating a star track.
 * @param fra_meteor_min Minimum number of CC/RoI associations before creating a meteor track.
 * @param fra_meteor_max Maximum number of CC/RoI associations after which a meteor track is transformed in a noise
 *                       track.
 * @param save_RoIs_id Boolean to save the list of the RoI ids for each tracks.
 * @param extrapol_order_max Maximum number of frames where a lost track is extrapolated (0 means no extrapolation).
 * @param min_extrapol_ratio_S Minimum ratio between two RoIs. \f$ r_S = RoI_{S}^j / RoI_{S}^i\f$, if
 *                             \f$r_S < r_S^{min}\f$ then the association for the extrapolation is not made.
 * @param min_ellipse_ratio Minimum ellipse ratio of a meteor (for classification). If `0` then this parameter is
 *                          ignored. `RoIs->misc->a` and `RoIs->misc->b` can't be NULL.
 */
void tracking_perform(tracking_data_t* tracking_data, const RoIs_t* RoIs, size_t frame, const motion_t* motion_est,
                      const size_t r_extrapol, const float angle_max, const float diff_dev, const int track_all,
                      const size_t fra_star_min, const size_t fra_meteor_min, const size_t fra_meteor_max,
                      const uint8_t save_RoIs_id, const uint8_t extrapol_order_max, const float min_extrapol_ratio_S,
                      const float min_ellipse_ratio);
