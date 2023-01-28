#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "fmdt/macros.h"
#include "vec.h"

#include "fmdt/features/features_compute.h"

ROI_basic_t* features_alloc_ROI_basic(const size_t max_size, uint32_t* ROI_id) {
    ROI_basic_t* ROI_basic_array = (ROI_basic_t*)malloc(sizeof(ROI_basic_t));
    ROI_basic_array->id = (ROI_id == NULL) ? (uint32_t*)malloc(max_size * sizeof(uint32_t)) : ROI_id;
    ROI_basic_array->xmin = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    ROI_basic_array->xmax = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    ROI_basic_array->ymin = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    ROI_basic_array->ymax = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    ROI_basic_array->S = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    ROI_basic_array->Sx = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    ROI_basic_array->Sy = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    ROI_basic_array->x = (float*)malloc(max_size * sizeof(float));
    ROI_basic_array->y = (float*)malloc(max_size * sizeof(float));
    if (ROI_id == NULL) {
        ROI_basic_array->_max_size = (size_t*)malloc(sizeof(size_t));
        ROI_basic_array->_size = (size_t*)malloc(sizeof(size_t));
        *ROI_basic_array->_max_size = max_size;
    }
    return ROI_basic_array;
}

void features_init_ROI_basic(ROI_basic_t* ROI_basic_array, const uint8_t init_id) {
    if (init_id)
        memset(ROI_basic_array->id, 0, *ROI_basic_array->_max_size * sizeof(uint32_t));
    memset(ROI_basic_array->xmin, 0, *ROI_basic_array->_max_size * sizeof(uint32_t));
    memset(ROI_basic_array->xmax, 0, *ROI_basic_array->_max_size * sizeof(uint32_t));
    memset(ROI_basic_array->ymin, 0, *ROI_basic_array->_max_size * sizeof(uint32_t));
    memset(ROI_basic_array->ymax, 0, *ROI_basic_array->_max_size * sizeof(uint32_t));
    memset(ROI_basic_array->S, 0, *ROI_basic_array->_max_size * sizeof(uint32_t));
    memset(ROI_basic_array->Sx, 0, *ROI_basic_array->_max_size * sizeof(uint32_t));
    memset(ROI_basic_array->Sy, 0, *ROI_basic_array->_max_size * sizeof(uint32_t));
    memset(ROI_basic_array->x, 0, *ROI_basic_array->_max_size * sizeof(float));
    memset(ROI_basic_array->y, 0, *ROI_basic_array->_max_size * sizeof(float));
    *ROI_basic_array->_size = 0;
}

void features_free_ROI_basic(ROI_basic_t* ROI_basic_array, const uint8_t free_id) {
    if (free_id) {
        free(ROI_basic_array->id);
        free(ROI_basic_array->_size);
        free(ROI_basic_array->_max_size);
    }
    free(ROI_basic_array->xmin);
    free(ROI_basic_array->xmax);
    free(ROI_basic_array->ymin);
    free(ROI_basic_array->ymax);
    free(ROI_basic_array->S);
    free(ROI_basic_array->Sx);
    free(ROI_basic_array->Sy);
    free(ROI_basic_array->x);
    free(ROI_basic_array->y);
    free(ROI_basic_array);
}

ROI_asso_t* features_alloc_ROI_asso(const size_t max_size, uint32_t* ROI_id) {
    ROI_asso_t* ROI_asso_array = (ROI_asso_t*)malloc(sizeof(ROI_asso_t));
    ROI_asso_array->id = (ROI_id == NULL) ? (uint32_t*)malloc(max_size * sizeof(uint32_t)) : ROI_id;
    ROI_asso_array->prev_id = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    ROI_asso_array->next_id = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    if (ROI_id == NULL) {
        ROI_asso_array->_max_size = (size_t*)malloc(sizeof(size_t));
        ROI_asso_array->_size = (size_t*)malloc(sizeof(size_t));
        *ROI_asso_array->_max_size = max_size;
    }
    return ROI_asso_array;
}

