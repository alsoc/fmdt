#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "fmdt/macros.h"
#include "vec.h"

#include "fmdt/features/features_compute.h"

RoIs_basic_t* features_alloc_RoIs_basic(const size_t max_size, uint32_t* RoIs_id) {
    RoIs_basic_t* RoIs_basic = (RoIs_basic_t*)malloc(sizeof(RoIs_basic_t));
    RoIs_basic->id = (RoIs_id == NULL) ? (uint32_t*)malloc(max_size * sizeof(uint32_t)) : RoIs_id;
    RoIs_basic->xmin = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoIs_basic->xmax = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoIs_basic->ymin = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoIs_basic->ymax = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoIs_basic->S = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoIs_basic->Sx = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoIs_basic->Sy = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoIs_basic->Sx2 = (uint64_t*)malloc(max_size * sizeof(uint64_t));
    RoIs_basic->Sy2 = (uint64_t*)malloc(max_size * sizeof(uint64_t));
    RoIs_basic->Sxy = (uint64_t*)malloc(max_size * sizeof(uint64_t));
    RoIs_basic->x = (float*)malloc(max_size * sizeof(float));
    RoIs_basic->y = (float*)malloc(max_size * sizeof(float));
    if (RoIs_id == NULL) {
        RoIs_basic->_max_size = (size_t*)malloc(sizeof(size_t));
        RoIs_basic->_size = (size_t*)malloc(sizeof(size_t));
        *RoIs_basic->_max_size = max_size;
    }
    return RoIs_basic;
}

void features_init_RoIs_basic(RoIs_basic_t* RoIs_basic, const uint8_t init_id) {
    if (init_id)
        memset(RoIs_basic->id, 0, *RoIs_basic->_max_size * sizeof(uint32_t));
    memset(RoIs_basic->xmin, 0, *RoIs_basic->_max_size * sizeof(uint32_t));
    memset(RoIs_basic->xmax, 0, *RoIs_basic->_max_size * sizeof(uint32_t));
    memset(RoIs_basic->ymin, 0, *RoIs_basic->_max_size * sizeof(uint32_t));
    memset(RoIs_basic->ymax, 0, *RoIs_basic->_max_size * sizeof(uint32_t));
    memset(RoIs_basic->S, 0, *RoIs_basic->_max_size * sizeof(uint32_t));
    memset(RoIs_basic->Sx, 0, *RoIs_basic->_max_size * sizeof(uint32_t));
    memset(RoIs_basic->Sy, 0, *RoIs_basic->_max_size * sizeof(uint32_t));
    memset(RoIs_basic->Sx2, 0, *RoIs_basic->_max_size * sizeof(uint64_t));
    memset(RoIs_basic->Sy2, 0, *RoIs_basic->_max_size * sizeof(uint64_t));
    memset(RoIs_basic->Sxy, 0, *RoIs_basic->_max_size * sizeof(uint64_t));
    memset(RoIs_basic->x, 0, *RoIs_basic->_max_size * sizeof(float));
    memset(RoIs_basic->y, 0, *RoIs_basic->_max_size * sizeof(float));
    *RoIs_basic->_size = 0;
}

void features_free_RoIs_basic(RoIs_basic_t* RoIs_basic, const uint8_t free_id) {
    if (free_id) {
        free(RoIs_basic->id);
        free(RoIs_basic->_size);
        free(RoIs_basic->_max_size);
    }
    free(RoIs_basic->xmin);
    free(RoIs_basic->xmax);
    free(RoIs_basic->ymin);
    free(RoIs_basic->ymax);
    free(RoIs_basic->S);
    free(RoIs_basic->Sx);
    free(RoIs_basic->Sy);
    free(RoIs_basic->Sx2);
    free(RoIs_basic->Sy2);
    free(RoIs_basic->Sxy);
    free(RoIs_basic->x);
    free(RoIs_basic->y);
    free(RoIs_basic);
}

RoIs_asso_t* features_alloc_RoIs_asso(const size_t max_size, uint32_t* RoIs_id) {
    RoIs_asso_t* RoIs_asso = (RoIs_asso_t*)malloc(sizeof(RoIs_asso_t));
    RoIs_asso->id = (RoIs_id == NULL) ? (uint32_t*)malloc(max_size * sizeof(uint32_t)) : RoIs_id;
    RoIs_asso->prev_id = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoIs_asso->next_id = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    if (RoIs_id == NULL) {
        RoIs_asso->_max_size = (size_t*)malloc(sizeof(size_t));
        RoIs_asso->_size = (size_t*)malloc(sizeof(size_t));
        *RoIs_asso->_max_size = max_size;
    }
    return RoIs_asso;
}

