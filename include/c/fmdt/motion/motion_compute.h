/*!
 * \file
 * \brief Functions to estimate global motion and to compute per RoI motion / error.
 */

#pragma once

#include "fmdt/features/features_struct.h"
#include "fmdt/motion/motion_struct.h"

/**
 * Compute the global motion estimation and, after global motion compensation, compute the movement of each RoI.
 * In order to compute the motion estimation, the translation vector \f$(Tx, Ty)\f$ and the angle of rotation \f$\theta\f$
 * must be calculated as follows:
 * \f[
 * \theta = \tan^{-1}\left(\frac{ \sum_{i=1}^N [(y_i'-\bar{y})(x_i-\bar{x}) - (x_i'-\bar{x})(y_i-\bar{y})]}{\sum_{i=1}^N[(x_i'-\bar{x})(x_i-\bar{x}) + (y_i'-\bar{y})(y_i-\bar{y})]} \right),
 * \f]
 *
 * \f[
 * T = \begin{bmatrix} T_x \\ T_y \\ \end{bmatrix} =  \begin{bmatrix} x' - x \cdot cos(\theta) + y \cdot sin(\theta)\\ y' - x \cdot sin(\theta) - y \cdot cos(\theta) \end{bmatrix},
 * \f]
 * where \f$N\f$ is the number of RoIs,
 *        \f$(x,y)\f$ and \f$(x',y')\f$ are the centroids of RoIs at \f$t -1\f$ and \f$t\f$, respectively,  and\n
 *        \f[ \bar{x} = \sum_{i=1}^N x_i ~~~~~ \bar{y} = \sum_{i=1}^N y_i ~~~~~ \bar{x}' = \sum_{i=1}^N x'_i ~~~~~ \bar{y}' = \sum_{i=1}^N y'_i .
 *       \f]
 * For the first global motion estimation, all the associated RoIs are considered.
 * For the second global motion estimation, only the RoIs considered as "not moving" are considered.
 * To be considered in movement the motion norm of the RoI has to be higher that de motion standard deviation.
 *
 * @param RoIs0_basic Basic features (at \f$t -1\f$).
 * @param RoIs1_basic Basic features (at \f$t\f$).
 * @param RoIs1_asso Association features (at \f$t\f$).
 * @param RoIs1_motion Motion features (at \f$t\f$).
 * @param n_RoIs1 Number of connected-components (= number of RoIs) (at \f$t\f$).
 * @param motion_est1 First global motion estimation.
 * @param motion_est2 Second global motion estimation.
 */
void motion_compute(const RoI_basic_t* RoIs0_basic, const RoI_basic_t* RoIs1_basic, const RoI_asso_t* RoIs1_asso,
                    RoI_motion_t* RoIs1_motion, const size_t n_RoIs1, motion_t* motion_est1, motion_t* motion_est2);