void features_init_ROI_asso(ROI_asso_t* ROI_asso_array, const uint8_t init_id) {
    if (init_id)
        memset(ROI_asso_array->id, 0, *ROI_asso_array->_max_size * sizeof(uint32_t));
    memset(ROI_asso_array->prev_id, 0, *ROI_asso_array->_max_size * sizeof(uint32_t));
    memset(ROI_asso_array->next_id, 0, *ROI_asso_array->_max_size * sizeof(uint32_t));
    *ROI_asso_array->_size = 0;
}

void features_free_ROI_asso(ROI_asso_t* ROI_asso_array, const uint8_t free_id) {
    if (free_id) {
        free(ROI_asso_array->id);
        free(ROI_asso_array->_size);
        free(ROI_asso_array->_max_size);
    }
    free(ROI_asso_array->prev_id);
    free(ROI_asso_array->next_id);
    free(ROI_asso_array);
}

ROI_motion_t* features_alloc_ROI_motion(const size_t max_size, uint32_t* ROI_id) {
    ROI_motion_t* ROI_motion_array = (ROI_motion_t*)malloc(sizeof(ROI_motion_t));
    ROI_motion_array->id = (ROI_id == NULL) ? (uint32_t*)malloc(max_size * sizeof(uint32_t)) : ROI_id;
    ROI_motion_array->dx = (float*)malloc(max_size * sizeof(float));
    ROI_motion_array->dy = (float*)malloc(max_size * sizeof(float));
    ROI_motion_array->error = (float*)malloc(max_size * sizeof(float));
    ROI_motion_array->is_moving = (uint8_t*)malloc(max_size * sizeof(uint8_t));
    if (ROI_id == NULL) {
        ROI_motion_array->_max_size = (size_t*)malloc(sizeof(size_t));
        ROI_motion_array->_size = (size_t*)malloc(sizeof(size_t));
        *ROI_motion_array->_max_size = max_size;
    }
    return ROI_motion_array;
}

void features_init_ROI_motion(ROI_motion_t* ROI_motion_array, const uint8_t init_id) {
    if (init_id)
        memset(ROI_motion_array->id, 0, *ROI_motion_array->_max_size * sizeof(uint32_t));
    memset(ROI_motion_array->dx, 0, *ROI_motion_array->_max_size * sizeof(float));
    memset(ROI_motion_array->dy, 0, *ROI_motion_array->_max_size * sizeof(float));
    memset(ROI_motion_array->error, 0, *ROI_motion_array->_max_size * sizeof(float));
    memset(ROI_motion_array->is_moving, 0, *ROI_motion_array->_max_size * sizeof(uint8_t));
    *ROI_motion_array->_size = 0;
}

void features_free_ROI_motion(ROI_motion_t* ROI_motion_array, const uint8_t free_id) {
    if (free_id) {
        free(ROI_motion_array->id);
        free(ROI_motion_array->_size);
        free(ROI_motion_array->_max_size);
    }
    free(ROI_motion_array->dx);
    free(ROI_motion_array->dy);
    free(ROI_motion_array->error);
    free(ROI_motion_array->is_moving);
    free(ROI_motion_array);
}

ROI_misc_t* features_alloc_ROI_misc(const size_t max_size, uint32_t* ROI_id) {
    ROI_misc_t* ROI_misc_array = (ROI_misc_t*)malloc(sizeof(ROI_misc_t));
    ROI_misc_array->id = (ROI_id == NULL) ? (uint32_t*)malloc(max_size * sizeof(uint32_t)) : ROI_id;
    ROI_misc_array->magnitude = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    if (ROI_id == NULL) {
        ROI_misc_array->_max_size = (size_t*)malloc(sizeof(size_t));
        ROI_misc_array->_size = (size_t*)malloc(sizeof(size_t));
        *ROI_misc_array->_max_size = max_size;
    }
    return ROI_misc_array;
}

