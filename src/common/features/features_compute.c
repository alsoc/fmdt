#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "fmdt/macros.h"
#include "vec.h"

#include "fmdt/features/features_compute.h"

RoI_basic_t* features_alloc_RoI_basic(const size_t max_size, uint32_t* RoI_id) {
    RoI_basic_t* RoI_basic_array = (RoI_basic_t*)malloc(sizeof(RoI_basic_t));
    RoI_basic_array->id = (RoI_id == NULL) ? (uint32_t*)malloc(max_size * sizeof(uint32_t)) : RoI_id;
    RoI_basic_array->xmin = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoI_basic_array->xmax = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoI_basic_array->ymin = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoI_basic_array->ymax = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoI_basic_array->S = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoI_basic_array->Sx = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoI_basic_array->Sy = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoI_basic_array->x = (float*)malloc(max_size * sizeof(float));
    RoI_basic_array->y = (float*)malloc(max_size * sizeof(float));
    if (RoI_id == NULL) {
        RoI_basic_array->_max_size = (size_t*)malloc(sizeof(size_t));
        RoI_basic_array->_size = (size_t*)malloc(sizeof(size_t));
        *RoI_basic_array->_max_size = max_size;
    }
    return RoI_basic_array;
}

void features_init_RoI_basic(RoI_basic_t* RoI_basic_array, const uint8_t init_id) {
    if (init_id)
        memset(RoI_basic_array->id, 0, *RoI_basic_array->_max_size * sizeof(uint32_t));
    memset(RoI_basic_array->xmin, 0, *RoI_basic_array->_max_size * sizeof(uint32_t));
    memset(RoI_basic_array->xmax, 0, *RoI_basic_array->_max_size * sizeof(uint32_t));
    memset(RoI_basic_array->ymin, 0, *RoI_basic_array->_max_size * sizeof(uint32_t));
    memset(RoI_basic_array->ymax, 0, *RoI_basic_array->_max_size * sizeof(uint32_t));
    memset(RoI_basic_array->S, 0, *RoI_basic_array->_max_size * sizeof(uint32_t));
    memset(RoI_basic_array->Sx, 0, *RoI_basic_array->_max_size * sizeof(uint32_t));
    memset(RoI_basic_array->Sy, 0, *RoI_basic_array->_max_size * sizeof(uint32_t));
    memset(RoI_basic_array->x, 0, *RoI_basic_array->_max_size * sizeof(float));
    memset(RoI_basic_array->y, 0, *RoI_basic_array->_max_size * sizeof(float));
    *RoI_basic_array->_size = 0;
}

void features_free_RoI_basic(RoI_basic_t* RoI_basic_array, const uint8_t free_id) {
    if (free_id) {
        free(RoI_basic_array->id);
        free(RoI_basic_array->_size);
        free(RoI_basic_array->_max_size);
    }
    free(RoI_basic_array->xmin);
    free(RoI_basic_array->xmax);
    free(RoI_basic_array->ymin);
    free(RoI_basic_array->ymax);
    free(RoI_basic_array->S);
    free(RoI_basic_array->Sx);
    free(RoI_basic_array->Sy);
    free(RoI_basic_array->x);
    free(RoI_basic_array->y);
    free(RoI_basic_array);
}

RoI_asso_t* features_alloc_RoI_asso(const size_t max_size, uint32_t* RoI_id) {
    RoI_asso_t* RoI_asso_array = (RoI_asso_t*)malloc(sizeof(RoI_asso_t));
    RoI_asso_array->id = (RoI_id == NULL) ? (uint32_t*)malloc(max_size * sizeof(uint32_t)) : RoI_id;
    RoI_asso_array->prev_id = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoI_asso_array->next_id = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    if (RoI_id == NULL) {
        RoI_asso_array->_max_size = (size_t*)malloc(sizeof(size_t));
        RoI_asso_array->_size = (size_t*)malloc(sizeof(size_t));
        *RoI_asso_array->_max_size = max_size;
    }
    return RoI_asso_array;
}

void features_init_RoI_asso(RoI_asso_t* RoI_asso_array, const uint8_t init_id) {
    if (init_id)
        memset(RoI_asso_array->id, 0, *RoI_asso_array->_max_size * sizeof(uint32_t));
    memset(RoI_asso_array->prev_id, 0, *RoI_asso_array->_max_size * sizeof(uint32_t));
    memset(RoI_asso_array->next_id, 0, *RoI_asso_array->_max_size * sizeof(uint32_t));
    *RoI_asso_array->_size = 0;
}