void features_init_RoIs_asso(RoIs_asso_t* RoIs_asso, const uint8_t init_id) {
    if (init_id)
        memset(RoIs_asso->id, 0, *RoIs_asso->_max_size * sizeof(uint32_t));
    memset(RoIs_asso->prev_id, 0, *RoIs_asso->_max_size * sizeof(uint32_t));
    memset(RoIs_asso->next_id, 0, *RoIs_asso->_max_size * sizeof(uint32_t));
    *RoIs_asso->_size = 0;
}

void features_free_RoIs_asso(RoIs_asso_t* RoIs_asso, const uint8_t free_id) {
    if (free_id) {
        free(RoIs_asso->id);
        free(RoIs_asso->_size);
        free(RoIs_asso->_max_size);
    }
    free(RoIs_asso->prev_id);
    free(RoIs_asso->next_id);
    free(RoIs_asso);
}

RoIs_motion_t* features_alloc_RoIs_motion(const size_t max_size, uint32_t* RoIs_id) {
    RoIs_motion_t* RoIs_motion = (RoIs_motion_t*)malloc(sizeof(RoIs_motion_t));
    RoIs_motion->id = (RoIs_id == NULL) ? (uint32_t*)malloc(max_size * sizeof(uint32_t)) : RoIs_id;
    RoIs_motion->dx = (float*)malloc(max_size * sizeof(float));
    RoIs_motion->dy = (float*)malloc(max_size * sizeof(float));
    RoIs_motion->error = (float*)malloc(max_size * sizeof(float));
    RoIs_motion->is_moving = (uint8_t*)malloc(max_size * sizeof(uint8_t));
    if (RoIs_id == NULL) {
        RoIs_motion->_max_size = (size_t*)malloc(sizeof(size_t));
        RoIs_motion->_size = (size_t*)malloc(sizeof(size_t));
        *RoIs_motion->_max_size = max_size;
    }
    return RoIs_motion;
}

void features_init_RoIs_motion(RoIs_motion_t* RoIs_motion, const uint8_t init_id) {
    if (init_id)
        memset(RoIs_motion->id, 0, *RoIs_motion->_max_size * sizeof(uint32_t));
    memset(RoIs_motion->dx, 0, *RoIs_motion->_max_size * sizeof(float));
    memset(RoIs_motion->dy, 0, *RoIs_motion->_max_size * sizeof(float));
    memset(RoIs_motion->error, 0, *RoIs_motion->_max_size * sizeof(float));
    memset(RoIs_motion->is_moving, 0, *RoIs_motion->_max_size * sizeof(uint8_t));
    *RoIs_motion->_size = 0;
}

void features_free_RoIs_motion(RoIs_motion_t* RoIs_motion, const uint8_t free_id) {
    if (free_id) {
        free(RoIs_motion->id);
        free(RoIs_motion->_size);
        free(RoIs_motion->_max_size);
    }
    free(RoIs_motion->dx);
    free(RoIs_motion->dy);
    free(RoIs_motion->error);
    free(RoIs_motion->is_moving);
    free(RoIs_motion);
}

RoIs_misc_t* features_alloc_RoIs_misc(const uint8_t enable_magnitude, const uint8_t enable_sat_count,
                                      const uint8_t enable_ellipse, const size_t max_size, uint32_t* RoIs_id) {
    RoIs_misc_t* RoIs_misc = (RoIs_misc_t*)malloc(sizeof(RoIs_misc_t));
    RoIs_misc->id = (RoIs_id == NULL) ? (uint32_t*)malloc(max_size * sizeof(uint32_t)) : RoIs_id;
    RoIs_misc->magnitude = enable_magnitude ? (uint32_t*)malloc(max_size * sizeof(uint32_t)) : NULL;
    RoIs_misc->sat_count = enable_sat_count ? (uint32_t*)malloc(max_size * sizeof(uint32_t)) : NULL;
    RoIs_misc->a = enable_ellipse ? (float*)malloc(max_size * sizeof(float)) : NULL;
    RoIs_misc->b = enable_ellipse ? (float*)malloc(max_size * sizeof(float)) : NULL;
    if (RoIs_id == NULL) {
        RoIs_misc->_max_size = (size_t*)malloc(sizeof(size_t));
        RoIs_misc->_size = (size_t*)malloc(sizeof(size_t));
        *RoIs_misc->_max_size = max_size;
    }
    return RoIs_misc;
}

