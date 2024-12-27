/*!
 * \file
 * \brief Global motion structure.
 */

#pragma once

/*!< Store the transformation in a matrix (see https://staff-fnwi-uva-nl.translate.goog/r.vandenboomgaard/IPCV20162017/LectureNotes/MATH/homogenous.html?_x_tr_sl=en&_x_tr_tl=fr&_x_tr_hl=fr&_x_tr_pto=rq#overview-of-2d-transforms)
 *   In FMDT we only consider "rigid body" transformation (= translation + rotation).
 *   Here is the contents of the matrix:
 *   | [0][0] -> cos(theta), [0][1] -> -sin(theta), [0][2] -> tx |
 *   | [1][0] -> sin(theta), [1][1] ->  cos(theta), [1][2] -> ty |
 *   | [2][0] ->          0, [2][1] ->           0, [2][2] ->  1 |.
 */
typedef struct {
    float cos_theta;     /*!< [0][0] */
    float neg_sin_theta; /*!< [0][1] */
    float tx;            /*!< [0][2] */
    float sin_theta;     /*!< [1][0] */
    float cos_theta2;    /*!< [1][1] */
    float ty;            /*!< [1][2] */
    float zero1;         /*!< [2][0] */
    float zero2;         /*!< [2][1] */
    float one;           /*!< [2][2] */
} tmat3x3_t;

// homogeneous coordinates
typedef struct {
    float x;
    float y;
    float one;
} hcoord_t;

void motion_init_tmat3x3(tmat3x3_t* tmat, const float cos_theta, const float sin_theta, const float tx, const float ty);

void motion_init_tmat3x3_v2(tmat3x3_t* tmat, const float theta, const float tx, const float ty);

void motion_init_tmat3x3_identity(tmat3x3_t* tmat);

void motion_tmat3x3_to_2d_array_const(const tmat3x3_t* tmat, const float* tmat_2d_array[3]);

void motion_tmat3x3_to_2d_array(tmat3x3_t* tmat, float* tmat_2d_array[3]);

float motion_get_theta(const tmat3x3_t* tmat);

hcoord_t motion_create_homo_pos(float x, float y);

hcoord_t motion_update_pos(const tmat3x3_t* tmat, const float x, const float y);

hcoord_t motion_update_pos_opt(const tmat3x3_t* tmat, const float x, const float y);

void motion_combine_tmat3x3(const tmat3x3_t* tmat0, const tmat3x3_t* tmat1, tmat3x3_t* tmat0_1);

void motion_combine_tmat3x3_opt(const tmat3x3_t* tmat0, const tmat3x3_t* tmat1, tmat3x3_t* tmat0_1);

/**
 *  Structure that defines the global motion estimation between two consecutive images at \f$t - 1\f$ and \f$t\f$.
 *  These fields define an angle and a translation vector from \f$I_{t}\f$ to \f$I_{t - 1}\f$.
 */
typedef struct {
    tmat3x3_t tmat; /*!< Store the transformation in a matrix: contains rotation & translation transformations. */
    float mean_error; /*!< Mean error of the global motion estimation. */
    float std_deviation; /*!< Standard deviation of the global motion estimation. */
} motion_t;
