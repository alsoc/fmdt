/*!
 * \file
 * \brief Connected-Component Analysis (CCA) structures (this is also known as the *features*).
 * Generally these structures represents characteristics (= features) of the Regions of Interest (ROIs).
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

/**
 *  Maximum number of ROIs before `features_merge_CCL_HI_v2` selection.
 */
#define MAX_ROI_SIZE_BEFORE_SHRINK 65535

/**
 *  Maximum number of ROIs after `features_merge_CCL_HI_v2` selection.
 */
#define MAX_ROI_SIZE 400

/**
 *  Basic features: bounding boxes, surface & centroid.
 *  The memory layout is a Structure of Array (SoA), each field of the structure is an array of `_max_size` capacity
 *  (except for `_max_size` itself and `_size` fields).
 */
typedef struct {
    uint32_t* id; /**< ROI unique identifier, ROI identifier starts from 1, 0 means uninitialized. */
    uint32_t* xmin; /**< Minimum \f$x\f$ value of the bounding box. */
    uint32_t* xmax; /**< Maximum \f$x\f$ value of the bounding box. */
    uint32_t* ymin; /**< Minimum \f$y\f$ value of the bounding box. */
    uint32_t* ymax; /**< Maximum \f$y\f$ value of the bounding box. */
    uint32_t* S; /**< Number of points = ROI surface. */
    uint32_t* Sx; /**< Sum of \f$x\f$ properties. */
    uint32_t* Sy; /**< Sum of \f$y\f$ properties. */
    float* x; /**< Abscissa of the centroid = center of mass (\f$ x = S_x / S \f$). */
    float* y; /**< Ordinate of the centroid = center of mass (\f$ y = S_y / S \f$). */

    size_t* _size; /**< Current size/utilization of the fields.
                        Note: it is allocated on the heap but it represents only one value. */
    size_t* _max_size; /**< Maximum capacity of data that can be contained in the fields.
                            Note: it is allocated on the heap but it represents only one value. */
} ROI_basic_t;

/**
 *  Associations between ROI at \f$t - 1\f$, \f$t\f$ and \f$t + 1\f$. Generally these associations are computed by a
 *  \f$k\f$-Nearest Neighbors matching algorithm.
 *  The memory layout is a Structure of Array (SoA), each field of the structure is an array of `_max_size` capacity
 *  (except for `_max_size` itself and `_size` fields).
 */
typedef struct {
    uint32_t* id; /**< ROI unique identifier, ROI identifier starts from 1, 0 means uninitialized. */
    uint32_t* prev_id; /**< Associated ROI identifier at \f$t - 1 \f$. */
    uint32_t* next_id; /**< Associated ROI identifier at \f$t + 1 \f$. */

    size_t* _size; /**< Current size/utilization of the fields.
                        Note: it is allocated on the heap but it represents only one value. */
    size_t* _max_size; /**< Maximum capacity of data that can be contained in the fields.
                            Note: it is allocated on the heap but it represents only one value. */
} ROI_asso_t;

/**
 *  Motion between ROI at \f$t - 1\f$ and \f$t\f$.
 *  The memory layout is a Structure of Array (SoA), each field of the structure is an array of `_max_size` capacity
 *  (except for `_max_size` itself and `_size` fields).
 */
typedef struct {
    uint32_t* id; /**< ROI unique identifier, ROI identifier starts from 1, 0 means uninitialized. */
    float* dx; /**< \f$x\f$ centroids distance between  between \f$t - 1\f$ and \f$t\f$.
                    This distance is computed after motion compensation. Thus, it represents either abscissa velocity
                    (if `is_moving` == 1) or abscissa error distance (if `is_moving` == 0). */
    float* dy; /**< \f$y\f$ centroids distance between between \f$t - 1\f$ and \f$t\f$.
                    This distance is computed after motion compensation. Thus, it represents either ordinate velocity
                    (if `is_moving` == 1) or ordinate error distance if (`is_moving` == 0). */
    float* error; /**< Velocity norm (if `is_moving` == 1) or error (if `is_moving` == 0). It is computed after motion
                       compensation. */
    uint8_t* is_moving; /**< Boolean that defines if the ROI is moving (`is_moving` == 1) or not moving (`is_moving`
                             == 0). */

    size_t* _size; /**< Current size/utilization of the fields.
                        Note: it is allocated on the heap but it represents only one value. */
    size_t* _max_size; /**< Maximum capacity of data that can be contained in the fields.
                            Note: it is allocated on the heap but it represents only one value. */
} ROI_motion_t;

/**
 *  Miscellaneous features.
 *  The memory layout is a Structure of Array (SoA), each field of the structure is an array of `_max_size` capacity
 *  (except for `_max_size` itself and `_size` fields).
 */
typedef struct {
    uint32_t* id; /**< ROI unique identifier, ROI identifier starts from 1, 0 means uninitialized. */
    uint32_t* magnitude; /**< Represent the ROI magnitude or brightness. Sum of the pixels intensities. */

    size_t* _size; /**< Current size/utilization of the fields.
                        Note: it is allocated on the heap but it represents only one value. */
    size_t* _max_size; /**< Maximum capacity of data that can be contained in the fields.
                            Note: it is allocated on the heap but it represents only one value. */
} ROI_misc_t;

/**
 *  Structure of ROI structures.
 *  This structure contains all previously defined ROI structures.
 *  `id`, `_size` and `_max_size` fields are shared with the sub-structures.
 */
typedef struct {
    uint32_t* id; /**< ROI unique identifier, ROI identifier starts from 1, 0 means uninitialized. */

    ROI_basic_t* basic; /**< Basic features: bounding boxes, surface & centroid. */
    ROI_asso_t* asso; /**< Associations between ROI at \f$t - 1\f$, \f$t\f$ and \f$t + 1\f$. */
    ROI_motion_t* motion; /**< Motion between ROI at \f$t - 1\f$ and \f$t\f$. */
    ROI_misc_t* misc; /**< Miscellaneous features. */

    size_t _size; /**< Current size/utilization of the fields. */
    size_t _max_size; /**< Maximum capacity of data that can be contained in the fields. */
} ROI_t;