void features_init_RoIs_misc(RoIs_misc_t* RoIs_misc, const uint8_t init_id) {
    if (init_id)
        memset(RoIs_misc->id, 0, *RoIs_misc->_max_size * sizeof(uint32_t));
    if (RoIs_misc->magnitude != NULL)
        memset(RoIs_misc->magnitude, 0, *RoIs_misc->_max_size * sizeof(uint32_t));
    if (RoIs_misc->sat_count != NULL)
        memset(RoIs_misc->sat_count, 0, *RoIs_misc->_max_size * sizeof(uint32_t));
    if (RoIs_misc->a != NULL)
        memset(RoIs_misc->a, 0, *RoIs_misc->_max_size * sizeof(float));
    if (RoIs_misc->b != NULL)
        memset(RoIs_misc->b, 0, *RoIs_misc->_max_size * sizeof(float));
    *RoIs_misc->_size = 0;
}

void features_free_RoIs_misc(RoIs_misc_t* RoIs_misc, const uint8_t free_id) {
    if (free_id) {
        free(RoIs_misc->id);
        free(RoIs_misc->_size);
        free(RoIs_misc->_max_size);
    }
    if (RoIs_misc->magnitude != NULL)
        free(RoIs_misc->magnitude);
    if (RoIs_misc->sat_count != NULL)
        free(RoIs_misc->sat_count);
    if (RoIs_misc->a != NULL)
        memset(RoIs_misc->a, 0, *RoIs_misc->_max_size * sizeof(float));
    if (RoIs_misc->b != NULL)
        memset(RoIs_misc->b, 0, *RoIs_misc->_max_size * sizeof(float));
    free(RoIs_misc);
}

RoIs_t* features_alloc_RoIs(const uint8_t enable_magnitude, const uint8_t enable_sat_count,
                            const uint8_t enable_ellipse, const size_t max_size) {
    RoIs_t* RoI = (RoIs_t*)malloc(sizeof(RoIs_t));
    RoI->id = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoI->basic = features_alloc_RoIs_basic(max_size, RoI->id);
    RoI->basic->_max_size = &RoI->_max_size;
    RoI->basic->_size = &RoI->_size;
    RoI->asso = features_alloc_RoIs_asso(max_size, RoI->id);
    RoI->asso->_max_size = &RoI->_max_size;
    RoI->asso->_size = &RoI->_size;
    RoI->motion = features_alloc_RoIs_motion(max_size, RoI->id);
    RoI->motion->_max_size = &RoI->_max_size;
    RoI->motion->_size = &RoI->_size;
    RoI->misc = features_alloc_RoIs_misc(enable_magnitude, enable_sat_count, enable_ellipse, max_size, RoI->id);
    RoI->misc->_max_size = &RoI->_max_size;
    RoI->misc->_size = &RoI->_size;
    RoI->_max_size = max_size;
    return RoI;
}

void features_init_RoIs(RoIs_t* RoI) {
    memset(RoI->id, 0, RoI->_max_size * sizeof(uint32_t));
    const uint8_t init_id = 0;
    features_init_RoIs_basic(RoI->basic, init_id);
    features_init_RoIs_asso(RoI->asso, init_id);
    features_init_RoIs_motion(RoI->motion, init_id);
    features_init_RoIs_misc(RoI->misc, init_id);
    RoI->_size = 0;
}

void features_free_RoIs(RoIs_t* RoI) {
    free(RoI->id);
    const uint8_t free_id = 0;
    features_free_RoIs_basic(RoI->basic, free_id);
    features_free_RoIs_asso(RoI->asso, free_id);
    features_free_RoIs_motion(RoI->motion, free_id);
    features_free_RoIs_misc(RoI->misc, free_id);
    free(RoI);
}

