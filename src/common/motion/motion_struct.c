#include <stddef.h>
#include <math.h>
#define _USE_MATH_DEFINES

#include "fmdt/motion/motion_struct.h"

void motion_init_tmat3x3(tmat3x3_t* tmat, const float cos_theta, const float sin_theta, const float tx,
                         const float ty) {
    tmat->cos_theta     = cos_theta;
    tmat->neg_sin_theta = -sin_theta;
    tmat->tx            = tx;
    tmat->sin_theta     = sin_theta;
    tmat->cos_theta2    = cos_theta;
    tmat->ty            = ty;
    tmat->zero1         = 0.f;
    tmat->zero2         = 0.f;
    tmat->one           = 1.f;
}

void motion_init_tmat3x3_v2(tmat3x3_t* tmat, const float theta, const float tx, const float ty) {
    float cos_theta = cosf(theta);
    float sin_theta = sinf(theta);

    tmat->cos_theta     = cos_theta;
    tmat->neg_sin_theta = -sin_theta;
    tmat->tx            = tx;
    tmat->sin_theta     = sin_theta;
    tmat->cos_theta2    = cos_theta;
    tmat->ty            = ty;
    tmat->zero1         = 0.f;
    tmat->zero2         = 0.f;
    tmat->one           = 1.f;
}

void motion_init_tmat3x3_identity(tmat3x3_t* tmat) {
    tmat->cos_theta     = 1.f;
    tmat->neg_sin_theta = 0.f;
    tmat->tx            = 0.f;
    tmat->sin_theta     = 0.f;
    tmat->cos_theta2    = 1.f;
    tmat->ty            = 0.f;
    tmat->zero1         = 0.f;
    tmat->zero2         = 0.f;
    tmat->one           = 1.f;
}

// return theta angle in radian
float motion_get_theta(const tmat3x3_t* tmat) {
    float angle_radian = atan2f(tmat->sin_theta, tmat->cos_theta);
    return angle_radian;
}

void motion_tmat3x3_to_2d_array_const(const tmat3x3_t* tmat, const float* tmat_2d_array[3]) {
    tmat_2d_array[0] = &tmat->cos_theta;
    tmat_2d_array[1] = &tmat->sin_theta;
    tmat_2d_array[2] = &tmat->zero1;
}

void motion_tmat3x3_to_2d_array(tmat3x3_t* tmat, float* tmat_2d_array[3]) {
    tmat_2d_array[0] = &tmat->cos_theta;
    tmat_2d_array[1] = &tmat->sin_theta;
    tmat_2d_array[2] = &tmat->zero1;
}

//                     [x ]
//                     [y ]
//   matrix * vector   [1 ]
//
// [cos(t) -sin(t) tx] [x'] = cos(t) * x - sin(t) * y + tx
// [sin(t)  cos(t) ty] [y'] = sin(t) * x + cos(t) * y + ty
// [     0       0  1] [1 ]
hcoord_t motion_update_pos(const tmat3x3_t* tmat, const float x, const float y) {
    hcoord_t hpos = {x, y, 1};

    const float* tmat_2d[3];
    motion_tmat3x3_to_2d_array_const(tmat, tmat_2d);

    float* vec_hpos = (float*)((void*)&hpos);

    hcoord_t updated_hpos;
    float* vec_uhpos = (float*)((void*)&updated_hpos);

    for (size_t i = 0; i < 3; i++)
    {
        float acc = 0.f;
        for (size_t j = 0; j < 3; j++)
            acc += tmat_2d[i][j] * vec_hpos[j];
        vec_uhpos[i] = acc;
    }

    return updated_hpos;
}

//                     [x ]
//                     [y ]
//   matrix * vector   [1 ]
//
// [cos(t) -sin(t) tx] [x'] = cos(t) * x - sin(t) * y + tx
// [sin(t)  cos(t) ty] [y'] = sin(t) * x + cos(t) * y + ty
// [     0       0  1] [1 ]
//
// optimized version for translation & rotation transformations only
hcoord_t motion_update_pos_opt(const tmat3x3_t* tmat, const float x, const float y) {
    hcoord_t updated_hpos;

    // optimized 'mat' x 'vec' op
    updated_hpos.x = tmat->cos_theta * x + tmat->neg_sin_theta * y + tmat->tx;
    updated_hpos.y = tmat->sin_theta * x + tmat->cos_theta     * y + tmat->ty;
    updated_hpos.one = 1.f;

    return updated_hpos;
}

// param: tmat0 (= transformation matix from t-2 to t-1),
// param: tmat1 (= transformation matix from t-1 to t),
// return: tmat0_1 (= transformation matrix from t-2 to t)
void motion_combine_tmat3x3(const tmat3x3_t* tmat0, const tmat3x3_t* tmat1, tmat3x3_t* tmat0_1) {
    const float* tmat0_2d[3];
    const float* tmat1_2d[3];
    float* tmat0_1_2d[3];
    motion_tmat3x3_to_2d_array_const(tmat0,    tmat0_2d  );
    motion_tmat3x3_to_2d_array_const(tmat1,    tmat1_2d  );
    motion_tmat3x3_to_2d_array      (tmat0_1,  tmat0_1_2d);

    for (size_t i = 0; i < 3; i++)
        for (size_t j = 0; j < 3; j++)
        {
            float acc = 0.f;
            for (size_t k = 0; k < 3; k++)
                acc += tmat0_2d[i][k] * tmat1_2d[k][j];
            tmat0_1_2d[i][j] = acc;
        }
}

// param: tmat0 (= transformation matix from t-2 to t-1),
// param: tmat1 (= transformation matix from t-1 to t),
// return: tmat0_1 (= transformation matrix from t-2 to t)
//
// optimized version for translation & rotation transformations only
//
//            a1 b1 c1
//            d1 e1 f1
//             0  0  1
//
// a0 b0 c0   a2 b2 c2
// d0 e0 f0   d2 e2 f2
//  0 0   1    0  0  1
void motion_combine_tmat3x3_opt(const tmat3x3_t* tmat0, const tmat3x3_t* tmat1, tmat3x3_t* tmat0_1) {
    float cos_theta0_x_cos_theta1 = tmat0->cos_theta * tmat1->cos_theta;
    tmat0_1->cos_theta = cos_theta0_x_cos_theta1 + tmat0->neg_sin_theta * tmat1->sin_theta;
    tmat0_1->neg_sin_theta = tmat0->cos_theta * tmat1->neg_sin_theta + tmat0->neg_sin_theta * tmat1->cos_theta;
    tmat0_1->tx = tmat0->cos_theta * tmat1->tx + tmat0->neg_sin_theta * tmat1->ty + tmat0->tx;
    tmat0_1->sin_theta = tmat0->sin_theta * tmat1->cos_theta + tmat0->cos_theta * tmat1->sin_theta;
    tmat0_1->cos_theta2 = tmat0->sin_theta * tmat1->neg_sin_theta + cos_theta0_x_cos_theta1;
    tmat0_1->ty = tmat0->sin_theta * tmat1->tx + tmat0->cos_theta * tmat1->ty + tmat0->ty;
    tmat0_1->zero1 = 0.f;
    tmat0_1->zero2 = 0.f;
    tmat0_1->one = 1.f;
}