void features_init_ROI_misc(ROI_misc_t* ROI_misc_array, const uint8_t init_id) {
    if (init_id)
        memset(ROI_misc_array->id, 0, *ROI_misc_array->_max_size * sizeof(uint32_t));
    memset(ROI_misc_array->magnitude, 0, *ROI_misc_array->_max_size * sizeof(uint32_t));
    *ROI_misc_array->_size = 0;
}

void features_free_ROI_misc(ROI_misc_t* ROI_misc_array, const uint8_t free_id) {
    if (free_id) {
        free(ROI_misc_array->id);
        free(ROI_misc_array->_size);
        free(ROI_misc_array->_max_size);
    }
    free(ROI_misc_array->magnitude);
    free(ROI_misc_array);
}

ROI_t* features_alloc_ROI(const size_t max_size) {
    ROI_t* ROI_array = (ROI_t*)malloc(sizeof(ROI_t));
    ROI_array->id = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    ROI_array->basic = features_alloc_ROI_basic(max_size, ROI_array->id);
    ROI_array->basic->_max_size = &ROI_array->_max_size;
    ROI_array->basic->_size = &ROI_array->_size;
    ROI_array->asso = features_alloc_ROI_asso(max_size, ROI_array->id);
    ROI_array->asso->_max_size = &ROI_array->_max_size;
    ROI_array->asso->_size = &ROI_array->_size;
    ROI_array->motion = features_alloc_ROI_motion(max_size, ROI_array->id);
    ROI_array->motion->_max_size = &ROI_array->_max_size;
    ROI_array->motion->_size = &ROI_array->_size;
    ROI_array->misc = features_alloc_ROI_misc(max_size, ROI_array->id);
    ROI_array->misc->_max_size = &ROI_array->_max_size;
    ROI_array->misc->_size = &ROI_array->_size;
    ROI_array->_max_size = max_size;
    return ROI_array;
}

void features_init_ROI(ROI_t* ROI_array) {
    memset(ROI_array->id, 0, ROI_array->_max_size * sizeof(uint32_t));
    const uint8_t init_id = 0;
    features_init_ROI_basic(ROI_array->basic, init_id);
    features_init_ROI_asso(ROI_array->asso, init_id);
    features_init_ROI_motion(ROI_array->motion, init_id);
    features_init_ROI_misc(ROI_array->misc, init_id);
    ROI_array->_size = 0;
}

void features_free_ROI(ROI_t* ROI_array) {
    free(ROI_array->id);
    const uint8_t free_id = 0;
    features_free_ROI_basic(ROI_array->basic, free_id);
    features_free_ROI_asso(ROI_array->asso, free_id);
    features_free_ROI_motion(ROI_array->motion, free_id);
    features_free_ROI_misc(ROI_array->misc, free_id);
    free(ROI_array);
}

void _features_extract(const uint32_t** img, const int i0, const int i1, const int j0, const int j1, uint32_t* ROI_id,
                       uint32_t* ROI_xmin, uint32_t* ROI_xmax, uint32_t* ROI_ymin, uint32_t* ROI_ymax, uint32_t* ROI_S,
                       uint32_t* ROI_Sx, uint32_t* ROI_Sy, float* ROI_x, float* ROI_y, const size_t n_ROI) {
    for (size_t i = 0; i < n_ROI; i++) {
        ROI_xmin[i] = j1;
        ROI_xmax[i] = j0;
        ROI_ymin[i] = i1;
        ROI_ymax[i] = i0;
    }

    memset(ROI_S, 0, n_ROI * sizeof(uint32_t));
    memset(ROI_Sx, 0, n_ROI * sizeof(uint32_t));
    memset(ROI_Sy, 0, n_ROI * sizeof(uint32_t));

    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            uint32_t e = (uint32_t)img[i][j];
            if (e > 0) {
                assert(e < MAX_ROI_SIZE_BEFORE_SHRINK);
                uint32_t r = e - 1;
                ROI_S[r] += 1;
                ROI_id[r] = e;
                ROI_Sx[r] += j;
                ROI_Sy[r] += i;
                if (j < (int)ROI_xmin[r])
                    ROI_xmin[r] = j;
                if (j > (int)ROI_xmax[r])
                    ROI_xmax[r] = j;
                if (i < (int)ROI_ymin[r])
                    ROI_ymin[r] = i;
                if (i > (int)ROI_ymax[r])
                    ROI_ymax[r] = i;
            }
        }
    }

    for (size_t i = 0; i < n_ROI; i++) {
        ROI_x[i] = (float)ROI_Sx[i] / (float)ROI_S[i];
        ROI_y[i] = (float)ROI_Sy[i] / (float)ROI_S[i];
    }
}

