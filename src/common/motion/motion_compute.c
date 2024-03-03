#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "fmdt/macros.h"
#include "vec.h"

#include "fmdt/motion/motion_compute.h"

void _motion_rigid_registration_corrected(const RoI_basic_t* RoIs0_basic, const RoI_basic_t* RoIs1_basic,
                                          const RoI_asso_t* RoIs1_asso, RoI_motion_t* RoIs1_motion,
                                          const size_t n_RoIs1, float* theta, float* tx, float* ty, float mean_error,
                                          float std_deviation) {
    float Sx = 0, Sxp = 0, Sy = 0, Syp = 0, Sx_xp = 0, Sxp_y = 0, Sx_yp = 0, Sy_yp = 0;
    int cpt = 0, asso;

    // parcours tab assos
    for (size_t i = 0; i < n_RoIs1; i++) {
        asso = RoIs1_asso[i].prev_id;
        if (asso) {
            if (RoIs1_motion != NULL && fabs(RoIs1_motion[i].error - mean_error) > std_deviation) {
                RoIs1_motion[i].is_moving = 1;
                continue;
            }

            Sx += RoIs0_basic[asso - 1].x;
            Sy += RoIs0_basic[asso - 1].y;
            Sxp += RoIs1_basic[i].x;
            Syp += RoIs1_basic[i].y;
            cpt++;
        }
    }

    float xg = Sx / cpt;
    float yg = Sy / cpt;
    float xpg = Sxp / cpt;
    float ypg = Syp / cpt;

    Sx = 0;
    Sxp = 0;
    Sy = 0;
    Syp = 0;

    // parcours tab assos
    for (size_t i = 0; i < n_RoIs1; i++) {
        if (RoIs1_motion != NULL && RoIs1_motion[i].is_moving)
            continue;
        asso = RoIs1_asso[i].prev_id;
        if (asso) {
            float x0 = RoIs0_basic[asso - 1].x - xg;
            float y0 = RoIs0_basic[asso - 1].y - yg;
            float x1 = RoIs1_basic[i].x - xpg;
            float y1 = RoIs1_basic[i].y - ypg;

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
    float a = cpt * cpt * (Sx_yp - Sxp_y) + (1 - 2 * cpt) * (Sx * Syp - Sxp * Sy);
    float b = cpt * cpt * (Sx_xp + Sy_yp) + (1 - 2 * cpt) * (Sx * Sxp + Syp * Sy);

    *theta = atan2f(a, b);
    *tx = xpg - cosf(*theta) * xg + sinf(*theta) * yg;
    *ty = ypg - sinf(*theta) * xg - cosf(*theta) * yg;
}

void _motion_rigid_registration(const RoI_basic_t* RoIs0_basic, const RoI_basic_t* RoIs1_basic,
                                const RoI_asso_t* RoIs1_asso, const size_t n_RoIs1, float* theta, float* tx,
                                float* ty) {
    _motion_rigid_registration_corrected(RoIs0_basic, RoIs1_basic, RoIs1_asso, NULL, n_RoIs1, theta, tx, ty, 0.f, 0.f);
}

// TODO: Pour l'optimisation : faire une version errorMoy_corrected()
float _motion_compute_mean_error(const RoI_asso_t* RoIs_asso, const RoI_motion_t* RoIs_motion, const size_t n_RoIs) {
    float S = 0.f;
    int cpt = 0;
    for (size_t i = 0; i < n_RoIs; i++) {
        if (RoIs_motion[i].is_moving || !RoIs_asso[i].prev_id)
            continue;
        S += RoIs_motion[i].error;
        cpt++;
    }
    return S / cpt;
}

// TODO: Pour l'optimisation : faire une version ecartType_corrected()
float _motion_compute_std_deviation(const RoI_asso_t* RoIs_asso, const RoI_motion_t* RoIs_motion, const size_t n_RoIs,
                                    const float mean_error) {
    float S = 0.f;
    int cpt = 0;
    for (size_t i = 0; i < n_RoIs; i++) {
        if (RoIs_motion[i].is_moving || !RoIs_asso[i].prev_id)
            continue;
        float e = RoIs_motion[i].error;
        S += ((e - mean_error) * (e - mean_error));
        cpt++;
    }
    return sqrt(S / cpt);
}

void _motion_extraction(const RoI_basic_t* RoIs0_basic, const RoI_basic_t* RoIs1_basic,
                        const RoI_asso_t* RoIs1_asso, RoI_motion_t* RoIs1_motion, const size_t n_RoIs1, float theta,
                        float tx, float ty) {
    int cc0;
    float x, y, xp, yp;
    float dx, dy;
    float e;

    for (size_t i = 0; i < n_RoIs1; i++) {
        cc0 = RoIs1_asso[i].prev_id;
        if (cc0) {
            // coordonees du point dans l'image I+1
            xp = RoIs1_basic[i].x;
            yp = RoIs1_basic[i].y;
            // calcul de (x,y) pour l'image I
            x = cosf(theta) * (xp - tx) + sinf(theta) * (yp - ty);
            y = cosf(theta) * (yp - ty) - sinf(theta) * (xp - tx);

            dx = x - RoIs0_basic[cc0 - 1].x;
            dy = y - RoIs0_basic[cc0 - 1].y;
            RoIs1_motion[i].dx = dx;
            RoIs1_motion[i].dy = dy;

            e = sqrtf(dx * dx + dy * dy);
            RoIs1_motion[i].error = e;
        }
    }
}

void motion_compute(const RoI_basic_t* RoIs0_basic, const RoI_basic_t* RoIs1_basic, const RoI_asso_t* RoIs1_asso,
                    RoI_motion_t* RoIs1_motion, const size_t n_RoIs1, motion_t* motion_est1, motion_t* motion_est2) {
    for (size_t r = 0; r < n_RoIs1; r++)
        RoIs1_motion[r].is_moving = 0;

    _motion_rigid_registration(RoIs0_basic, RoIs1_basic, RoIs1_asso, n_RoIs1, &motion_est1->theta, &motion_est1->tx,
                               &motion_est1->ty);
    _motion_extraction(RoIs0_basic, RoIs1_basic, RoIs1_asso, RoIs1_motion, n_RoIs1, motion_est1->theta, motion_est1->tx,
                       motion_est1->ty);
    motion_est1->mean_error = _motion_compute_mean_error(RoIs1_asso, RoIs1_motion, n_RoIs1);
    motion_est1->std_deviation = _motion_compute_std_deviation(RoIs1_asso, RoIs1_motion, n_RoIs1,
                                                               motion_est1->mean_error);
    _motion_rigid_registration_corrected(RoIs0_basic, RoIs1_basic, RoIs1_asso, RoIs1_motion, n_RoIs1,
                                         &motion_est2->theta, &motion_est2->tx, &motion_est2->ty,
                                         motion_est1->mean_error, motion_est1->std_deviation);
    _motion_extraction(RoIs0_basic, RoIs1_basic, RoIs1_asso, RoIs1_motion, n_RoIs1, motion_est2->theta, motion_est2->tx,
                       motion_est2->ty);
    motion_est2->mean_error = _motion_compute_mean_error(RoIs1_asso, RoIs1_motion, n_RoIs1);
    motion_est2->std_deviation = _motion_compute_std_deviation(RoIs1_asso, RoIs1_motion, n_RoIs1,
                                                               motion_est2->mean_error);
}
