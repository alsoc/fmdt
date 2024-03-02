#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "fmdt/macros.h"
#include "vec.h"

#include "fmdt/features/features_compute.h"

RoI_basic_t* features_alloc_RoIs_basic(const size_t max_size) {
    RoI_basic_t* RoIs_basic = (RoI_basic_t*)malloc(max_size * sizeof(RoI_basic_t));
    return RoIs_basic;
}

void features_init_RoIs_basic(RoI_basic_t* RoIs_basic, const size_t max_size) {
    memset(RoIs_basic, 0, max_size * sizeof(RoI_basic_t));
}

void features_free_RoIs_basic(RoI_basic_t* RoIs_basic) {
    free(RoIs_basic);
}

RoI_asso_t* features_alloc_RoIs_asso(const size_t max_size) {
    RoI_asso_t* RoIs_asso = (RoI_asso_t*)malloc(max_size * sizeof(RoI_asso_t));
    return RoIs_asso;
}

void features_init_RoIs_asso(RoI_asso_t* RoIs_asso, const size_t max_size) {
    memset(RoIs_asso, 0, max_size * sizeof(RoI_asso_t));
}

void features_free_RoIs_asso(RoI_asso_t* RoIs_asso) {
    free(RoIs_asso);
}

RoI_motion_t* features_alloc_RoIs_motion(const size_t max_size) {
    RoI_motion_t* RoIs_motion = (RoI_motion_t*)malloc(max_size * sizeof(RoI_motion_t));
    return RoIs_motion;
}

void features_init_RoIs_motion(RoI_motion_t* RoIs_motion, const size_t max_size) {
    memset(RoIs_motion, 0, max_size * sizeof(RoI_motion_t));
}

void features_free_RoIs_motion(RoI_motion_t* RoIs_motion) {
    free(RoIs_motion);
}

RoI_magn_t* features_alloc_RoIs_magn(const size_t max_size) {
    RoI_magn_t* RoIs_magn = (RoI_magn_t*)malloc(max_size * sizeof(RoI_magn_t));
    return RoIs_magn;
}

void features_init_RoIs_magn(RoI_magn_t* RoIs_magn, const size_t max_size) {
    memset(RoIs_magn, 0, max_size * sizeof(RoI_magn_t));
}

void features_free_RoIs_magn(RoI_magn_t* RoIs_magn) {
    free(RoIs_magn);
}

RoI_elli_t* features_alloc_RoIs_elli(const size_t max_size) {
    RoI_elli_t* RoIs_elli = (RoI_elli_t*)malloc(max_size * sizeof(RoI_elli_t));
    return RoIs_elli;
}

void features_init_RoIs_elli(RoI_elli_t* RoIs_elli, const size_t max_size) {
    memset(RoIs_elli, 0, max_size * sizeof(RoI_elli_t));
}

void features_free_RoIs_elli(RoI_elli_t* RoIs_elli) {
    free(RoIs_elli);
}

RoIs_t* features_alloc_RoIs(const size_t max_size, const uint8_t alloc_asso, const uint8_t alloc_motion,
                            const uint8_t alloc_magn, const uint8_t alloc_elli) {
    RoIs_t* RoI = (RoIs_t*)malloc(sizeof(RoIs_t));
    RoI->basic = features_alloc_RoIs_basic(max_size);
    if (alloc_asso)
        RoI->asso = features_alloc_RoIs_asso(max_size);
    else
        RoI->asso = NULL;
    if (alloc_motion)
        RoI->motion = features_alloc_RoIs_motion(max_size);
    else
        RoI->motion = NULL;
    if (alloc_magn)
        RoI->magn = features_alloc_RoIs_magn(max_size);
    else
        RoI->magn = NULL;
    if (alloc_elli)
        RoI->elli = features_alloc_RoIs_elli(max_size);
    else
        RoI->elli = NULL;
    RoI->_max_size = max_size;
    return RoI;
}

