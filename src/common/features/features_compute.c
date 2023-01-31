#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "fmdt/macros.h"
#include "vec.h"

#include "fmdt/features/features_compute.h"

RoI_basic_t* features_alloc_RoI_basic(const size_t max_size, uint32_t* RoI_id) {
    RoI_basic_t* RoI_basic = (RoI_basic_t*)malloc(sizeof(RoI_basic_t));
    RoI_basic->id = (RoI_id == NULL) ? (uint32_t*)malloc(max_size * sizeof(uint32_t)) : RoI_id;
    RoI_basic->xmin = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoI_basic->xmax = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoI_basic->ymin = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoI_basic->ymax = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoI_basic->S = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoI_basic->Sx = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoI_basic->Sy = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoI_basic->x = (float*)malloc(max_size * sizeof(float));
    RoI_basic->y = (float*)malloc(max_size * sizeof(float));
    if (RoI_id == NULL) {
        RoI_basic->_max_size = (size_t*)malloc(sizeof(size_t));
        RoI_basic->_size = (size_t*)malloc(sizeof(size_t));
        *RoI_basic->_max_size = max_size;
    }
    return RoI_basic;
}

void features_init_RoI_basic(RoI_basic_t* RoI_basic, const uint8_t init_id) {
    if (init_id)
        memset(RoI_basic->id, 0, *RoI_basic->_max_size * sizeof(uint32_t));
    memset(RoI_basic->xmin, 0, *RoI_basic->_max_size * sizeof(uint32_t));
    memset(RoI_basic->xmax, 0, *RoI_basic->_max_size * sizeof(uint32_t));
    memset(RoI_basic->ymin, 0, *RoI_basic->_max_size * sizeof(uint32_t));
    memset(RoI_basic->ymax, 0, *RoI_basic->_max_size * sizeof(uint32_t));
    memset(RoI_basic->S, 0, *RoI_basic->_max_size * sizeof(uint32_t));
    memset(RoI_basic->Sx, 0, *RoI_basic->_max_size * sizeof(uint32_t));
    memset(RoI_basic->Sy, 0, *RoI_basic->_max_size * sizeof(uint32_t));
    memset(RoI_basic->x, 0, *RoI_basic->_max_size * sizeof(float));
    memset(RoI_basic->y, 0, *RoI_basic->_max_size * sizeof(float));
    *RoI_basic->_size = 0;
}

void features_free_RoI_basic(RoI_basic_t* RoI_basic, const uint8_t free_id) {
    if (free_id) {
        free(RoI_basic->id);
        free(RoI_basic->_size);
        free(RoI_basic->_max_size);
    }
    free(RoI_basic->xmin);
    free(RoI_basic->xmax);
    free(RoI_basic->ymin);
    free(RoI_basic->ymax);
    free(RoI_basic->S);
    free(RoI_basic->Sx);
    free(RoI_basic->Sy);
    free(RoI_basic->x);
    free(RoI_basic->y);
    free(RoI_basic);
}

RoI_asso_t* features_alloc_RoI_asso(const size_t max_size, uint32_t* RoI_id) {
    RoI_asso_t* RoI_asso = (RoI_asso_t*)malloc(sizeof(RoI_asso_t));
    RoI_asso->id = (RoI_id == NULL) ? (uint32_t*)malloc(max_size * sizeof(uint32_t)) : RoI_id;
    RoI_asso->prev_id = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoI_asso->next_id = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    if (RoI_id == NULL) {
        RoI_asso->_max_size = (size_t*)malloc(sizeof(size_t));
        RoI_asso->_size = (size_t*)malloc(sizeof(size_t));
        *RoI_asso->_max_size = max_size;
    }
    return RoI_asso;
}

void features_init_RoI_asso(RoI_asso_t* RoI_asso, const uint8_t init_id) {
    if (init_id)
        memset(RoI_asso->id, 0, *RoI_asso->_max_size * sizeof(uint32_t));
    memset(RoI_asso->prev_id, 0, *RoI_asso->_max_size * sizeof(uint32_t));
    memset(RoI_asso->next_id, 0, *RoI_asso->_max_size * sizeof(uint32_t));
    *RoI_asso->_size = 0;
}