void features_free_RoI_asso(RoI_asso_t* RoI_asso_array, const uint8_t free_id) {
    if (free_id) {
        free(RoI_asso_array->id);
        free(RoI_asso_array->_size);
        free(RoI_asso_array->_max_size);
    }
    free(RoI_asso_array->prev_id);
    free(RoI_asso_array->next_id);
    free(RoI_asso_array);
}

RoI_motion_t* features_alloc_RoI_motion(const size_t max_size, uint32_t* RoI_id) {
    RoI_motion_t* RoI_motion_array = (RoI_motion_t*)malloc(sizeof(RoI_motion_t));
    RoI_motion_array->id = (RoI_id == NULL) ? (uint32_t*)malloc(max_size * sizeof(uint32_t)) : RoI_id;
    RoI_motion_array->dx = (float*)malloc(max_size * sizeof(float));
    RoI_motion_array->dy = (float*)malloc(max_size * sizeof(float));
    RoI_motion_array->error = (float*)malloc(max_size * sizeof(float));
    RoI_motion_array->is_moving = (uint8_t*)malloc(max_size * sizeof(uint8_t));
    if (RoI_id == NULL) {
        RoI_motion_array->_max_size = (size_t*)malloc(sizeof(size_t));
        RoI_motion_array->_size = (size_t*)malloc(sizeof(size_t));
        *RoI_motion_array->_max_size = max_size;
    }
    return RoI_motion_array;
}

void features_init_RoI_motion(RoI_motion_t* RoI_motion_array, const uint8_t init_id) {
    if (init_id)
        memset(RoI_motion_array->id, 0, *RoI_motion_array->_max_size * sizeof(uint32_t));
    memset(RoI_motion_array->dx, 0, *RoI_motion_array->_max_size * sizeof(float));
    memset(RoI_motion_array->dy, 0, *RoI_motion_array->_max_size * sizeof(float));
    memset(RoI_motion_array->error, 0, *RoI_motion_array->_max_size * sizeof(float));
    memset(RoI_motion_array->is_moving, 0, *RoI_motion_array->_max_size * sizeof(uint8_t));
    *RoI_motion_array->_size = 0;
}

void features_free_RoI_motion(RoI_motion_t* RoI_motion_array, const uint8_t free_id) {
    if (free_id) {
        free(RoI_motion_array->id);
        free(RoI_motion_array->_size);
        free(RoI_motion_array->_max_size);
    }
    free(RoI_motion_array->dx);
    free(RoI_motion_array->dy);
    free(RoI_motion_array->error);
    free(RoI_motion_array->is_moving);
    free(RoI_motion_array);
}

RoI_misc_t* features_alloc_RoI_misc(const size_t max_size, uint32_t* RoI_id) {
    RoI_misc_t* RoI_misc_array = (RoI_misc_t*)malloc(sizeof(RoI_misc_t));
    RoI_misc_array->id = (RoI_id == NULL) ? (uint32_t*)malloc(max_size * sizeof(uint32_t)) : RoI_id;
    RoI_misc_array->magnitude = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    if (RoI_id == NULL) {
        RoI_misc_array->_max_size = (size_t*)malloc(sizeof(size_t));
        RoI_misc_array->_size = (size_t*)malloc(sizeof(size_t));
        *RoI_misc_array->_max_size = max_size;
    }
    return RoI_misc_array;
}

void features_init_RoI_misc(RoI_misc_t* RoI_misc_array, const uint8_t init_id) {
    if (init_id)
        memset(RoI_misc_array->id, 0, *RoI_misc_array->_max_size * sizeof(uint32_t));
    memset(RoI_misc_array->magnitude, 0, *RoI_misc_array->_max_size * sizeof(uint32_t));
    *RoI_misc_array->_size = 0;
}

void features_free_RoI_misc(RoI_misc_t* RoI_misc_array, const uint8_t free_id) {
    if (free_id) {
        free(RoI_misc_array->id);
        free(RoI_misc_array->_size);
        free(RoI_misc_array->_max_size);
    }
    free(RoI_misc_array->magnitude);
    free(RoI_misc_array);
}

