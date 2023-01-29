#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "fmdt/macros.h"
#include "vec.h"

#include "fmdt/motion/motion_compute.h"

void _motion_rigid_registration_corrected(const float* ROI0_x, const float* ROI0_y, const float* ROI1_x,
                                          const float* ROI1_y, const float* ROI1_error, const uint32_t* ROI1_prev_id,
                                          uint8_t* ROI1_is_moving, const size_t n_ROI1, float* theta, float* tx,
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
    for (size_t i = 0; i < n_ROI1; i++) {
        if (ROI1_error != NULL && fabs(ROI1_error[i] - mean_error) > std_deviation) {
            ROI1_is_moving[i] = 1;
            continue;
        }
        asso = ROI1_prev_id[i];
        if (asso) {
            Sx += ROI0_x[asso - 1];
            Sy += ROI0_y[asso - 1];
            Sxp += ROI1_x[i];
            Syp += ROI1_y[i];
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
    for (size_t i = 0; i < n_ROI1; i++) {
        if (ROI1_is_moving != NULL && ROI1_is_moving[i])
            continue;
        asso = ROI1_prev_id[i];
        if (asso) {
            x0 = ROI0_x[asso - 1] - xg;
            y0 = ROI0_y[asso - 1] - yg;
            x1 = ROI1_x[i] - xpg;
            y1 = ROI1_y[i] - ypg;

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

void _motion_rigid_registration(const float* ROI0_x, const float* ROI0_y, const float* ROI1_x, const float* ROI1_y,
                                const uint32_t* ROI1_prev_id, const size_t n_ROI1, float* theta, float* tx,
                                float* ty) {
    _motion_rigid_registration_corrected(ROI0_x, ROI0_y, ROI1_x, ROI1_y, NULL, ROI1_prev_id, NULL, n_ROI1, theta, tx,
                                         ty, 0.f, 0.f);
}

// TODO: Pour l'optimisation : faire une version errorMoy_corrected()
float _motion_compute_mean_error(const float* ROI_error, const uint32_t* ROI_prev_id, const uint8_t* ROI_is_moving,
                                 const size_t n_ROI) {
    float S = 0.f;
    int cpt = 0;
    for (size_t i = 0; i < n_ROI; i++) {
        if (ROI_is_moving[i] || !ROI_prev_id[i])
            continue;
        S += ROI_error[i];
        cpt++;
    }
    return S / cpt;
}

float motion_compute_mean_error(const ROI_asso_t* ROI_asso_array, const ROI_motion_t* ROI_motion_array) {
    return _motion_compute_mean_error(ROI_motion_array->error, ROI_asso_array->prev_id, ROI_motion_array->is_moving,
                                      *ROI_motion_array->_size);
}

// TODO: Pour l'optimisation : faire une version ecartType_corrected()
float _motion_compute_std_deviation(const float* ROI_error, const uint32_t* ROI_prev_id, const uint8_t* ROI_is_moving,
                                    const size_t n_ROI, const float mean_error) {
    float S = 0.f;
    int cpt = 0;
    for (size_t i = 0; i < n_ROI; i++) {
        if (ROI_is_moving[i] || !ROI_prev_id[i])
            continue;
        float e = ROI_error[i];
        S += ((e - mean_error) * (e - mean_error));
        cpt++;
    }
    return sqrt(S / cpt);
}

float motion_compute_std_deviation(const ROI_asso_t* ROI_asso_array, const ROI_motion_t* ROI_motion_array,
                                   const float mean_error) {
    return _motion_compute_std_deviation(ROI_motion_array->error, ROI_asso_array->prev_id,
                                         ROI_motion_array->is_moving, *ROI_motion_array->_size, mean_error);
}

void _motion_extraction(const float* ROI0_x, const float* ROI0_y, const float* ROI1_x, const float* ROI1_y,
                       float* ROI1_dx, float* ROI1_dy, float* ROI1_error, const uint32_t* ROI1_prev_id,
                       const size_t n_ROI1, float theta, float tx, float ty) {
    int cc0;
    float x, y, xp, yp;
    float dx, dy;
    float e;

    for (size_t i = 0; i < n_ROI1; i++) {
        cc0 = ROI1_prev_id[i];
        if (cc0) {
            // coordonees du point dans l'image I+1
            xp = ROI1_x[i];
            yp = ROI1_y[i];
            // calcul de (x,y) pour l'image I
            x = cosf(theta) * (xp - tx) + sinf(theta) * (yp - ty);
            y = cosf(theta) * (yp - ty) - sinf(theta) * (xp - tx);

            dx = x - ROI0_x[cc0 - 1];
            dy = y - ROI0_y[cc0 - 1];
            ROI1_dx[i] = dx;
            ROI1_dy[i] = dy;

            e = sqrtf(dx * dx + dy * dy);
            ROI1_error[i] = e;
        }
    }
}

void _motion_compute(const float* ROI0_x, const float* ROI0_y, const float* ROI1_x, const float* ROI1_y,
                     float* ROI1_dx, float* ROI1_dy, float* ROI1_error, const uint32_t* ROI1_prev_id,
                     uint8_t* ROI1_is_moving, const size_t n_ROI1, motion_t* motion_est1,
                     motion_t* motion_est2) {
    memset(ROI1_is_moving, 0, n_ROI1 * sizeof(uint8_t));

    _motion_rigid_registration(ROI0_x, ROI0_y, ROI1_x, ROI1_y, ROI1_prev_id, n_ROI1, &motion_est1->theta,
                               &motion_est1->tx, &motion_est1->ty);
    _motion_extraction(ROI0_x, ROI0_y, ROI1_x, ROI1_y, ROI1_dx, ROI1_dy, ROI1_error, ROI1_prev_id, n_ROI1,
                       motion_est1->theta, motion_est1->tx, motion_est1->ty);
    motion_est1->mean_error = _motion_compute_mean_error(ROI1_error, ROI1_prev_id, ROI1_is_moving, n_ROI1);
    motion_est1->std_deviation = _motion_compute_std_deviation(ROI1_error, ROI1_prev_id, ROI1_is_moving, n_ROI1,
                                                               motion_est1->mean_error);
    // saveErrorMoy("first_error.txt", mean_error, std_deviation);
    _motion_rigid_registration_corrected(ROI0_x, ROI0_y, ROI1_x, ROI1_y, ROI1_error, ROI1_prev_id, ROI1_is_moving,
                                         n_ROI1, &motion_est2->theta, &motion_est2->tx, &motion_est2->ty,
                                         motion_est1->mean_error, motion_est1->std_deviation);
    _motion_extraction(ROI0_x, ROI0_y, ROI1_x, ROI1_y, ROI1_dx, ROI1_dy, ROI1_error, ROI1_prev_id, n_ROI1,
                       motion_est2->theta, motion_est2->tx, motion_est2->ty);
    motion_est2->mean_error = _motion_compute_mean_error(ROI1_error, ROI1_prev_id, ROI1_is_moving, n_ROI1);
    motion_est2->std_deviation = _motion_compute_std_deviation(ROI1_error, ROI1_prev_id, ROI1_is_moving, n_ROI1,
                                 motion_est2->mean_error);
}

void motion_compute(const ROI_basic_t* ROI_basic_array0, const ROI_basic_t* ROI_basic_array1,
                    const ROI_asso_t* ROI_asso_array1, ROI_motion_t* ROI_motion_array1, motion_t* motion_est1,
                    motion_t* motion_est2) {
    _motion_compute(ROI_basic_array0->x, ROI_basic_array0->y, ROI_basic_array1->x, ROI_basic_array1->y,
                    ROI_motion_array1->dx, ROI_motion_array1->dy, ROI_motion_array1->error, ROI_asso_array1->prev_id,
                    ROI_motion_array1->is_moving, *ROI_asso_array1->_size, motion_est1, motion_est2);
}
