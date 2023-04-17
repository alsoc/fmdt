/*!
 * \file
 * \brief Global motion structure.
 */

#pragma once

/**
 *  Structure that defines the global motion estimation between two consecutive images at \f$t - 1\f$ and \f$t\f$.
 *  These fields define an angle and a translation vector from \f$I_{t}\f$ to \f$I_{t - 1}\f$.
 */
typedef struct {
    float theta; /*!< Rotation angle in radian. */
    float tx; /*!< \f$x\f$ component of the translation vector. */
    float ty; /*!< \f$y\f$ component of the translation vector. */
    float mean_error; /*!< Mean error of the global motion estimation. */
    float std_deviation; /*!< Standard deviation of the global motion estimation. */
} motion_t;