void _features_extract(const uint32_t** labels, const int i0, const int i1, const int j0, const int j1,
                       uint32_t* RoIs_id, uint32_t* RoIs_xmin, uint32_t* RoIs_xmax, uint32_t* RoIs_ymin,
                       uint32_t* RoIs_ymax, uint32_t* RoIs_S, uint32_t* RoIs_Sx, uint32_t* RoIs_Sy,
                       uint64_t* RoIs_Sx2, uint64_t* RoIs_Sy2, uint64_t* RoIs_Sxy, float* RoIs_x,
                       float* RoIs_y, const size_t n_RoIs) {
    for (size_t i = 0; i < n_RoIs; i++) {
        RoIs_xmin[i] = j1;
        RoIs_xmax[i] = j0;
        RoIs_ymin[i] = i1;
        RoIs_ymax[i] = i0;
    }

    memset(RoIs_S, 0, n_RoIs * sizeof(uint32_t));
    memset(RoIs_Sx, 0, n_RoIs * sizeof(uint32_t));
    memset(RoIs_Sy, 0, n_RoIs * sizeof(uint32_t));
    memset(RoIs_Sx2, 0, n_RoIs * sizeof(uint32_t));
    memset(RoIs_Sy2, 0, n_RoIs * sizeof(uint32_t));
    memset(RoIs_Sxy, 0, n_RoIs * sizeof(uint32_t));

    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            uint32_t e = (uint32_t)labels[i][j];
            if (e > 0) {
                assert(e < MAX_ROI_SIZE_BEFORE_SHRINK);
                uint32_t r = e - 1;
                RoIs_S[r] += 1;
                RoIs_id[r] = e;
                RoIs_Sx[r] += j;
                RoIs_Sy[r] += i;
                RoIs_Sx2[r] += j * j;
                RoIs_Sy2[r] += i * i;
                RoIs_Sxy[r] += j * i;
                if (j < (int)RoIs_xmin[r])
                    RoIs_xmin[r] = j;
                if (j > (int)RoIs_xmax[r])
                    RoIs_xmax[r] = j;
                if (i < (int)RoIs_ymin[r])
                    RoIs_ymin[r] = i;
                if (i > (int)RoIs_ymax[r])
                    RoIs_ymax[r] = i;
            }
        }
    }

    for (size_t i = 0; i < n_RoIs; i++) {
        RoIs_x[i] = (float)RoIs_Sx[i] / (float)RoIs_S[i];
        RoIs_y[i] = (float)RoIs_Sy[i] / (float)RoIs_S[i];
    }
}

void features_extract(const uint32_t** labels, const int i0, const int i1, const int j0, const int j1,
                      const size_t n_RoIs, RoIs_basic_t* RoIs_basic) {
    const uint8_t init_id = 1;
    features_init_RoIs_basic(RoIs_basic, init_id);
    *RoIs_basic->_size = n_RoIs;
     _features_extract(labels, i0, i1, j0, j1, RoIs_basic->id, RoIs_basic->xmin, RoIs_basic->xmax, RoIs_basic->ymin,
                      RoIs_basic->ymax, RoIs_basic->S, RoIs_basic->Sx, RoIs_basic->Sy, RoIs_basic->Sx2, RoIs_basic->Sy2,
                      RoIs_basic->Sxy, RoIs_basic->x, RoIs_basic->y, *RoIs_basic->_size);
}

