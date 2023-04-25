#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "fmdt/macros.h"
#include "vec.h"

#include "fmdt/motion/motion_compute.h"

void _motion_rigid_registration_corrected(const float* RoIs0_x, const float* RoIs0_y, const float* RoIs1_x,
                                          const float* RoIs1_y, const float* RoIs1_error, const uint32_t* RoIs1_prev_id,
                                          uint8_t* RoIs1_is_moving, const size_t n_RoIs1, float* theta, float* tx,
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
    for (size_t i = 0; i < n_RoIs1; i++) {
        asso = RoIs1_prev_id[i];
        if (asso) {
            if (RoIs1_error != NULL && fabs(RoIs1_error[i] - mean_error) > std_deviation) {
                RoIs1_is_moving[i] = 1;
                continue;
            }

            Sx += RoIs0_x[asso - 1];
            Sy += RoIs0_y[asso - 1];
            Sxp += RoIs1_x[i];
            Syp += RoIs1_y[i];
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
    for (size_t i = 0; i < n_RoIs1; i++) {
        if (RoIs1_is_moving != NULL && RoIs1_is_moving[i])
            continue;
        asso = RoIs1_prev_id[i];
        if (asso) {
            x0 = RoIs0_x[asso - 1] - xg;
            y0 = RoIs0_y[asso - 1] - yg;
            x1 = RoIs1_x[i] - xpg;
            y1 = RoIs1_y[i] - ypg;

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

void _motion_rigid_registration(const float* RoIs0_x, const float* RoIs0_y, const float* RoIs1_x, const float* RoIs1_y,
                                const uint32_t* RoIs1_prev_id, const size_t n_RoIs1, float* theta, float* tx,
                                float* ty) {
    _motion_rigid_registration_corrected(RoIs0_x, RoIs0_y, RoIs1_x, RoIs1_y, NULL, RoIs1_prev_id, NULL, n_RoIs1, theta,
                                         tx, ty, 0.f, 0.f);
}

// TODO: Pour l'optimisation : faire une version errorMoy_corrected()
float _motion_compute_mean_error(const float* RoIs_error, const uint32_t* RoIs_prev_id, const uint8_t* RoIs_is_moving,
                                 const size_t n_RoIs) {
    float S = 0.f;
    int cpt = 0;
    for (size_t i = 0; i < n_RoIs; i++) {
        if (RoIs_is_moving[i] || !RoIs_prev_id[i])
            continue;
        S += RoIs_error[i];
        cpt++;
    }
    return S / cpt;
}

float motion_compute_mean_error(const RoIs_asso_t* RoIs_asso, const RoIs_motion_t* RoIs_motion) {
    return _motion_compute_mean_error(RoIs_motion->error, RoIs_asso->prev_id, RoIs_motion->is_moving,
                                      *RoIs_motion->_size);
}

// TODO: Pour l'optimisation : faire une version ecartType_corrected()
float _motion_compute_std_deviation(const float* RoIs_error, const uint32_t* RoIs_prev_id,
                                    const uint8_t* RoIs_is_moving, const size_t n_RoIs, const float mean_error) {
    float S = 0.f;
    int cpt = 0;
    for (size_t i = 0; i < n_RoIs; i++) {
        if (RoIs_is_moving[i] || !RoIs_prev_id[i])
            continue;
        float e = RoIs_error[i];
        S += ((e - mean_error) * (e - mean_error));
        cpt++;
    }
    return sqrt(S / cpt);
}

float motion_compute_std_deviation(const RoIs_asso_t* RoIs_asso, const RoIs_motion_t* RoIs_motion,
                                   const float mean_error) {
    return _motion_compute_std_deviation(RoIs_motion->error, RoIs_asso->prev_id,
                                         RoIs_motion->is_moving, *RoIs_motion->_size, mean_error);
}

void _motion_extraction(const float* RoIs0_x, const float* RoIs0_y, const float* RoIs1_x, const float* RoIs1_y,
                       float* RoIs1_dx, float* RoIs1_dy, float* RoIs1_error, const uint32_t* RoIs1_prev_id,
                       const size_t n_RoIs1, float theta, float tx, float ty) {
    int cc0;
    float x, y, xp, yp;
    float dx, dy;
    float e;

    for (size_t i = 0; i < n_RoIs1; i++) {
        cc0 = RoIs1_prev_id[i];
        if (cc0) {
            // coordonees du point dans l'image I+1
            xp = RoIs1_x[i];
            yp = RoIs1_y[i];
            // calcul de (x,y) pour l'image I
            x = cosf(theta) * (xp - tx) + sinf(theta) * (yp - ty);
            y = cosf(theta) * (yp - ty) - sinf(theta) * (xp - tx);

            dx = x - RoIs0_x[cc0 - 1];
            dy = y - RoIs0_y[cc0 - 1];
            RoIs1_dx[i] = dx;
            RoIs1_dy[i] = dy;

            e = sqrtf(dx * dx + dy * dy);
            RoIs1_error[i] = e;
        }
    }
}

void _motion_compute(const float* RoIs0_x, const float* RoIs0_y, const float* RoIs1_x, const float* RoIs1_y,
                     float* RoIs1_dx, float* RoIs1_dy, float* RoIs1_error, const uint32_t* RoIs1_prev_id,
                     uint8_t* RoIs1_is_moving, const size_t n_RoIs1, motion_t* motion_est1, motion_t* motion_est2) {
    memset(RoIs1_is_moving, 0, n_RoIs1 * sizeof(uint8_t));

    _motion_rigid_registration(RoIs0_x, RoIs0_y, RoIs1_x, RoIs1_y, RoIs1_prev_id, n_RoIs1, &motion_est1->theta,
                               &motion_est1->tx, &motion_est1->ty);
    _motion_extraction(RoIs0_x, RoIs0_y, RoIs1_x, RoIs1_y, RoIs1_dx, RoIs1_dy, RoIs1_error, RoIs1_prev_id, n_RoIs1,
                       motion_est1->theta, motion_est1->tx, motion_est1->ty);
    motion_est1->mean_error = _motion_compute_mean_error(RoIs1_error, RoIs1_prev_id, RoIs1_is_moving, n_RoIs1);
    motion_est1->std_deviation = _motion_compute_std_deviation(RoIs1_error, RoIs1_prev_id, RoIs1_is_moving, n_RoIs1,
                                                               motion_est1->mean_error);
    _motion_rigid_registration_corrected(RoIs0_x, RoIs0_y, RoIs1_x, RoIs1_y, RoIs1_error, RoIs1_prev_id,
                                         RoIs1_is_moving, n_RoIs1, &motion_est2->theta, &motion_est2->tx,
                                         &motion_est2->ty, motion_est1->mean_error, motion_est1->std_deviation);
    _motion_extraction(RoIs0_x, RoIs0_y, RoIs1_x, RoIs1_y, RoIs1_dx, RoIs1_dy, RoIs1_error, RoIs1_prev_id, n_RoIs1,
                       motion_est2->theta, motion_est2->tx, motion_est2->ty);
    motion_est2->mean_error = _motion_compute_mean_error(RoIs1_error, RoIs1_prev_id, RoIs1_is_moving, n_RoIs1);
    motion_est2->std_deviation = _motion_compute_std_deviation(RoIs1_error, RoIs1_prev_id, RoIs1_is_moving, n_RoIs1,
                                                               motion_est2->mean_error);
}

void motion_compute(const RoIs_basic_t* RoIs0_basic, const RoIs_basic_t* RoIs1_basic, const RoIs_asso_t* RoIs1_asso,
                    RoIs_motion_t* RoIs1_motion, motion_t* motion_est1, motion_t* motion_est2) {
    _motion_compute(RoIs0_basic->x, RoIs0_basic->y, RoIs1_basic->x, RoIs1_basic->y, RoIs1_motion->dx, RoIs1_motion->dy,
                    RoIs1_motion->error, RoIs1_asso->prev_id, RoIs1_motion->is_moving, *RoIs1_asso->_size, motion_est1,
                    motion_est2);
}
