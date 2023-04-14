/*!
 * \file
 * \brief Connected-Component Analysis (CCA) structures (this is also known as *features*).
 * Generally these structures represents characteristics (= *features*) of the Regions of Interest (RoIs).
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

/**
 *  Maximum number of RoIs before `features_merge_CCL_HI_v2` selection.
 */
#define MAX_ROI_SIZE_BEFORE_SHRINK 65535

/**
 *  Maximum number of RoIs after `features_merge_CCL_HI_v2` selection.
 */
#define MAX_ROI_SIZE 400

/**
 *  Basic features: bounding box, surface & centroid.
 *  A bounding box represents a rectangular box around the RoI.
 *  The surface is the number of pixels that are in the connected-component (CC).
 *  The centroid is the center of mass of the RoI.
 *  The memory layout is a Structure of Arrays (SoA), each field is an array of `_max_size` capacity (except for
 * `_max_size` itself and `_size` fields that are both scalar values).
 */
typedef struct {
    uint32_t* id; /**< RoI unique identifiers. A RoI identifier should starts from 1 while 0 should be reserved for
                       uninitialized structure. */
    uint32_t* xmin; /**< Minimum \f$x\f$ coordinates of the bounding box. */
    uint32_t* xmax; /**< Maximum \f$x\f$ coordinates of the bounding box. */
    uint32_t* ymin; /**< Minimum \f$y\f$ coordinates of the bounding box. */
    uint32_t* ymax; /**< Maximum \f$y\f$ coordinates of the bounding box. */
    uint32_t* S; /**< Numbers of points/pixels = surfaces of the RoIs. */
    uint32_t* Sx; /**< Sums of \f$x\f$ properties. */
    uint32_t* Sy; /**< Sums of \f$y\f$ properties. */
    uint64_t* Sx2; /**< Sums of squared \f$x\f$ properties. */
    uint64_t* Sy2; /**< Sums of squared \f$x\f$ properties. */
    uint64_t* Sxy; /**< Sums of \f$xy\f$ properties. */
    float* x; /**< \f$x\f$ coordinates of the centroid (\f$ x = S_x / S \f$). */
    float* y; /**< \f$y\f$ coordinates of the centroid (\f$ y = S_y / S \f$). */

    size_t* _size; /**< Current number of RoIs in each *feature* field.
                        Note: this field a pointer but it has to be a scalar value. */
    size_t* _max_size; /**< Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
                            Note: this field a pointer but it has to be a scalar value. */
} RoIs_basic_t;

/**
 *  Associations between RoIs. \f$RoI_{t - 1} \leftrightarrow RoI_{t}\f$ and \f$ RoI_{t} \leftrightarrow RoI_{t + 1}\f$.
 *  Generally these associations are computed by a \f$k\f$-Nearest Neighbors (\f$k\f$-NN) matching algorithm.
 *  The memory layout is a Structure of Arrays (SoA), each field is an array of `_max_size` capacity (except for
 * `_max_size` itself and `_size` fields that are both scalar values).
 */
typedef struct {
    uint32_t* id; /**< RoI unique identifiers. A RoI identifier should starts from 1 while 0 should be reserved for
                       uninitialized structure. */
    uint32_t* prev_id; /**< Previous corresponding RoI identifiers (\f$RoI_{t - 1} \leftrightarrow RoI_{t}\f$). */
    uint32_t* next_id; /**< Next corresponding RoI identifiers (\f$ RoI_{t} \leftrightarrow RoI_{t + 1}\f$). */

    size_t* _size; /**< Current number of RoIs in each *feature* field.
                        Note: this field a pointer but it has to be a scalar value. */
    size_t* _max_size; /**< Maximum capacity of each *feature* field (= maximum number of elements in the arrays).
                            Note: this field a pointer but it has to be a scalar value. */
} RoIs_asso_t;

/**
 *  Motion between RoI at \f$t - 1\f$ and \f$t\f$.
 *  The features of this structure are values computed after motion compensation.
 *  The memory layout is a Structure of Arrays (SoA), each field is an array of `_max_size` capacity (except for
 * `_max_size` itself and `_size` fields that are both scalar values).
 */
typedef struct {
    uint32_t* id; /**< RoI unique identifiers. A RoI identifier should starts from 1 while 0 should be reserved for
                       uninitialized structure. */
    float* dx; /**< \f$x\f$ components of the distance between centroids at \f$t - 1\f$ and \f$t\f$.
                    It can represent either abscissa velocity (if `is_moving` == 1) or abscissa error distance (if
                    `is_moving` == 0). */
    float* dy; /**< \f$y\f$ components of the distance between centroids at \f$t - 1\f$ and \f$t\f$.
                    It can represent either ordinate velocity (if `is_moving` == 1) or ordinate error distance if
                    (`is_moving` == 0). */
    float* error; /**< Velocity norm (if `is_moving` == 1) or error (if `is_moving` == 0).
                       \f$ e = \sqrt{dx^2 + dy^2} \f$. */
    uint8_t* is_moving; /**< Boolean that defines if the RoI is moving (`is_moving` == 1) or not (`is_moving` == 0). */

    size_t* _size; /**< Current size/utilization of the fields.
                        Note: it is allocated on the heap but it represents only one value. */
    size_t* _max_size; /**< Maximum capacity of data that can be contained in the fields.
                            Note: it is allocated on the heap but it represents only one value. */
} RoIs_motion_t;

/**
 *  Miscellaneous features.
 *  This structure contains features that are considered "less important" than others.
 *  The memory layout is a Structure of Arrays (SoA), each field is an array of `_max_size` capacity (except for
 * `_max_size` itself and `_size` fields that are both scalar values).
 */
typedef struct {
    uint32_t* id; /**< RoI unique identifiers. A RoI identifier should starts from 1 while 0 should be reserved for
                       uninitialized structure. */
    uint32_t* magnitude; /**< Magnitudes or brightness of the RoIs. Sums of the pixels intensities. */

    float* a; /**< Semi-major axis of the RoIs.*/
    float* b; /**< Semi-minor axis of the RoIs.*/

    size_t* _size; /**< Current size/utilization of the fields.
                        Note: it is allocated on the heap but it represents only one value. */
    size_t* _max_size; /**< Maximum capacity of data that can be contained in the fields.
                            Note: it is allocated on the heap but it represents only one value. */
} RoIs_misc_t;

/**
 *  Structure of RoI structures.
 *  This structure contains all previously defined RoI structures.
 *  `id`, `_size` and `_max_size` fields are shared with the sub-structures.
 * @see RoIs_basic_t.
 * @see RoIs_asso_t.
 * @see RoIs_motion_t.
 * @see RoIs_misc_t.
 */
typedef struct {
    uint32_t* id; /**< RoI unique identifiers. A RoI identifier should starts from 1 while 0 should be reserved for
                       uninitialized structure. */

    RoIs_basic_t* basic; /**< Basic features. */
    RoIs_asso_t* asso; /**< Association features. */
    RoIs_motion_t* motion; /**< Motion features. */
    RoIs_misc_t* misc; /**< Miscellaneous features. */

    size_t _size; /**< Current size/utilization of the fields. */
    size_t _max_size; /**< Maximum capacity of data that can be contained in the fields. */
} RoIs_t;