void features_extract(const uint32_t** img, const int i0, const int i1, const int j0, const int j1, const size_t n_ROI,
                      ROI_basic_t* ROI_basic_array) {
    const uint8_t init_id = 1;
    features_init_ROI_basic(ROI_basic_array, init_id);
    *ROI_basic_array->_size = n_ROI;
    _features_extract(img, i0, i1, j0, j1, ROI_basic_array->id, ROI_basic_array->xmin, ROI_basic_array->xmax,
                      ROI_basic_array->ymin, ROI_basic_array->ymax, ROI_basic_array->S, ROI_basic_array->Sx,
                      ROI_basic_array->Sy, ROI_basic_array->x, ROI_basic_array->y, *ROI_basic_array->_size);
}

void _features_merge_CCL_HI_v2(const uint32_t** in_labels, const uint8_t** img_HI, uint32_t** out_labels, const int i0,
                               const int i1, const int j0, const int j1, uint32_t* ROI_id, const uint32_t* ROI_xmin,
                               const uint32_t* ROI_xmax, const uint32_t* ROI_ymin, const uint32_t* ROI_ymax,
                               const uint32_t* ROI_S, const size_t n_ROI, const uint32_t S_min, const uint32_t S_max) {
    if ((void*)img_HI != (void*)out_labels)
        for (int i = i0; i <= i1; i++)
            for (int j = j0; j <= j1; j++)
                out_labels[i][j] = (uint32_t)img_HI[i][j];

    uint32_t x0, x1, y0, y1, id;
    uint32_t cur_label = 1;
    for (size_t i = 0; i < n_ROI; i++) {
        if (ROI_id[i]) {
            id = ROI_id[i];
            x0 = ROI_ymin[i];
            x1 = ROI_ymax[i];
            y0 = ROI_xmin[i];
            y1 = ROI_xmax[i];
            if (S_min > ROI_S[i] || ROI_S[i] > S_max) {
                ROI_id[i] = 0;
                for (uint32_t k = x0; k <= x1; k++) {
                    for (uint32_t l = y0; l <= y1; l++) {
                        if (in_labels[k][l] == id)
                            out_labels[k][l] = 0;
                    }
                }
                continue;
            }
            for (uint32_t k = x0; k <= x1; k++) {
                for (uint32_t l = y0; l <= y1; l++) {
                    if (out_labels[k][l]) {
                        for (k = x0; k <= x1; k++) {
                            for (l = y0; l <= y1; l++) {
                                if (in_labels[k][l] == id) {
                                    out_labels[k][l] = cur_label;
                                }
                            }
                        }
                        cur_label++;
                        goto next;
                    }
                }
            }
            ROI_id[i] = 0;
        next:;
        }
    }
}