void features_init_RoIs(RoIs_t* RoI) {
    features_init_RoIs_basic(RoI->basic, RoI->_max_size);
    if (RoI->asso)
        features_init_RoIs_asso(RoI->asso, RoI->_max_size);
    if (RoI->motion)
        features_init_RoIs_motion(RoI->motion, RoI->_max_size);
    if (RoI->magn)
        features_init_RoIs_magn(RoI->magn, RoI->_max_size);
    if (RoI->elli)
        features_init_RoIs_elli(RoI->elli, RoI->_max_size);
    RoI->_size = 0;
}

void features_free_RoIs(RoIs_t* RoI) {
    features_free_RoIs_basic(RoI->basic);
    if (RoI->asso)
        features_free_RoIs_asso(RoI->asso);
    if (RoI->motion)
        features_free_RoIs_motion(RoI->motion);
    if (RoI->magn)
        features_free_RoIs_magn(RoI->magn);
    if (RoI->elli)
        features_free_RoIs_elli(RoI->elli);
    free(RoI);
}

void features_extract(const uint32_t** labels, const int i0, const int i1, const int j0, const int j1,
                      RoI_basic_t* RoIs_basic, const size_t n_RoIs) {
    features_init_RoIs_basic(RoIs_basic, n_RoIs);

    for (size_t i = 0; i < n_RoIs; i++) {
        RoIs_basic[i].xmin = j1;
        RoIs_basic[i].xmax = j0;
        RoIs_basic[i].ymin = i1;
        RoIs_basic[i].ymax = i0;
    }

    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            uint32_t e = (uint32_t)labels[i][j];
            if (e > 0) {
                uint32_t r = e - 1;
                RoIs_basic[r].S += 1;
                RoIs_basic[r].id = e;
                RoIs_basic[r].Sx += j;
                RoIs_basic[r].Sy += i;
                RoIs_basic[r].Sx2 += j * j;
                RoIs_basic[r].Sy2 += i * i;
                RoIs_basic[r].Sxy += j * i;
                if (j < (int)RoIs_basic[r].xmin)
                    RoIs_basic[r].xmin = j;
                if (j > (int)RoIs_basic[r].xmax)
                    RoIs_basic[r].xmax = j;
                if (i < (int)RoIs_basic[r].ymin)
                    RoIs_basic[r].ymin = i;
                if (i > (int)RoIs_basic[r].ymax)
                    RoIs_basic[r].ymax = i;
            }
        }
    }

    for (size_t i = 0; i < n_RoIs; i++) {
        RoIs_basic[i].x = (float)RoIs_basic[i].Sx / (float)RoIs_basic[i].S;
        RoIs_basic[i].y = (float)RoIs_basic[i].Sy / (float)RoIs_basic[i].S;
    }
}