RoI_t* features_alloc_RoI(const size_t max_size) {
    RoI_t* RoI_array = (RoI_t*)malloc(sizeof(RoI_t));
    RoI_array->id = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    RoI_array->basic = features_alloc_RoI_basic(max_size, RoI_array->id);
    RoI_array->basic->_max_size = &RoI_array->_max_size;
    RoI_array->basic->_size = &RoI_array->_size;
    RoI_array->asso = features_alloc_RoI_asso(max_size, RoI_array->id);
    RoI_array->asso->_max_size = &RoI_array->_max_size;
    RoI_array->asso->_size = &RoI_array->_size;
    RoI_array->motion = features_alloc_RoI_motion(max_size, RoI_array->id);
    RoI_array->motion->_max_size = &RoI_array->_max_size;
    RoI_array->motion->_size = &RoI_array->_size;
    RoI_array->misc = features_alloc_RoI_misc(max_size, RoI_array->id);
    RoI_array->misc->_max_size = &RoI_array->_max_size;
    RoI_array->misc->_size = &RoI_array->_size;
    RoI_array->_max_size = max_size;
    return RoI_array;
}

void features_init_RoI(RoI_t* RoI_array) {
    memset(RoI_array->id, 0, RoI_array->_max_size * sizeof(uint32_t));
    const uint8_t init_id = 0;
    features_init_RoI_basic(RoI_array->basic, init_id);
    features_init_RoI_asso(RoI_array->asso, init_id);
    features_init_RoI_motion(RoI_array->motion, init_id);
    features_init_RoI_misc(RoI_array->misc, init_id);
    RoI_array->_size = 0;
}

void features_free_RoI(RoI_t* RoI_array) {
    free(RoI_array->id);
    const uint8_t free_id = 0;
    features_free_RoI_basic(RoI_array->basic, free_id);
    features_free_RoI_asso(RoI_array->asso, free_id);
    features_free_RoI_motion(RoI_array->motion, free_id);
    features_free_RoI_misc(RoI_array->misc, free_id);
    free(RoI_array);
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
                assert(e < MAX_RoI_SIZE_BEFORE_SHRINK);
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
                      RoI_basic_t* RoI_basic_array) {
    const uint8_t init_id = 1;
    features_init_RoI_basic(RoI_basic_array, init_id);
    *RoI_basic_array->_size = n_RoI;
    _features_extract(labels, i0, i1, j0, j1, RoI_basic_array->id, RoI_basic_array->xmin, RoI_basic_array->xmax,
                      RoI_basic_array->ymin, RoI_basic_array->ymax, RoI_basic_array->S, RoI_basic_array->Sx,
                      RoI_basic_array->Sy, RoI_basic_array->x, RoI_basic_array->y, *RoI_basic_array->_size);
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
                              const int i1, const int j0, const int j1, RoI_basic_t* RoI_basic_array,
                              const uint32_t S_min, const uint32_t S_max) {
    _features_merge_CCL_HI_v2(in_labels, img_HI, out_labels, i0,i1, j0, j1, RoI_basic_array->id, RoI_basic_array->xmin,
                              RoI_basic_array->xmax, RoI_basic_array->ymin, RoI_basic_array->ymax, RoI_basic_array->S,
                              *RoI_basic_array->_size, S_min, S_max);
}

size_t _features_shrink_RoI_array(const uint32_t* RoI_src_id, const uint32_t* RoI_src_xmin,
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
            assert(cpt < MAX_RoI_SIZE);
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

void features_shrink_RoI_array(const RoI_basic_t* RoI_basic_array_src, RoI_basic_t* RoI_basic_array_dest) {
    *RoI_basic_array_dest->_size = _features_shrink_RoI_array(RoI_basic_array_src->id, RoI_basic_array_src->xmin,
                                                              RoI_basic_array_src->xmax, RoI_basic_array_src->ymin,
                                                              RoI_basic_array_src->ymax, RoI_basic_array_src->S,
                                                              RoI_basic_array_src->Sx, RoI_basic_array_src->Sy,
                                                              RoI_basic_array_src->x, RoI_basic_array_src->y,
                                                              *RoI_basic_array_src->_size, RoI_basic_array_dest->id,
                                                              RoI_basic_array_dest->xmin, RoI_basic_array_dest->xmax,
                                                              RoI_basic_array_dest->ymin, RoI_basic_array_dest->ymax,
                                                              RoI_basic_array_dest->S, RoI_basic_array_dest->Sx,
                                                              RoI_basic_array_dest->Sy, RoI_basic_array_dest->x,
                                                              RoI_basic_array_dest->y);
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
                                const uint32_t** labels, const RoI_basic_t* RoI_basic_array,
                                RoI_misc_t* RoI_misc_array) {
    _features_compute_magnitude(img, img_width, img_height, labels, RoI_basic_array->xmin, RoI_basic_array->xmax,
                                RoI_basic_array->ymin, RoI_basic_array->ymax, RoI_basic_array->S,
                                RoI_misc_array->magnitude, *RoI_misc_array->_size);
}