void features_merge_CCL_HI_v2(const uint32_t** in_labels, const uint8_t** img_HI, uint32_t** out_labels, const int i0,
                              const int i1, const int j0, const int j1, ROI_basic_t* ROI_basic_array,
                              const uint32_t S_min, const uint32_t S_max) {
    _features_merge_CCL_HI_v2(in_labels, img_HI, out_labels, i0,i1, j0, j1, ROI_basic_array->id, ROI_basic_array->xmin,
                              ROI_basic_array->xmax, ROI_basic_array->ymin, ROI_basic_array->ymax, ROI_basic_array->S,
                              *ROI_basic_array->_size, S_min, S_max);
}

size_t _features_shrink_ROI_array(const uint32_t* ROI_src_id, const uint32_t* ROI_src_xmin,
                                  const uint32_t* ROI_src_xmax, const uint32_t* ROI_src_ymin,
                                  const uint32_t* ROI_src_ymax, const uint32_t* ROI_src_S, const uint32_t* ROI_src_Sx,
                                  const uint32_t* ROI_src_Sy, const float* ROI_src_x, const float* ROI_src_y,
                                  const size_t n_ROI_src, uint32_t* ROI_dest_id, uint32_t* ROI_dest_xmin,
                                  uint32_t* ROI_dest_xmax, uint32_t* ROI_dest_ymin, uint32_t* ROI_dest_ymax,
                                  uint32_t* ROI_dest_S, uint32_t* ROI_dest_Sx, uint32_t* ROI_dest_Sy, float* ROI_dest_x,
                                  float* ROI_dest_y) {
    size_t cpt = 0;
    for (size_t i = 0; i < n_ROI_src; i++) {
        if (ROI_src_id[i]) {
            assert(cpt < MAX_ROI_SIZE);
            ROI_dest_id[cpt] = cpt + 1;
            ROI_dest_xmin[cpt] = ROI_src_xmin[i];
            ROI_dest_xmax[cpt] = ROI_src_xmax[i];
            ROI_dest_ymin[cpt] = ROI_src_ymin[i];
            ROI_dest_ymax[cpt] = ROI_src_ymax[i];
            ROI_dest_S[cpt] = ROI_src_S[i];
            ROI_dest_Sx[cpt] = ROI_src_Sx[i];
            ROI_dest_Sy[cpt] = ROI_src_Sy[i];
            ROI_dest_x[cpt] = ROI_src_x[i];
            ROI_dest_y[cpt] = ROI_src_y[i];
            cpt++;
        }
    }
    return cpt;
}

void features_shrink_ROI_array(const ROI_basic_t* ROI_basic_array_src, ROI_basic_t* ROI_basic_array_dest) {
    *ROI_basic_array_dest->_size = _features_shrink_ROI_array(ROI_basic_array_src->id, ROI_basic_array_src->xmin,
                                                              ROI_basic_array_src->xmax, ROI_basic_array_src->ymin,
                                                              ROI_basic_array_src->ymax, ROI_basic_array_src->S,
                                                              ROI_basic_array_src->Sx, ROI_basic_array_src->Sy,
                                                              ROI_basic_array_src->x, ROI_basic_array_src->y,
                                                              *ROI_basic_array_src->_size, ROI_basic_array_dest->id,
                                                              ROI_basic_array_dest->xmin, ROI_basic_array_dest->xmax,
                                                              ROI_basic_array_dest->ymin, ROI_basic_array_dest->ymax,
                                                              ROI_basic_array_dest->S, ROI_basic_array_dest->Sx,
                                                              ROI_basic_array_dest->Sy, ROI_basic_array_dest->x,
                                                              ROI_basic_array_dest->y);
}