uint32_t features_filter_surface(const uint32_t** in_labels, uint32_t** out_labels, const int i0, const int i1,
                                 const int j0, const int j1, RoI_basic_t* RoIs_basic, const size_t n_RoIs,
                                 const uint32_t S_min, const uint32_t S_max) {
    if (out_labels != NULL && (void*)in_labels != (void*)out_labels)
        for (int i = i0; i <= i1; i++)
            memset(out_labels[i], 0, (j1 - j0 + 1) * sizeof(uint32_t));

    uint32_t x0, x1, y0, y1, id;
    uint32_t cur_label = 1;
    for (size_t i = 0; i < n_RoIs; i++) {
        if (RoIs_basic[i].id) {
            id = RoIs_basic[i].id;
            x0 = RoIs_basic[i].ymin;
            x1 = RoIs_basic[i].ymax;
            y0 = RoIs_basic[i].xmin;
            y1 = RoIs_basic[i].xmax;
            if (S_min > RoIs_basic[i].S || RoIs_basic[i].S > S_max) {
                RoIs_basic[i].id = 0;
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
            if (out_labels != NULL) {
                for (uint32_t k = x0; k <= x1; k++) {
                    for (uint32_t l = y0; l <= y1; l++) {
                        if (in_labels[k][l] == id) {
                            out_labels[k][l] = cur_label;
                        }
                    }
                }
            }
            cur_label++;
        }
    }

    return cur_label - 1;
}

uint32_t features_merge_CCL_HI_v2(const uint32_t** in_labels, const uint8_t** img_HI, uint32_t** out_labels,
                                  const int i0, const int i1, const int j0, const int j1, RoI_basic_t* RoIs_basic,
                                  const size_t n_RoIs, const uint32_t S_min, const uint32_t S_max) {
    if (out_labels != NULL && (void*)in_labels != (void*)out_labels)
        for (int i = i0; i <= i1; i++)
            memset(out_labels[i], 0, (j1 - j0 + 1) * sizeof(uint32_t));

    uint32_t x0, x1, y0, y1, id;
    uint32_t cur_label = 1;
    for (size_t i = 0; i < n_RoIs; i++) {
        if (RoIs_basic[i].id) {
            id = RoIs_basic[i].id;
            x0 = RoIs_basic[i].ymin;
            x1 = RoIs_basic[i].ymax;
            y0 = RoIs_basic[i].xmin;
            y1 = RoIs_basic[i].xmax;
            if (S_min > RoIs_basic[i].S || RoIs_basic[i].S > S_max) {
                RoIs_basic[i].id = 0;
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
            RoIs_basic[i].id = 0;
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

    return cur_label - 1;
}

uint32_t features_merge_CCL_HI_v3(const uint32_t** in_labels, const uint8_t** img, uint32_t** out_labels, const int i0,
                                  const int i1, const int j0, const int j1, RoI_basic_t* RoIs_basic,
                                  const size_t n_RoIs, const uint32_t S_min, const uint32_t S_max,
                                  const uint8_t threshold_high, const uint8_t no_labels_zeros_init) {
    if (out_labels != NULL && (void*)in_labels != (void*)out_labels && !no_labels_zeros_init)
        for (int i = i0; i <= i1; i++)
            memset(out_labels[i], 0, (j1 - j0 + 1) * sizeof(uint32_t));

    uint32_t cur_label = 1;
    for (size_t i = 0; i < n_RoIs; i++) {
        if (RoIs_basic[i].id) {
            uint32_t id = RoIs_basic[i].id;
            uint32_t x0 = RoIs_basic[i].ymin;
            uint32_t x1 = RoIs_basic[i].ymax;
            uint32_t y0 = RoIs_basic[i].xmin;
            uint32_t y1 = RoIs_basic[i].xmax;
            if (S_min > RoIs_basic[i].S || RoIs_basic[i].S > S_max) {
                RoIs_basic[i].id = 0;
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
                    if (img[k][l] >= threshold_high && in_labels[k][l] == id) {
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
            RoIs_basic[i].id = 0;
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

    return cur_label - 1;
}

size_t features_shrink(const RoI_basic_t* RoIs_basic_src, const RoI_magn_t* RoIs_magn_src,
                       const RoI_elli_t* RoIs_elli_src, const size_t n_RoIs_src, RoI_basic_t* RoIs_basic_dst,
                       RoI_magn_t* RoIs_magn_dst, RoI_elli_t* RoIs_elli_dst) {
    size_t cpt = 0;
    for (size_t i = 0; i < n_RoIs_src; i++) {
        if (RoIs_basic_src[i].id) {
            RoIs_basic_dst[cpt] = RoIs_basic_src[i];
            RoIs_basic_dst[cpt].id = cpt + 1;
            if (RoIs_magn_src && RoIs_magn_dst)
                RoIs_magn_dst[cpt] = RoIs_magn_src[i];
            if (RoIs_elli_src && RoIs_elli_dst)
                RoIs_elli_dst[cpt] = RoIs_elli_src[i];
            cpt++;
        }
    }
    return cpt;
}

void features_compute_magnitude(const uint8_t** img, const int i0, const int i1, const int j0, const int j1,
                                const uint32_t** labels, const RoI_basic_t* RoIs_basic, RoI_magn_t* RoIs_magn,
                                const size_t n_RoIs) {
    // for each RoI (= Region of Interest = object)
    for (uint32_t r = 0; r < n_RoIs; r++) {
        // initialize magnitude & sat_count
        RoIs_magn[r].magnitude = 0;
        RoIs_magn[r].sat_count = 0;

        // width and height of the current RoI
        int32_t w = (int32_t)(RoIs_basic[r].xmax - RoIs_basic[r].xmin) + 1;
        int32_t h = (int32_t)(RoIs_basic[r].ymax - RoIs_basic[r].ymin) + 1;

        // bounding box around the RoI + extra space to consider local noise level
        // here this is important to cast 'RoIs_basic_ymin' and 'RoIs_basic_xmin' into signed integers because the
        // subtraction with 'h' or 'w' can result in a negative number
        int32_t ymin = (int32_t)RoIs_basic[r].ymin - h > i0 ? (int32_t)RoIs_basic[r].ymin - h : i0;
        int32_t ymax = (int32_t)RoIs_basic[r].ymax + h < i1 ? (int32_t)RoIs_basic[r].ymax + h : i1;
        int32_t xmin = (int32_t)RoIs_basic[r].xmin - w > j0 ? (int32_t)RoIs_basic[r].xmin - w : j0;
        int32_t xmax = (int32_t)RoIs_basic[r].xmax + w < j1 ? (int32_t)RoIs_basic[r].xmax + w : j1;

        uint32_t acc_noise = 0; // accumulate noisy pixels (= dark pixels)
        uint32_t count_noise = 0; // count the number of noisy pixels
        uint32_t count_px = 0; // count the number of pixels for the current RoI (= bright pixels)

        // moving in a square (bigger that the real bounding box) around the current RoI
        for (int32_t i = ymin; i <= ymax; i++) {
            for (int32_t j = xmin; j <= xmax; j++) {
                // get the label from the current pixel position
                // if l != 0 then it is a RoI, else it is a dark / noisy pixel
                uint32_t l = labels[i][j];
                // check if the current pixel belong to the current RoI (same bounding box can share multiple RoIs)
                if (l == r + 1) {
                    RoIs_magn[r].magnitude += (uint32_t)img[i][j];
                    count_px++;
                    // increment the saturation counter if the current pixel is saturated
                    if (img[i][j] == 255)
                        RoIs_magn[r].sat_count++;
                } else if (l == 0) {
                    acc_noise += (uint32_t)img[i][j];
                    count_noise++;
                }
            }
        }
        assert(count_px == RoIs_basic[r].S); // useless check, only for debugging purpose
        // compute mean noise value
        uint32_t noise = acc_noise / count_noise;
        // subtract mean noise to the current RoI (Region of Interest) magnitude
        RoIs_magn[r].magnitude -= noise * RoIs_basic[r].S;
    }
}

void features_compute_ellipse(const RoI_basic_t* RoIs_basic, RoI_elli_t* RoIs_elli, const size_t n_RoIs) {
    for (size_t e = 0; e < n_RoIs; e++) {
        double S = RoIs_basic[e].S;

        double Sx = RoIs_basic[e].Sx;
        double Sy = RoIs_basic[e].Sy;

        // moments non centres
        double Sxx = RoIs_basic[e].Sx2;
        double Sxy = RoIs_basic[e].Sxy;
        double Syy = RoIs_basic[e].Sy2;

        // moments centres
        double m20 = S * Sxx - Sx * Sx;
        double m11 = S * Sxy - Sx * Sy;
        double m02 = S * Syy - Sy * Sy;

        // par construction a > b
        double a2 = (m20 + m02 + sqrt((m20 - m02) * (m20 - m02) + 4.0 * m11 * m11)) / (2.0 * S);
        double b2 = (m20 + m02 - sqrt((m20 - m02) * (m20 - m02) + 4.0 * m11 * m11)) / (2.0 * S);

        float a = sqrt(a2);
        float b = sqrt(b2);

        RoIs_elli[e].a = a;
        RoIs_elli[e].b = b;
    }
}

void features_labels_zero_init(const RoI_basic_t* RoIs_basic, const size_t n_RoIs, uint32_t** labels) {
    for (size_t i = 0; i < n_RoIs; i++) {
        uint32_t x0 = RoIs_basic[i].ymin;
        uint32_t x1 = RoIs_basic[i].ymax;
        uint32_t y0 = RoIs_basic[i].xmin;
        uint32_t y1 = RoIs_basic[i].xmax;
        for (uint32_t k = x0; k <= x1; k++)
            for (uint32_t l = y0; l <= y1; l++)
                labels[k][l] = 0;
    }
}