void _features_merge_CCL_HI_v2(const uint32_t** in_labels, const uint8_t** img_HI, uint32_t** out_labels, const int i0,
                               const int i1, const int j0, const int j1, uint32_t* RoIs_id, const uint32_t* RoIs_xmin,
                               const uint32_t* RoIs_xmax, const uint32_t* RoIs_ymin, const uint32_t* RoIs_ymax,
                               const uint32_t* RoIs_S, const size_t n_RoIs, const uint32_t S_min,
                               const uint32_t S_max) {
    if (out_labels != NULL && (void*)in_labels != (void*)out_labels)
        for (int i = i0; i <= i1; i++)
            memset(out_labels[i], 0, (j1 - j0 + 1) * sizeof(uint32_t));

    uint32_t x0, x1, y0, y1, id;
    uint32_t cur_label = 1;
    for (size_t i = 0; i < n_RoIs; i++) {
        if (RoIs_id[i]) {
            id = RoIs_id[i];
            x0 = RoIs_ymin[i];
            x1 = RoIs_ymax[i];
            y0 = RoIs_xmin[i];
            y1 = RoIs_xmax[i];
            if (S_min > RoIs_S[i] || RoIs_S[i] > S_max) {
                RoIs_id[i] = 0;
                if (out_labels != NULL && ((void*)in_labels == (void*)out_labels)) {
                    for (uint32_t k = x0; k <= x1; k++) {
                        for (uint32_t l = y0; l <= y1; l++) {
                            if (in_labels[k][l] == id)
                                out_labels[k][l] = 0;
                        }
                    }
                }
                continue;
            }
            for (uint32_t k = x0; k <= x1; k++) {
                for (uint32_t l = y0; l <= y1; l++) {
                    if (img_HI[k][l] && in_labels[k][l] == id) {
                        if (out_labels != NULL) {
                            for (k = x0; k <= x1; k++) {
                                for (l = y0; l <= y1; l++) {
                                    if (in_labels[k][l] == id) {
                                        out_labels[k][l] = cur_label;
                                    }
                                }
                            }
                        }
                        cur_label++;
                        goto next;
                    }
                }
            }
            RoIs_id[i] = 0;
            if (out_labels != NULL && ((void*)in_labels == (void*)out_labels)) {
                for (uint32_t k = x0; k <= x1; k++) {
                    for (uint32_t l = y0; l <= y1; l++) {
                        if (in_labels[k][l] == id)
                            out_labels[k][l] = 0;
                    }
                }
            }
        next:;
        }
    }
}

void features_merge_CCL_HI_v2(const uint32_t** in_labels, const uint8_t** img_HI, uint32_t** out_labels, const int i0,
                              const int i1, const int j0, const int j1, RoIs_basic_t* RoIs_basic, const uint32_t S_min,
                              const uint32_t S_max) {
    _features_merge_CCL_HI_v2(in_labels, img_HI, out_labels, i0,i1, j0, j1, RoIs_basic->id, RoIs_basic->xmin,
                              RoIs_basic->xmax, RoIs_basic->ymin, RoIs_basic->ymax, RoIs_basic->S,
                              *RoIs_basic->_size, S_min, S_max);
}

size_t _features_shrink_basic(const uint32_t* RoIs_src_id, const uint32_t* RoIs_src_xmin,
                              const uint32_t* RoIs_src_xmax, const uint32_t* RoIs_src_ymin,
                              const uint32_t* RoIs_src_ymax, const uint32_t* RoIs_src_S, const uint32_t* RoIs_src_Sx,
                              const uint32_t* RoIs_src_Sy, const uint64_t* RoIs_src_Sx2, const uint64_t* RoIs_src_Sy2,
                              const uint64_t* RoIs_src_Sxy, const float* RoIs_src_x, const float* RoIs_src_y,
                              const size_t n_RoIs_src, uint32_t* RoIs_dst_id, uint32_t* RoIs_dst_xmin,
                              uint32_t* RoIs_dst_xmax, uint32_t* RoIs_dst_ymin, uint32_t* RoIs_dst_ymax,
                              uint32_t* RoIs_dst_S, uint32_t* RoIs_dst_Sx, uint32_t* RoIs_dst_Sy,
                              uint64_t* RoIs_dst_Sx2, uint64_t* RoIs_dst_Sy2, uint64_t* RoIs_dst_Sxy, float* RoIs_dst_x,
                              float* RoIs_dst_y) {
    size_t cpt = 0;
    for (size_t i = 0; i < n_RoIs_src; i++) {
        if (RoIs_src_id[i]) {
            assert(cpt < MAX_ROI_SIZE);
            RoIs_dst_id[cpt] = cpt + 1;
            RoIs_dst_xmin[cpt] = RoIs_src_xmin[i];
            RoIs_dst_xmax[cpt] = RoIs_src_xmax[i];
            RoIs_dst_ymin[cpt] = RoIs_src_ymin[i];
            RoIs_dst_ymax[cpt] = RoIs_src_ymax[i];
            RoIs_dst_S[cpt] = RoIs_src_S[i];
            RoIs_dst_Sx[cpt] = RoIs_src_Sx[i];
            RoIs_dst_Sy[cpt] = RoIs_src_Sy[i];
            RoIs_dst_Sx2[cpt] = RoIs_src_Sx2[i];
            RoIs_dst_Sy2[cpt] = RoIs_src_Sy2[i];
            RoIs_dst_Sxy[cpt] = RoIs_src_Sxy[i];
            RoIs_dst_x[cpt] = RoIs_src_x[i];
            RoIs_dst_y[cpt] = RoIs_src_y[i];
            cpt++;
        }
    }
    return cpt;
}