void _features_rigid_registration_corrected(const float* ROI0_x, const float* ROI0_y, const float* ROI1_x,
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

void _features_rigid_registration(const float* ROI0_x, const float* ROI0_y, const float* ROI1_x, const float* ROI1_y,
                                  const uint32_t* ROI1_prev_id, const size_t n_ROI1, float* theta, float* tx,
                                  float* ty) {
    _features_rigid_registration_corrected(ROI0_x, ROI0_y, ROI1_x, ROI1_y, NULL, ROI1_prev_id, NULL, n_ROI1, theta, tx,
                                           ty, 0.f, 0.f);
}

// TODO: Pour l'optimisation : faire une version errorMoy_corrected()
float _features_compute_mean_error(const float* ROI_error, const uint32_t* ROI_prev_id, const uint8_t* ROI_is_moving,
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

float features_compute_mean_error(const ROI_asso_t* ROI_asso_array, const ROI_motion_t* ROI_motion_array) {
    return _features_compute_mean_error(ROI_motion_array->error, ROI_asso_array->prev_id, ROI_motion_array->is_moving,
                                        *ROI_motion_array->_size);
}

// TODO: Pour l'optimisation : faire une version ecartType_corrected()
float _features_compute_std_deviation(const float* ROI_error, const uint32_t* ROI_prev_id, const uint8_t* ROI_is_moving,
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

float features_compute_std_deviation(const ROI_asso_t* ROI_asso_array, const ROI_motion_t* ROI_motion_array,
                                     const float mean_error) {
    return _features_compute_std_deviation(ROI_motion_array->error, ROI_asso_array->prev_id,
                                           ROI_motion_array->is_moving, *ROI_motion_array->_size, mean_error);
}

void _features_motion_extraction(const float* ROI0_x, const float* ROI0_y, const float* ROI1_x, const float* ROI1_y,
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

void _features_compute_motion(const float* ROI0_x, const float* ROI0_y, const float* ROI1_x, const float* ROI1_y,
                              float* ROI1_dx, float* ROI1_dy, float* ROI1_error, const uint32_t* ROI1_prev_id,
                              uint8_t* ROI1_is_moving, const size_t n_ROI1, motion_t* motion_est1,
                              motion_t* motion_est2) {
    memset(ROI1_is_moving, 0, n_ROI1 * sizeof(uint8_t));

    _features_rigid_registration(ROI0_x, ROI0_y, ROI1_x, ROI1_y, ROI1_prev_id, n_ROI1, &motion_est1->theta,
                                 &motion_est1->tx, &motion_est1->ty);
    _features_motion_extraction(ROI0_x, ROI0_y, ROI1_x, ROI1_y, ROI1_dx, ROI1_dy, ROI1_error, ROI1_prev_id, n_ROI1,
                                motion_est1->theta, motion_est1->tx, motion_est1->ty);
    motion_est1->mean_error = _features_compute_mean_error(ROI1_error, ROI1_prev_id, ROI1_is_moving, n_ROI1);
    motion_est1->std_deviation = _features_compute_std_deviation(ROI1_error, ROI1_prev_id, ROI1_is_moving, n_ROI1,
                                                                 motion_est1->mean_error);
    // saveErrorMoy("first_error.txt", mean_error, std_deviation);
    _features_rigid_registration_corrected(ROI0_x, ROI0_y, ROI1_x, ROI1_y, ROI1_error, ROI1_prev_id, ROI1_is_moving,
                                           n_ROI1, &motion_est2->theta, &motion_est2->tx, &motion_est2->ty,
                                           motion_est1->mean_error, motion_est1->std_deviation);
    _features_motion_extraction(ROI0_x, ROI0_y, ROI1_x, ROI1_y, ROI1_dx, ROI1_dy, ROI1_error, ROI1_prev_id, n_ROI1,
                                motion_est2->theta, motion_est2->tx, motion_est2->ty);
    motion_est2->mean_error = _features_compute_mean_error(ROI1_error, ROI1_prev_id, ROI1_is_moving, n_ROI1);
    motion_est2->std_deviation = _features_compute_std_deviation(ROI1_error, ROI1_prev_id, ROI1_is_moving, n_ROI1,
                                 motion_est2->mean_error);
}

void features_compute_motion(const ROI_basic_t* ROI_basic_array0, const ROI_basic_t* ROI_basic_array1,
                             const ROI_asso_t* ROI_asso_array1, ROI_motion_t* ROI_motion_array1,
                             motion_t* motion_est1, motion_t* motion_est2) {
    _features_compute_motion(ROI_basic_array0->x, ROI_basic_array0->y, ROI_basic_array1->x, ROI_basic_array1->y,
                             ROI_motion_array1->dx, ROI_motion_array1->dy,
                             ROI_motion_array1->error, ROI_asso_array1->prev_id, ROI_motion_array1->is_moving,
                             *ROI_asso_array1->_size, motion_est1, motion_est2);
}

void _features_compute_magnitude(const uint8_t** img, const uint32_t img_width, const uint32_t img_height,
                                 const uint32_t** labels, const uint32_t* ROI_xmin, const uint32_t* ROI_xmax,
                                 const uint32_t* ROI_ymin, const uint32_t* ROI_ymax, const uint32_t* ROI_S,
                                 uint32_t* ROI_magnitude, const size_t n_ROI) {
    // set all ROIs (Regions of Interest) magnitudes to 0
    memset(ROI_magnitude, 0, n_ROI * sizeof(uint32_t));
    // for each ROI (= Region of Interest = object)
    for (uint32_t r = 0; r < n_ROI; r++) {
        // width and height of the current ROI
        uint32_t w = (ROI_xmax[r] - ROI_xmin[r]) + 1;
        uint32_t h = (ROI_ymax[r] - ROI_ymin[r]) + 1;

        // bounding box around the ROI + extra space to consider local noise level
        // here this is important to cast 'ROI_ymin' and 'ROI_xmin' into signed integers because the subtraction with
        // 'h' or 'w' can result in a negative number
        uint32_t ymin = (int64_t)ROI_ymin[r] - h >          0 ? ROI_ymin[r] - h :          0;
        uint32_t ymax =          ROI_ymax[r] + h < img_height ? ROI_ymax[r] + h : img_height;
        uint32_t xmin = (int64_t)ROI_xmin[r] - w >          0 ? ROI_xmin[r] - w :          0;
        uint32_t xmax =          ROI_xmax[r] + w <  img_width ? ROI_xmax[r] + w :  img_width;

        uint32_t acc_noise = 0; // accumulate noisy pixels (= dark pixels)
        uint32_t count_noise = 0; // count the number of noisy pixels
        uint32_t count_px = 0; // count the number of pixels for the current ROI (= bright pixels)

        // moving in a square (bigger that the real bounding box) around the current ROI
        for (uint32_t i = ymin; i <= ymax; i++) {
            for (uint32_t j = xmin; j <= xmax; j++) {
                // get the label from the current pixel position
                // if l != 0 then it is a ROI, else it is a dark / noisy pixel
                uint32_t l = labels[i][j];
                // check if the current pixel belong to the current ROI (same bounding box can share multiple ROIs)
                if (l == r + 1) {
                    ROI_magnitude[r] += (uint32_t)img[i][j];
                    count_px++;
                } else if (l == 0) {
                    acc_noise += (uint32_t)img[i][j];
                    count_noise++;
                }
            }
        }
        assert(count_px == ROI_S[r]); // useless check, only for debugging purpose
        // compute mean noise value
        uint32_t noise = acc_noise / count_noise;
        // subtract mean noise to the current ROI (Region of Interest) magnitude
        ROI_magnitude[r] -= noise * ROI_S[r];
    }
}

void features_compute_magnitude(const uint8_t** img, const uint32_t img_width, const uint32_t img_height,
                                const uint32_t** labels, const ROI_basic_t* ROI_basic_array,
                                ROI_misc_t* ROI_misc_array) {
    _features_compute_magnitude(img, img_width, img_height, labels, ROI_basic_array->xmin, ROI_basic_array->xmax,
                                ROI_basic_array->ymin, ROI_basic_array->ymax, ROI_basic_array->S,
                                ROI_misc_array->magnitude, *ROI_misc_array->_size);
}
