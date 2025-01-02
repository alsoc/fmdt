/*!
 * \file
 * \brief Global motion structure.
 */

#pragma once

/**
 *  Store the transformation in a matrix.
 *  In FMDT we only consider "rigid body" transformations (= translation \f$\vec{t}\f$ + rotation of \f$\theta\f$
 *  angle). Here is the corresponding \f$3 \times 3\f$ transformation matrix:
 *  \f[
 *  T_{3 \times3 } =
 *  \begin{bmatrix}
 *  \cos(\theta) & -\sin(\theta) & t_x \\
 *  \sin(\theta) &  \cos(\theta) & t_y \\
 *             0 &            0  &   1 \\
 *  \end{bmatrix}.
 *  \f]
 *
 *  Some comprehensive explanations about 2D transformation matrices & homogeneous coordinates are given here:
 *  - https://staff.fnwi.uva.nl/r.vandenboomgaard/IPCV20162017/LectureNotes/MATH/homogenous.html
 *
 *  The order of the fields in the structure is important because the structure can easily be "cast" in a 2-dimensional
 *  \f$3 \times 3\f$ matrix for transformation combinations (= \f$3 \times 3\f$ matrix multiplication) & positions
 *  update (= \f$3 \times 3\f$ transformation matrix multiplied by \f$3 \times 1\f$ position vector).
 */
typedef struct {
    float cos_theta;     /*!< \f$  \cos(\theta) \f$ (\f$[0][0]\f$ element in the \f$T\f$ matrix). */
    float neg_sin_theta; /*!< \f$ -\sin(\theta) \f$ (\f$[0][1]\f$ element in the \f$T\f$ matrix). */
    float tx;            /*!< Abscissa component \f$ t_x \f$ of the translation vector \f$\vec{t}\f$ (\f$[0][2]\f$ element in the \f$T\f$ matrix). */
    float sin_theta;     /*!< \f$ \sin(\theta) \f$ (\f$[1][0]\f$ element in the \f$T\f$ matrix). */
    float cos_theta2;    /*!< \f$ \cos(\theta) \f$, same value as previous `cos_theta` field (\f$[1][1]\f$ element in the \f$T\f$ matrix). */
    float ty;            /*!< Ordinate component \f$ t_y \f$ of the translation vector \f$\vec{t}\f$  (\f$[1][2]\f$ element in the \f$T\f$ matrix). */
    float zero1;         /*!< As only rigid transformations are considered in FMDT, this field should always be null (\f$[2][0]\f$ element in the \f$T\f$ matrix). */
    float zero2;         /*!< As only rigid transformations are considered in FMDT, this field should always be null (\f$[2][1]\f$ element in the \f$T\f$ matrix). */
    float one;           /*!< Always set to one in transformation matrices (\f$[2][0]\f$ element in the \f$T\f$ matrix). */
} tmat3x3_t;

/**
 *  Homogeneous coordinates in 2D.
 *
 *  Some comprehensive explanations about 2D transformation matrices & homogeneous coordinates are given here:
 *  - https://staff.fnwi.uva.nl/r.vandenboomgaard/IPCV20162017/LectureNotes/MATH/homogenous.html
 *
 *  The order of the fields in the structure is important because the structure can easily be "cast" in a position
 *  vector.
 */
typedef struct {
    float x; /*!< Abscissa component of the coordinates. */
    float y; /*!< Ordinate component of the coordinates. */
    float one; /*!< Always set to one in homogeneous coordinates (to be compatible with \f$ 3 \times 3 \f$
                *   transformation matrices). */
} hcoord_t;

/**
 *  Initialize a `tmat3x3_t` transformation matrix from \f$\cos(\theta)\f$, \f$\sin(\theta)\f$ and \f$\vec{t}\f$.
 *  The produced matrix is specialized for rigid transformations.
 *
 *  @param tmat An allocated `tmat3x3_t` transformation matrix.
 *  @param cos_theta \f$\cos(\theta)\f$ with \f$\theta\f$ the rotation angle.
 *  @param sin_theta \f$\sin(\theta)\f$ with \f$\theta\f$ the rotation angle.
 *  @param tx Abscissa component of the translation vector \f$\vec{t}\f$.
 *  @param ty Ordinate component of the translation vector \f$\vec{t}\f$.
 */