void features_shrink_basic(const RoIs_basic_t* RoIs_basic_src, RoIs_basic_t* RoIs_basic_dst) {
    *RoIs_basic_dst->_size = _features_shrink_basic(RoIs_basic_src->id, RoIs_basic_src->xmin,
                                                     RoIs_basic_src->xmax, RoIs_basic_src->ymin,
                                                     RoIs_basic_src->ymax, RoIs_basic_src->S,
                                                     RoIs_basic_src->Sx, RoIs_basic_src->Sy,
                                                     RoIs_basic_src->Sx2, RoIs_basic_src->Sy2,
                                                     RoIs_basic_src->Sxy, RoIs_basic_src->x,
                                                     RoIs_basic_src->y, *RoIs_basic_src->_size,
                                                     RoIs_basic_dst->id,
                                                     RoIs_basic_dst->xmin, RoIs_basic_dst->xmax,
                                                     RoIs_basic_dst->ymin, RoIs_basic_dst->ymax,
                                                     RoIs_basic_dst->S, RoIs_basic_dst->Sx,
                                                     RoIs_basic_dst->Sy, RoIs_basic_dst->Sx2,
                                                     RoIs_basic_dst->Sy2, RoIs_basic_dst->Sxy,
                                                     RoIs_basic_dst->x, RoIs_basic_dst->y);
}

size_t _features_shrink_basic_misc(const uint32_t* RoIs_src_id, const uint32_t* RoIs_src_xmin,
                                   const uint32_t* RoIs_src_xmax, const uint32_t* RoIs_src_ymin,
                                   const uint32_t* RoIs_src_ymax, const uint32_t* RoIs_src_S,
                                   const uint32_t* RoIs_src_Sx, const uint32_t* RoIs_src_Sy,
                                   const uint64_t* RoIs_src_Sx2, const uint64_t* RoIs_src_Sy2,
                                   const uint64_t* RoIs_src_Sxy, const float* RoIs_src_x, const float* RoIs_src_y,
                                   const uint32_t* RoIs_src_magnitude, const uint32_t* RoIs_src_sat_count,
                                   const float* RoIs_src_a, const float* RoIs_src_b, const size_t n_RoIs_src,
                                   uint32_t* RoIs_dst_id, uint32_t* RoIs_dst_xmin, uint32_t* RoIs_dst_xmax,
                                   uint32_t* RoIs_dst_ymin, uint32_t* RoIs_dst_ymax, uint32_t* RoIs_dst_S,
                                   uint32_t* RoIs_dst_Sx, uint32_t* RoIs_dst_Sy, uint64_t* RoIs_dst_Sx2,
                                   uint64_t* RoIs_dst_Sy2, uint64_t* RoIs_dst_Sxy, float* RoIs_dst_x,
                                   float* RoIs_dst_y, uint32_t* RoIs_dst_magnitude, uint32_t* RoIs_dst_sat_count,
                                   float* RoIs_dst_a, float* RoIs_dst_b) {

    size_t cpt = 0;
    for (size_t i = 0; i < n_RoIs_src; i++) {
        if (RoIs_src_id[i]) {
            assert(cpt < MAX_ROI_SIZE);
            RoIs_dst_id[cpt] = cpt + 1;
            RoIs_dst_xmin[cpt] = RoIs_src_xmin[i];
            RoIs_dst_xmax[cpt] = RoIs_src_xmax[i];
            RoIs_dst_ymin[cpt] = RoIs_src_ymin[i];
            RoIs_dst_ymax[cpt] = RoIs_src_ymax[i];
            RoIs_dst_S[cpt] = RoIs_src_S[i];
            RoIs_dst_Sx[cpt] = RoIs_src_Sx[i];
            RoIs_dst_Sy[cpt] = RoIs_src_Sy[i];
            RoIs_dst_Sx2[cpt] = RoIs_src_Sx2[i];
            RoIs_dst_Sy2[cpt] = RoIs_src_Sy2[i];
            RoIs_dst_Sxy[cpt] = RoIs_src_Sxy[i];
            RoIs_dst_x[cpt] = RoIs_src_x[i];
            RoIs_dst_y[cpt] = RoIs_src_y[i];
            if (RoIs_dst_magnitude && RoIs_src_magnitude)
                RoIs_dst_magnitude[cpt] = RoIs_src_magnitude[i];
            if (RoIs_dst_sat_count && RoIs_src_sat_count)
                RoIs_dst_sat_count[cpt] = RoIs_src_sat_count[i];
            if (RoIs_dst_a && RoIs_src_a)
                RoIs_dst_a[cpt] = RoIs_src_a[i];
            if (RoIs_dst_b && RoIs_src_b)
                RoIs_dst_b[cpt] = RoIs_src_b[i];
            cpt++;
        }
    }
    return cpt;
}

