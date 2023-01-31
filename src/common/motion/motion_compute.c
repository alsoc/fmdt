#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "fmdt/macros.h"
#include "vec.h"

#include "fmdt/motion/motion_compute.h"

void _motion_rigid_registration_corrected(const float* RoI0_x, const float* RoI0_y, const float* RoI1_x,
                                          const float* RoI1_y, const float* RoI1_error, const uint32_t* RoI1_prev_id,
                                          uint8_t* RoI1_is_moving, const size_t n_RoI1, float* theta, float* tx,
                                          float* ty, float mean_error, float std_deviation) {
    float Sx, Sxp, Sy, Syp, Sx_xp, Sxp_y, Sx_yp, Sy_yp;
    float x0, y0, x1, y1;
    float a, b;
    float xg, yg, xpg, ypg;
    int asso;
    int cpt;

    Sx = 0;
    Sxp = 0;
    Sy = 0;
    Syp = 0;
    Sx_xp = 0;
    Sxp_y = 0;
    Sx_yp = 0;
    Sy_yp = 0;
    cpt = 0;

    // parcours tab assos
    for (size_t i = 0; i < n_RoI1; i++) {
        if (RoI1_error != NULL && fabs(RoI1_error[i] - mean_error) > std_deviation) {
            RoI1_is_moving[i] = 1;
            continue;
        }
        asso = RoI1_prev_id[i];
        if (asso) {
            Sx += RoI0_x[asso - 1];
            Sy += RoI0_y[asso - 1];
            Sxp += RoI1_x[i];
            Syp += RoI1_y[i];
            cpt++;
        }
    }

    xg = Sx / cpt;
    yg = Sy / cpt;
    xpg = Sxp / cpt;
    ypg = Syp / cpt;

    Sx = 0;
    Sxp = 0;
    Sy = 0;
    Syp = 0;

    // parcours tab assos
    for (size_t i = 0; i < n_RoI1; i++) {
        if (RoI1_is_moving != NULL && RoI1_is_moving[i])
            continue;
        asso = RoI1_prev_id[i];
        if (asso) {
            x0 = RoI0_x[asso - 1] - xg;
            y0 = RoI0_y[asso - 1] - yg;
            x1 = RoI1_x[i] - xpg;
            y1 = RoI1_y[i] - ypg;

            Sx += x0;
            Sy += y0;
            Sxp += x1;
            Syp += y1;
            Sx_xp += x0 * x1;
            Sxp_y += x1 * y0;
            Sx_yp += x0 * y1;
            Sy_yp += y0 * y1;
        }
    }
    a = cpt * cpt * (Sx_yp - Sxp_y) + (1 - 2 * cpt) * (Sx * Syp - Sxp * Sy);
    b = cpt * cpt * (Sx_xp + Sy_yp) + (1 - 2 * cpt) * (Sx * Sxp + Syp * Sy);

    *theta = atan2f(a, b);
    *tx = xpg - cosf(*theta) * xg + sinf(*theta) * yg;
    *ty = ypg - sinf(*theta) * xg - cosf(*theta) * yg;
}

void _motion_rigid_registration(const float* RoI0_x, const float* RoI0_y, const float* RoI1_x, const float* RoI1_y,
                                const uint32_t* RoI1_prev_id, const size_t n_RoI1, float* theta, float* tx,
                                float* ty) {
    _motion_rigid_registration_corrected(RoI0_x, RoI0_y, RoI1_x, RoI1_y, NULL, RoI1_prev_id, NULL, n_RoI1, theta, tx,
                                         ty, 0.f, 0.f);
}

// TODO: Pour l'optimisation : faire une version errorMoy_corrected()
float _motion_compute_mean_error(const float* RoI_error, const uint32_t* RoI_prev_id, const uint8_t* RoI_is_moving,
                                 const size_t n_RoI) {
    float S = 0.f;
    int cpt = 0;
    for (size_t i = 0; i < n_RoI; i++) {
        if (RoI_is_moving[i] || !RoI_prev_id[i])
            continue;
        S += RoI_error[i];
        cpt++;
    }
    return S / cpt;
}

float motion_compute_mean_error(const RoI_asso_t* RoI_asso_array, const RoI_motion_t* RoI_motion_array) {
    return _motion_compute_mean_error(RoI_motion_array->error, RoI_asso_array->prev_id, RoI_motion_array->is_moving,
                                      *RoI_motion_array->_size);
}

// TODO: Pour l'optimisation : faire une version ecartType_corrected()
float _motion_compute_std_deviation(const float* RoI_error, const uint32_t* RoI_prev_id, const uint8_t* RoI_is_moving,
                                    const size_t n_RoI, const float mean_error) {
    float S = 0.f;
    int cpt = 0;
    for (size_t i = 0; i < n_RoI; i++) {
        if (RoI_is_moving[i] || !RoI_prev_id[i])
            continue;
        float e = RoI_error[i];
        S += ((e - mean_error) * (e - mean_error));
        cpt++;
    }
    return sqrt(S / cpt);
}