void motion_init_tmat3x3(tmat3x3_t* tmat, const float cos_theta, const float sin_theta, const float tx, const float ty);

/**
 *  Initialize a `tmat3x3_t` identity matrix (= no transformation).
 *
 *  \f[
 *  T_{3 \times3 }^{identity} =
 *  \begin{bmatrix}
 *  1 & 0 & 0 \\
 *  0 & 1 & 0 \\
 *  0 & 0 & 1 \\
 *  \end{bmatrix}.
 *  \f]
 *
 *  @param tmat An allocated `tmat3x3_t` transformation matrix.
 */
void motion_init_tmat3x3_identity(tmat3x3_t* tmat);

/**
 *  Extract the rotation angle \f$\theta\f$ from a `tmat3x3_t` transformation matrix.
 *  Compute the arc tangent of \f$ y / x \f$ (= \f$\arctan(\frac{\sin(\theta)}{\cos(\theta)})\f$).
 *
 *  @param tmat An allocated `tmat3x3_t` and initialized transformation matrix.
 *
 *  @return \f$\theta\f$ angle in radians (range: \f$[-\pi;+\pi]\f$).
 */
float motion_get_theta(const tmat3x3_t* tmat);

/**
 *  Considering a position \f$q1\f$, computes the new position \f$q2\f$ after applying a `tmat3x3_t` transformation
 *  matrix. This implementation is optimized for rigid transformations only.
 *
 *  \f[
 *  T_{3 \times3 } ~~ . ~ \begin{bmatrix} q1_x \\ q1_y \\ 1 \end{bmatrix} = \begin{bmatrix} q2_x \\ q2_y \\ 1 \end{bmatrix}.
 *  \f]
 *
 *  @param tmat An allocated and initialized `tmat3x3_t` transformation matrix.
 *  @param x Abscissa component of the initial position \f$q1\f$.
 *  @param y Ordinate component of the initial position \f$q1\f$.
 *
 *  @return The new position \f$q2\f$ after the transformation (returned as `hcoord_t` homogeneous coordinates).
 */
hcoord_t motion_update_pos(const tmat3x3_t* tmat, const float x, const float y);

/**
 *  Compute the resulting transformation from two `tmat3x3_t` transformation matrices:
 *  \f[
 *  M_{3 \times 3}^{t_{-1} \rightarrow t} . M_{3 \times 3}^{t_{-2} \rightarrow t_{-1}} = M_{3 \times 3}^{t_{-2} \rightarrow t}.
 *  \f]
 *
 *  This implementation is optimized for rigid transformations only.
 *
 *  @param tmat0 An allocated and initialized `tmat3x3_t` transformation matrix (\f$M_{3 \times 3}^{t_{-1} \rightarrow t}\f$).
 *  @param tmat1 An allocated and initialized `tmat3x3_t` transformation matrix (\f$M_{3 \times 3}^{t_{-2} \rightarrow t_{-1}}\f$).
 *  @param tmat0_1 An allocated `tmat3x3_t` transformation matrix that is filled with the resulting combined
 *                 transformation (\f$M_{3 \times 3}^{t_{-2} \rightarrow t}\f$).
 */
void motion_combine_tmat3x3(const tmat3x3_t* tmat0, const tmat3x3_t* tmat1, tmat3x3_t* tmat0_1);

/**
 *  Structure that defines the global motion estimation between two consecutive images at \f$t - 1\f$ and \f$t\f$.
 *  Mainly stores the rotation angle and the translation vector from \f$I_{t}\f$ to \f$I_{t - 1}\f$.
 */
typedef struct {
    tmat3x3_t tmat; /*!< Store the transformation in a matrix: contains rotation & translation transformations. */
    float mean_error; /*!< Mean error of the global motion estimation. */
    float std_deviation; /*!< Standard deviation of the global motion estimation. */
} motion_t;