void features_shrink_basic_misc(const RoIs_basic_t* RoIs_basic_src, const RoIs_misc_t* RoIs_misc_src,
                                RoIs_basic_t* RoIs_basic_dst, RoIs_misc_t* RoIs_misc_dst) {
    *RoIs_basic_dst->_size = _features_shrink_basic_misc(RoIs_basic_src->id, RoIs_basic_src->xmin,
                                                         RoIs_basic_src->xmax, RoIs_basic_src->ymin,
                                                         RoIs_basic_src->ymax, RoIs_basic_src->S,
                                                         RoIs_basic_src->Sx, RoIs_basic_src->Sy,
                                                         RoIs_basic_src->Sx2, RoIs_basic_src->Sy2,
                                                         RoIs_basic_src->Sxy, RoIs_basic_src->x,
                                                         RoIs_basic_src->y, RoIs_misc_src->magnitude,
                                                         RoIs_misc_src->sat_count, RoIs_misc_src->a, RoIs_misc_src->b,
                                                         *RoIs_basic_src->_size, RoIs_basic_dst->id,
                                                         RoIs_basic_dst->xmin, RoIs_basic_dst->xmax,
                                                         RoIs_basic_dst->ymin, RoIs_basic_dst->ymax, RoIs_basic_dst->S,
                                                         RoIs_basic_dst->Sx, RoIs_basic_dst->Sy, RoIs_basic_dst->Sx2,
                                                         RoIs_basic_dst->Sy2, RoIs_basic_dst->Sxy, RoIs_basic_dst->x,
                                                         RoIs_basic_dst->y, RoIs_misc_dst->magnitude,
                                                         RoIs_misc_dst->sat_count, RoIs_misc_dst->a, RoIs_misc_dst->b);
}

void _features_compute_magnitude(const uint8_t** img, const uint32_t img_width, const uint32_t img_height,
                                 const uint32_t** labels, const uint32_t* RoIs_xmin, const uint32_t* RoIs_xmax,
                                 const uint32_t* RoIs_ymin, const uint32_t* RoIs_ymax, const uint32_t* RoIs_S,
                                 uint32_t* RoIs_magnitude, uint32_t* RoIs_sat_count, const size_t n_RoIs) {
    assert(RoIs_magnitude != NULL);
    // set all RoIs (Regions of Interest) magnitudes to 0 (same for the saturation counter)
    memset(RoIs_magnitude, 0, n_RoIs * sizeof(uint32_t));
    if (RoIs_sat_count != NULL)
        memset(RoIs_sat_count, 0, n_RoIs * sizeof(uint32_t));
    // for each RoI (= Region of Interest = object)
    for (uint32_t r = 0; r < n_RoIs; r++) {
        // width and height of the current RoI
        uint32_t w = (RoIs_xmax[r] - RoIs_xmin[r]) + 1;
        uint32_t h = (RoIs_ymax[r] - RoIs_ymin[r]) + 1;

        // bounding box around the RoI + extra space to consider local noise level
        // here this is important to cast 'RoIs_ymin' and 'RoIs_xmin' into signed integers because the subtraction with
        // 'h' or 'w' can result in a negative number
        uint32_t ymin = (int64_t)RoIs_ymin[r] - h >          0 ? RoIs_ymin[r] - h :          0;
        uint32_t ymax =          RoIs_ymax[r] + h < img_height ? RoIs_ymax[r] + h : img_height;
        uint32_t xmin = (int64_t)RoIs_xmin[r] - w >          0 ? RoIs_xmin[r] - w :          0;
        uint32_t xmax =          RoIs_xmax[r] + w <  img_width ? RoIs_xmax[r] + w :  img_width;

        uint32_t acc_noise = 0; // accumulate noisy pixels (= dark pixels)
        uint32_t count_noise = 0; // count the number of noisy pixels
        uint32_t count_px = 0; // count the number of pixels for the current RoI (= bright pixels)

        // moving in a square (bigger that the real bounding box) around the current RoI
        for (uint32_t i = ymin; i <= ymax; i++) {
            for (uint32_t j = xmin; j <= xmax; j++) {
                // get the label from the current pixel position
                // if l != 0 then it is a RoI, else it is a dark / noisy pixel
                uint32_t l = labels[i][j];
                // check if the current pixel belong to the current RoI (same bounding box can share multiple RoIs)
                if (l == r + 1) {
                    RoIs_magnitude[r] += (uint32_t)img[i][j];
                    count_px++;
                    // increment the saturation counter if the current pixel is saturated
                    if (RoIs_sat_count != NULL && img[i][j] == 255)
                        RoIs_sat_count[r]++;
                } else if (l == 0) {
                    acc_noise += (uint32_t)img[i][j];
                    count_noise++;
                }
            }
        }
        assert(count_px == RoIs_S[r]); // useless check, only for debugging purpose
        // compute mean noise value
        uint32_t noise = acc_noise / count_noise;
        // subtract mean noise to the current RoI (Region of Interest) magnitude
        RoIs_magnitude[r] -= noise * RoIs_S[r];
    }
}