void features_free_RoI_asso(RoI_asso_t* RoI_asso, const uint8_t free_id) {
    if (free_id) {
        free(RoI_asso->id);
        free(RoI_asso->_size);
        free(RoI_asso->_max_size);
    }
    free(RoI_asso->prev_id);
    free(RoI_asso->next_id);
    free(RoI_asso);
}

RoI_motion_t* features_alloc_RoI_motion(const size_t max_size, uint32_t* RoI_id) {
    RoI_motion_t* RoI_motion = (RoI_motion_t*)malloc(sizeof(RoI_motion_t));
    RoI_motion->id = (RoI_id == NULL) ? (uint32_t*)malloc(max_size * sizeof(uint32_t)) : RoI_id;
    RoI_motion->dx = (float*)malloc(max_size * sizeof(float));
    RoI_motion->dy = (float*)malloc(max_size * sizeof(float));
    RoI_motion->error = (float*)malloc(max_size * sizeof(float));
    RoI_motion->is_moving = (uint8_t*)malloc(max_size * sizeof(uint8_t));
    if (RoI_id == NULL) {
        RoI_motion->_max_size = (size_t*)malloc(sizeof(size_t));
        RoI_motion->_size = (size_t*)malloc(sizeof(size_t));
        *RoI_motion->_max_size = max_size;
    }
    return RoI_motion;
}

void features_init_RoI_motion(RoI_motion_t* RoI_motion, const uint8_t init_id) {
    if (init_id)
        memset(RoI_motion->id, 0, *RoI_motion->_max_size * sizeof(uint32_t));
    memset(RoI_motion->dx, 0, *RoI_motion->_max_size * sizeof(float));
    memset(RoI_motion->dy, 0, *RoI_motion->_max_size * sizeof(float));
    memset(RoI_motion->error, 0, *RoI_motion->_max_size * sizeof(float));
    memset(RoI_motion->is_moving, 0, *RoI_motion->_max_size * sizeof(uint8_t));
    *RoI_motion->_size = 0;
}

void features_free_RoI_motion(RoI_motion_t* RoI_motion, const uint8_t free_id) {
    if (free_id) {
        free(RoI_motion->id);
        free(RoI_motion->_size);
        free(RoI_motion->_max_size);
    }
    free(RoI_motion->dx);
    free(RoI_motion->dy);
    free(RoI_motion->error);
    free(RoI_motion->is_moving);
    free(RoI_motion);
}

RoI_misc_t* features_alloc_RoI_misc(const size_t max_size, uint32_t* RoI_id) {
    RoI_misc_t* RoI_misc = (RoI_misc_t*)malloc(sizeof(RoI_misc_t));
    RoI_misc->id = (RoI_id == NULL) ? (uint32_t*)malloc(max_size * sizeof(uint32_t)) : RoI_id;
    RoI_misc->magnitude = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    if (RoI_id == NULL) {
        RoI_misc->_max_size = (size_t*)malloc(sizeof(size_t));
        RoI_misc->_size = (size_t*)malloc(sizeof(size_t));
        *RoI_misc->_max_size = max_size;
    }
    return RoI_misc;
}

void features_init_RoI_misc(RoI_misc_t* RoI_misc, const uint8_t init_id) {
    if (init_id)
        memset(RoI_misc->id, 0, *RoI_misc->_max_size * sizeof(uint32_t));
    memset(RoI_misc->magnitude, 0, *RoI_misc->_max_size * sizeof(uint32_t));
    *RoI_misc->_size = 0;
}

void features_free_RoI_misc(RoI_misc_t* RoI_misc, const uint8_t free_id) {
    if (free_id) {
        free(RoI_misc->id);
        free(RoI_misc->_size);
        free(RoI_misc->_max_size);
    }
    free(RoI_misc->magnitude);
    free(RoI_misc);
}