float motion_compute_std_deviation(const RoI_asso_t* RoI_asso_array, const RoI_motion_t* RoI_motion_array,
                                   const float mean_error) {
    return _motion_compute_std_deviation(RoI_motion_array->error, RoI_asso_array->prev_id,
                                         RoI_motion_array->is_moving, *RoI_motion_array->_size, mean_error);
}

void _motion_extraction(const float* RoI0_x, const float* RoI0_y, const float* RoI1_x, const float* RoI1_y,
                       float* RoI1_dx, float* RoI1_dy, float* RoI1_error, const uint32_t* RoI1_prev_id,
                       const size_t n_RoI1, float theta, float tx, float ty) {
    int cc0;
    float x, y, xp, yp;
    float dx, dy;
    float e;

    for (size_t i = 0; i < n_RoI1; i++) {
        cc0 = RoI1_prev_id[i];
        if (cc0) {
            // coordonees du point dans l'image I+1
            xp = RoI1_x[i];
            yp = RoI1_y[i];
            // calcul de (x,y) pour l'image I
            x = cosf(theta) * (xp - tx) + sinf(theta) * (yp - ty);
            y = cosf(theta) * (yp - ty) - sinf(theta) * (xp - tx);

            dx = x - RoI0_x[cc0 - 1];
            dy = y - RoI0_y[cc0 - 1];
            RoI1_dx[i] = dx;
            RoI1_dy[i] = dy;

            e = sqrtf(dx * dx + dy * dy);
            RoI1_error[i] = e;
        }
    }
}

void _motion_compute(const float* RoI0_x, const float* RoI0_y, const float* RoI1_x, const float* RoI1_y,
                     float* RoI1_dx, float* RoI1_dy, float* RoI1_error, const uint32_t* RoI1_prev_id,
                     uint8_t* RoI1_is_moving, const size_t n_RoI1, motion_t* motion_est1,
                     motion_t* motion_est2) {
    memset(RoI1_is_moving, 0, n_RoI1 * sizeof(uint8_t));

    _motion_rigid_registration(RoI0_x, RoI0_y, RoI1_x, RoI1_y, RoI1_prev_id, n_RoI1, &motion_est1->theta,
                               &motion_est1->tx, &motion_est1->ty);
    _motion_extraction(RoI0_x, RoI0_y, RoI1_x, RoI1_y, RoI1_dx, RoI1_dy, RoI1_error, RoI1_prev_id, n_RoI1,
                       motion_est1->theta, motion_est1->tx, motion_est1->ty);
    motion_est1->mean_error = _motion_compute_mean_error(RoI1_error, RoI1_prev_id, RoI1_is_moving, n_RoI1);
    motion_est1->std_deviation = _motion_compute_std_deviation(RoI1_error, RoI1_prev_id, RoI1_is_moving, n_RoI1,
                                                               motion_est1->mean_error);
    // saveErrorMoy("first_error.txt", mean_error, std_deviation);
    _motion_rigid_registration_corrected(RoI0_x, RoI0_y, RoI1_x, RoI1_y, RoI1_error, RoI1_prev_id, RoI1_is_moving,
                                         n_RoI1, &motion_est2->theta, &motion_est2->tx, &motion_est2->ty,
                                         motion_est1->mean_error, motion_est1->std_deviation);
    _motion_extraction(RoI0_x, RoI0_y, RoI1_x, RoI1_y, RoI1_dx, RoI1_dy, RoI1_error, RoI1_prev_id, n_RoI1,
                       motion_est2->theta, motion_est2->tx, motion_est2->ty);
    motion_est2->mean_error = _motion_compute_mean_error(RoI1_error, RoI1_prev_id, RoI1_is_moving, n_RoI1);
    motion_est2->std_deviation = _motion_compute_std_deviation(RoI1_error, RoI1_prev_id, RoI1_is_moving, n_RoI1,
                                 motion_est2->mean_error);
}

void motion_compute(const RoI_basic_t* RoI_basic_array0, const RoI_basic_t* RoI_basic_array1,
                    const RoI_asso_t* RoI_asso_array1, RoI_motion_t* RoI_motion_array1, motion_t* motion_est1,
                    motion_t* motion_est2) {
    _motion_compute(RoI_basic_array0->x, RoI_basic_array0->y, RoI_basic_array1->x, RoI_basic_array1->y,
                    RoI_motion_array1->dx, RoI_motion_array1->dy, RoI_motion_array1->error, RoI_asso_array1->prev_id,
                    RoI_motion_array1->is_moving, *RoI_asso_array1->_size, motion_est1, motion_est2);
}