void features_compute_magnitude(const uint8_t** img, const uint32_t img_width, const uint32_t img_height,
                                const uint32_t** labels, const RoIs_basic_t* RoIs_basic, RoIs_misc_t* RoIs_misc) {
    _features_compute_magnitude(img, img_width, img_height, labels, RoIs_basic->xmin, RoIs_basic->xmax,
                                RoIs_basic->ymin, RoIs_basic->ymax, RoIs_basic->S, RoIs_misc->magnitude,
                                RoIs_misc->sat_count, *RoIs_misc->_size);
}

void _features_compute_ellipse(const uint32_t *RoIs_S, const uint32_t *RoIs_Sx, const uint32_t *RoIs_Sy,
                               const uint64_t *RoIs_Sx2, const uint64_t *RoIs_Sy2, const uint64_t* RoIs_Sxy,
                               float *RoIs_a, float *RoIs_b, const size_t n_RoIs) {
    assert(RoIs_a != NULL);
    assert(RoIs_b != NULL);

    // set all RoIs (Regions of Interest) axes to 0
    memset(RoIs_a, 0, n_RoIs * sizeof(float));
    memset(RoIs_b, 0, n_RoIs * sizeof(float));

    for (size_t e = 0; e < n_RoIs; e++) {
        double S = RoIs_S[e];

        double Sx = RoIs_Sx[e];
        double Sy = RoIs_Sy[e];

        // moments non centres
        double Sxx = RoIs_Sx2[e];
        double Sxy = RoIs_Sxy[e];
        double Syy = RoIs_Sy2[e];

        // moments centres
        double m20 = S * Sxx - Sx * Sx;
        double m11 = S * Sxy - Sx * Sy;
        double m02 = S * Syy - Sy * Sy;

        // par construction a > b
        double a2 = (m20 + m02 + sqrtf((m20 - m02) * (m20 - m02) + 4 * m11 * m11)) / (2 * S);
        double b2 = (m20 + m02 - sqrtf((m20 - m02) * (m20 - m02) + 4 * m11 * m11)) / (2 * S);

        float a = sqrt(a2);
        float b = sqrt(b2);

        RoIs_a[e] = a;
        RoIs_b[e] = b;

        // float ratio = a / b; // metrique plus simple a visualiser que l'applatissement
        // fprintf(stderr, "# %2d   S = %f   m20 = %f a = %5.1f   b = %5.1f   ratio = %5.1f \n", e, S, m11,
        // RoIs_a[e], RoIs_b[e], ratio);

        //putchar('\n');
    }
}

void features_compute_ellipse(const RoIs_basic_t* RoIs_basic, RoIs_misc_t* RoIs_misc) {
    _features_compute_ellipse(RoIs_basic->S, RoIs_basic->Sx, RoIs_basic->Sy, RoIs_basic->Sx2, RoIs_basic->Sy2,
                              RoIs_basic->Sxy, RoIs_misc->a, RoIs_misc->b, *RoIs_misc->_size);
}