RoI_t* features_alloc_RoI(const size_t max_size) {
    RoI_t* RoI = (RoI_t*)malloc(sizeof(RoI_t));
    RoI->id = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoI->basic = features_alloc_RoI_basic(max_size, RoI->id);
    RoI->basic->_max_size = &RoI->_max_size;
    RoI->basic->_size = &RoI->_size;
    RoI->asso = features_alloc_RoI_asso(max_size, RoI->id);
    RoI->asso->_max_size = &RoI->_max_size;
    RoI->asso->_size = &RoI->_size;
    RoI->motion = features_alloc_RoI_motion(max_size, RoI->id);
    RoI->motion->_max_size = &RoI->_max_size;
    RoI->motion->_size = &RoI->_size;
    RoI->misc = features_alloc_RoI_misc(max_size, RoI->id);
    RoI->misc->_max_size = &RoI->_max_size;
    RoI->misc->_size = &RoI->_size;
    RoI->_max_size = max_size;
    return RoI;
}

void features_init_RoI(RoI_t* RoI) {
    memset(RoI->id, 0, RoI->_max_size * sizeof(uint32_t));
    const uint8_t init_id = 0;
    features_init_RoI_basic(RoI->basic, init_id);
    features_init_RoI_asso(RoI->asso, init_id);
    features_init_RoI_motion(RoI->motion, init_id);
    features_init_RoI_misc(RoI->misc, init_id);
    RoI->_size = 0;
}

void features_free_RoI(RoI_t* RoI) {
    free(RoI->id);
    const uint8_t free_id = 0;
    features_free_RoI_basic(RoI->basic, free_id);
    features_free_RoI_asso(RoI->asso, free_id);
    features_free_RoI_motion(RoI->motion, free_id);
    features_free_RoI_misc(RoI->misc, free_id);
    free(RoI);
}

void _features_extract(const uint32_t** labels, const int i0, const int i1, const int j0, const int j1,
                       uint32_t* RoI_id, uint32_t* RoI_xmin, uint32_t* RoI_xmax, uint32_t* RoI_ymin, uint32_t* RoI_ymax,
                       uint32_t* RoI_S, uint32_t* RoI_Sx, uint32_t* RoI_Sy, float* RoI_x, float* RoI_y,
                       const size_t n_RoI) {
    for (size_t i = 0; i < n_RoI; i++) {
        RoI_xmin[i] = j1;
        RoI_xmax[i] = j0;
        RoI_ymin[i] = i1;
        RoI_ymax[i] = i0;
    }

    memset(RoI_S, 0, n_RoI * sizeof(uint32_t));
    memset(RoI_Sx, 0, n_RoI * sizeof(uint32_t));
    memset(RoI_Sy, 0, n_RoI * sizeof(uint32_t));

    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            uint32_t e = (uint32_t)labels[i][j];
            if (e > 0) {
                assert(e < MAX_ROI_SIZE_BEFORE_SHRINK);
                uint32_t r = e - 1;
                RoI_S[r] += 1;
                RoI_id[r] = e;
                RoI_Sx[r] += j;
                RoI_Sy[r] += i;
                if (j < (int)RoI_xmin[r])
                    RoI_xmin[r] = j;
                if (j > (int)RoI_xmax[r])
                    RoI_xmax[r] = j;
                if (i < (int)RoI_ymin[r])
                    RoI_ymin[r] = i;
                if (i > (int)RoI_ymax[r])
                    RoI_ymax[r] = i;
            }
        }
    }

    for (size_t i = 0; i < n_RoI; i++) {
        RoI_x[i] = (float)RoI_Sx[i] / (float)RoI_S[i];
        RoI_y[i] = (float)RoI_Sy[i] / (float)RoI_S[i];
    }
}

