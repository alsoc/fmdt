/*!
 * \file
 * \brief Connected-Component Analysis (CCA) structures (this is also known as *features*).
 * Generally these structures represents characteristics (= *features*) of the Regions of Interest (RoIs).
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

/**
 *  Basic features: bounding box, surface & centroid.
 *  A bounding box represents a rectangular box around the RoI.
 *  The surface is the number of pixels that are in the connected-component (CC).
 *  The centroid is the center of mass of the RoI.
 */
typedef struct {
    uint32_t id; /**< RoI unique identifiers. A RoI identifier should starts from 1 while 0 should be reserved for
                      uninitialized structure. */
    uint32_t xmin; /**< Minimum \f$x\f$ coordinates of the bounding box. */
    uint32_t xmax; /**< Maximum \f$x\f$ coordinates of the bounding box. */
    uint32_t ymin; /**< Minimum \f$y\f$ coordinates of the bounding box. */
    uint32_t ymax; /**< Maximum \f$y\f$ coordinates of the bounding box. */
    uint32_t S; /**< Numbers of points/pixels = surfaces of the RoIs. */
    uint32_t Sx; /**< Sums of \f$x\f$ properties. */
    uint32_t Sy; /**< Sums of \f$y\f$ properties. */
    uint64_t Sx2; /**< Sums of squared \f$x\f$ properties. */
    uint64_t Sy2; /**< Sums of squared \f$x\f$ properties. */
    uint64_t Sxy; /**< Sums of \f$x \times y\f$ properties. */
    float x; /**< \f$x\f$ coordinates of the centroid (\f$ x = S_x / S \f$). */
    float y; /**< \f$y\f$ coordinates of the centroid (\f$ y = S_y / S \f$). */
} RoI_basic_t;

/**
 *  Associations between RoIs. \f$RoI_{t - 1} \leftrightarrow RoI_{t}\f$ and \f$ RoI_{t} \leftrightarrow RoI_{t + 1}\f$.
 *  Generally these associations are computed by a \f$k\f$-Nearest Neighbors (\f$k\f$-NN) matching algorithm.
 */
typedef struct {
    uint32_t prev_id; /**< Previous corresponding RoI identifiers (\f$RoI_{t - 1} \leftrightarrow RoI_{t}\f$). */
    uint32_t next_id; /**< Next corresponding RoI identifiers (\f$ RoI_{t} \leftrightarrow RoI_{t + 1}\f$). */
} RoI_asso_t;

/**
 *  Motion between RoI at \f$t - 1\f$ and \f$t\f$.
 *  The features of this structure are values computed after motion compensation.
 */
typedef struct {
    float dx; /**< \f$x\f$ components of the distance between centroids at \f$t - 1\f$ and \f$t\f$.
                   It can represent either abscissa velocity (if `is_moving` == 1) or abscissa error distance (if
                   `is_moving` == 0). */
    float dy; /**< \f$y\f$ components of the distance between centroids at \f$t - 1\f$ and \f$t\f$.
                   It can represent either ordinate velocity (if `is_moving` == 1) or ordinate error distance if
                   (`is_moving` == 0). */
    float error; /**< Velocity norm (if `is_moving` == 1) or error (if `is_moving` == 0).
                      \f$ e = \sqrt{dx^2 + dy^2} \f$. */
    uint8_t is_moving; /**< Boolean that defines if the RoI is moving (`is_moving` == 1) or not (`is_moving` == 0). */
} RoI_motion_t;

/**
 *  Magnitude features.
 */
typedef struct {
    uint32_t magnitude; /**< Magnitudes or brightness of the RoIs. Sums of the pixels intensities. */
    uint32_t sat_count; /**< Number of pixels that are saturated in the CC. A pixel is saturated if its intensity
                            \f$I_p\f$ is equal to the maximum value (here it is 255). */
} RoI_magn_t;

/**
 *  Ellipse features.
 */
typedef struct {
    float a; /**< Semi-major axis (ellipse) of the RoIs.*/
    float b; /**< Semi-minor axis (ellipse) of the RoIs.*/
} RoI_elli_t;

/**
 *  Structure of RoI structures.
 *  This structure contains arrays of all previously defined RoI structures.
 *
 * @see RoIs_basic_t.
 * @see RoIs_asso_t.
 * @see RoIs_motion_t.
 * @see RoIs_magn_t.
 * @see RoIs_elli_t.
 */
typedef struct {
    RoI_basic_t* basic; /**< Basic features. */
    RoI_asso_t* asso; /**< Association features. */
    RoI_motion_t* motion; /**< Motion features. */
    RoI_magn_t* magn; /**< Magnitude features. */
    RoI_elli_t* elli; /**< Ellipse features. */

    size_t _size; /**< Current size/utilization of the fields. */
    size_t _max_size; /**< Maximum capacity of data that can be contained in the fields. */
} RoIs_t;