void features_extract(const uint32_t** labels, const int i0, const int i1, const int j0, const int j1, const size_t n_RoI,
                      RoI_basic_t* RoI_basic) {
    const uint8_t init_id = 1;
    features_init_RoI_basic(RoI_basic, init_id);
    *RoI_basic->_size = n_RoI;
    _features_extract(labels, i0, i1, j0, j1, RoI_basic->id, RoI_basic->xmin, RoI_basic->xmax,
                      RoI_basic->ymin, RoI_basic->ymax, RoI_basic->S, RoI_basic->Sx,
                      RoI_basic->Sy, RoI_basic->x, RoI_basic->y, *RoI_basic->_size);
}

void _features_merge_CCL_HI_v2(const uint32_t** in_labels, const uint8_t** img_HI, uint32_t** out_labels, const int i0,
                               const int i1, const int j0, const int j1, uint32_t* RoI_id, const uint32_t* RoI_xmin,
                               const uint32_t* RoI_xmax, const uint32_t* RoI_ymin, const uint32_t* RoI_ymax,
                               const uint32_t* RoI_S, const size_t n_RoI, const uint32_t S_min, const uint32_t S_max) {
    if ((void*)img_HI != (void*)out_labels)
        for (int i = i0; i <= i1; i++)
            for (int j = j0; j <= j1; j++)
                out_labels[i][j] = (uint32_t)img_HI[i][j];

    uint32_t x0, x1, y0, y1, id;
    uint32_t cur_label = 1;
    for (size_t i = 0; i < n_RoI; i++) {
        if (RoI_id[i]) {
            id = RoI_id[i];
            x0 = RoI_ymin[i];
            x1 = RoI_ymax[i];
            y0 = RoI_xmin[i];
            y1 = RoI_xmax[i];
            if (S_min > RoI_S[i] || RoI_S[i] > S_max) {
                RoI_id[i] = 0;
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
            RoI_id[i] = 0;
        next:;
        }
    }
}

void features_merge_CCL_HI_v2(const uint32_t** in_labels, const uint8_t** img_HI, uint32_t** out_labels, const int i0,
                              const int i1, const int j0, const int j1, RoI_basic_t* RoI_basic,
                              const uint32_t S_min, const uint32_t S_max) {
    _features_merge_CCL_HI_v2(in_labels, img_HI, out_labels, i0,i1, j0, j1, RoI_basic->id, RoI_basic->xmin,
                              RoI_basic->xmax, RoI_basic->ymin, RoI_basic->ymax, RoI_basic->S,
                              *RoI_basic->_size, S_min, S_max);
}

size_t _features_shrink(const uint32_t* RoI_src_id, const uint32_t* RoI_src_xmin,
                        const uint32_t* RoI_src_xmax, const uint32_t* RoI_src_ymin,
                        const uint32_t* RoI_src_ymax, const uint32_t* RoI_src_S, const uint32_t* RoI_src_Sx,
                        const uint32_t* RoI_src_Sy, const float* RoI_src_x, const float* RoI_src_y,
                        const size_t n_RoI_src, uint32_t* RoI_dest_id, uint32_t* RoI_dest_xmin,
                        uint32_t* RoI_dest_xmax, uint32_t* RoI_dest_ymin, uint32_t* RoI_dest_ymax,
                        uint32_t* RoI_dest_S, uint32_t* RoI_dest_Sx, uint32_t* RoI_dest_Sy, float* RoI_dest_x,
                        float* RoI_dest_y) {
    size_t cpt = 0;
    for (size_t i = 0; i < n_RoI_src; i++) {
        if (RoI_src_id[i]) {
            assert(cpt < MAX_ROI_SIZE);
            RoI_dest_id[cpt] = cpt + 1;
            RoI_dest_xmin[cpt] = RoI_src_xmin[i];
            RoI_dest_xmax[cpt] = RoI_src_xmax[i];
            RoI_dest_ymin[cpt] = RoI_src_ymin[i];
            RoI_dest_ymax[cpt] = RoI_src_ymax[i];
            RoI_dest_S[cpt] = RoI_src_S[i];
            RoI_dest_Sx[cpt] = RoI_src_Sx[i];
            RoI_dest_Sy[cpt] = RoI_src_Sy[i];
            RoI_dest_x[cpt] = RoI_src_x[i];
            RoI_dest_y[cpt] = RoI_src_y[i];
            cpt++;
        }
    }
    return cpt;
}

void features_shrink(const RoI_basic_t* RoI_basic_src, RoI_basic_t* RoI_basic_dest) {
    *RoI_basic_dest->_size = _features_shrink(RoI_basic_src->id, RoI_basic_src->xmin,
                                              RoI_basic_src->xmax, RoI_basic_src->ymin,
                                              RoI_basic_src->ymax, RoI_basic_src->S,
                                              RoI_basic_src->Sx, RoI_basic_src->Sy,
                                              RoI_basic_src->x, RoI_basic_src->y,
                                              *RoI_basic_src->_size, RoI_basic_dest->id,
                                              RoI_basic_dest->xmin, RoI_basic_dest->xmax,
                                              RoI_basic_dest->ymin, RoI_basic_dest->ymax,
                                              RoI_basic_dest->S, RoI_basic_dest->Sx,
                                              RoI_basic_dest->Sy, RoI_basic_dest->x,
                                              RoI_basic_dest->y);
}

void _features_compute_magnitude(const uint8_t** img, const uint32_t img_width, const uint32_t img_height,
                                 const uint32_t** labels, const uint32_t* RoI_xmin, const uint32_t* RoI_xmax,
                                 const uint32_t* RoI_ymin, const uint32_t* RoI_ymax, const uint32_t* RoI_S,
                                 uint32_t* RoI_magnitude, const size_t n_RoI) {
    // set all RoIs (Regions of Interest) magnitudes to 0
    memset(RoI_magnitude, 0, n_RoI * sizeof(uint32_t));
    // for each RoI (= Region of Interest = object)
    for (uint32_t r = 0; r < n_RoI; r++) {
        // width and height of the current RoI
        uint32_t w = (RoI_xmax[r] - RoI_xmin[r]) + 1;
        uint32_t h = (RoI_ymax[r] - RoI_ymin[r]) + 1;

        // bounding box around the RoI + extra space to consider local noise level
        // here this is important to cast 'RoI_ymin' and 'RoI_xmin' into signed integers because the subtraction with
        // 'h' or 'w' can result in a negative number
        uint32_t ymin = (int64_t)RoI_ymin[r] - h >          0 ? RoI_ymin[r] - h :          0;
        uint32_t ymax =          RoI_ymax[r] + h < img_height ? RoI_ymax[r] + h : img_height;
        uint32_t xmin = (int64_t)RoI_xmin[r] - w >          0 ? RoI_xmin[r] - w :          0;
        uint32_t xmax =          RoI_xmax[r] + w <  img_width ? RoI_xmax[r] + w :  img_width;

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
                    RoI_magnitude[r] += (uint32_t)img[i][j];
                    count_px++;
                } else if (l == 0) {
                    acc_noise += (uint32_t)img[i][j];
                    count_noise++;
                }
            }
        }
        assert(count_px == RoI_S[r]); // useless check, only for debugging purpose
        // compute mean noise value
        uint32_t noise = acc_noise / count_noise;
        // subtract mean noise to the current RoI (Region of Interest) magnitude
        RoI_magnitude[r] -= noise * RoI_S[r];
    }
}

void features_compute_magnitude(const uint8_t** img, const uint32_t img_width, const uint32_t img_height,
                                const uint32_t** labels, const RoI_basic_t* RoI_basic,
                                RoI_misc_t* RoI_misc) {
    _features_compute_magnitude(img, img_width, img_height, labels, RoI_basic->xmin, RoI_basic->xmax,
                                RoI_basic->ymin, RoI_basic->ymax, RoI_basic->S,
                                RoI_misc->magnitude, *RoI_misc->_size);
}
