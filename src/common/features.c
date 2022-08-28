/*
 * Copyright (c) 2017-2018, Arthur Hennequin, LIP6, UPMC, CNRS
 * Copyright (c) 2020-2020, Lionel Lacassagne, all rights reserved, LIP6 Sorbonne University, CNRS
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "tools.h"
#include "features.h"

ROI_t* features_alloc_ROI_array(const size_t max_size) {
    ROI_t* ROI_array = (ROI_t*)malloc(sizeof(ROI_t));
    ROI_array->id = (uint16_t*)malloc(max_size * sizeof(uint16_t));
    ROI_array->frame = (size_t*)malloc(max_size * sizeof(size_t));
    ROI_array->xmin = (uint16_t*)malloc(max_size * sizeof(uint16_t));
    ROI_array->xmax = (uint16_t*)malloc(max_size * sizeof(uint16_t));
    ROI_array->ymin = (uint16_t*)malloc(max_size * sizeof(uint16_t));
    ROI_array->ymax = (uint16_t*)malloc(max_size * sizeof(uint16_t));
    ROI_array->S = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    ROI_array->Sx = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    ROI_array->Sy = (uint32_t*)malloc(max_size * sizeof(uint32_t));
    ROI_array->x = (float*)malloc(max_size * sizeof(float));
    ROI_array->y = (float*)malloc(max_size * sizeof(float));
    ROI_array->dx = (float*)malloc(max_size * sizeof(float));
    ROI_array->dy = (float*)malloc(max_size * sizeof(float));
    ROI_array->error = (float*)malloc(max_size * sizeof(float));
    ROI_array->time = (int32_t*)malloc(max_size * sizeof(int32_t));
    ROI_array->time_motion = (int32_t*)malloc(max_size * sizeof(int32_t));
    ROI_array->prev_id = (int32_t*)malloc(max_size * sizeof(int32_t));
    ROI_array->next_id = (int32_t*)malloc(max_size * sizeof(int32_t));
    ROI_array->is_moving = (uint8_t*)malloc(max_size * sizeof(uint8_t));
    ROI_array->is_extrapolated = (uint8_t*)malloc(max_size * sizeof(uint8_t));
    ROI_array->_max_size = max_size;
    return ROI_array;
}

void features_init_ROI_array(ROI_t* ROI_array) {
    memset(ROI_array->id, 0, ROI_array->_max_size * sizeof(uint16_t));
    memset(ROI_array->frame, 0, ROI_array->_max_size * sizeof(size_t));
    memset(ROI_array->xmin, 0, ROI_array->_max_size * sizeof(uint16_t));
    memset(ROI_array->xmax, 0, ROI_array->_max_size * sizeof(uint16_t));
    memset(ROI_array->ymin, 0, ROI_array->_max_size * sizeof(uint16_t));
    memset(ROI_array->ymax, 0, ROI_array->_max_size * sizeof(uint16_t));
    memset(ROI_array->S, 0, ROI_array->_max_size * sizeof(uint32_t));
    memset(ROI_array->Sx, 0, ROI_array->_max_size * sizeof(uint32_t));
    memset(ROI_array->Sy, 0, ROI_array->_max_size * sizeof(uint32_t));
    memset(ROI_array->x, 0, ROI_array->_max_size * sizeof(float));
    memset(ROI_array->y, 0, ROI_array->_max_size * sizeof(float));
    memset(ROI_array->dx, 0, ROI_array->_max_size * sizeof(float));
    memset(ROI_array->dy, 0, ROI_array->_max_size * sizeof(float));
    memset(ROI_array->error, 0, ROI_array->_max_size * sizeof(float));
    memset(ROI_array->time, 0, ROI_array->_max_size * sizeof(int32_t));
    memset(ROI_array->time_motion, 0, ROI_array->_max_size * sizeof(int32_t));
    memset(ROI_array->prev_id, 0, ROI_array->_max_size * sizeof(int32_t));
    memset(ROI_array->next_id, 0, ROI_array->_max_size * sizeof(int32_t));
    memset(ROI_array->is_moving, 0, ROI_array->_max_size * sizeof(uint8_t));
    memset(ROI_array->is_extrapolated, 0, ROI_array->_max_size * sizeof(uint8_t));
    ROI_array->_size = 0;
}

void features_copy_ROI_array(const ROI_t* ROI_array_src, ROI_t* ROI_array_dest, const int i_src, const int i_dest) {
    ROI_array_dest->id[i_dest] = ROI_array_src->id[i_src];
    ROI_array_dest->frame[i_dest] = ROI_array_src->frame[i_src];
    ROI_array_dest->xmin[i_dest] = ROI_array_src->xmin[i_src];
    ROI_array_dest->xmax[i_dest] = ROI_array_src->xmax[i_src];
    ROI_array_dest->ymin[i_dest] = ROI_array_src->ymin[i_src];
    ROI_array_dest->ymax[i_dest] = ROI_array_src->ymax[i_src];
    ROI_array_dest->S[i_dest] = ROI_array_src->S[i_src];
    ROI_array_dest->Sx[i_dest] = ROI_array_src->Sx[i_src];
    ROI_array_dest->Sy[i_dest] = ROI_array_src->Sy[i_src];
    ROI_array_dest->x[i_dest] = ROI_array_src->x[i_src];
    ROI_array_dest->y[i_dest] = ROI_array_src->y[i_src];
    ROI_array_dest->dx[i_dest] = ROI_array_src->dx[i_src];
    ROI_array_dest->dy[i_dest] = ROI_array_src->dy[i_src];
    ROI_array_dest->error[i_dest] = ROI_array_src->error[i_src];
    ROI_array_dest->time[i_dest] = ROI_array_src->time[i_src];
    ROI_array_dest->time_motion[i_dest] = ROI_array_src->time_motion[i_src];
    ROI_array_dest->prev_id[i_dest] = ROI_array_src->prev_id[i_src];
    ROI_array_dest->next_id[i_dest] = ROI_array_src->next_id[i_src];
    ROI_array_dest->is_moving[i_dest] = ROI_array_src->is_moving[i_src];
    ROI_array_dest->is_extrapolated[i_dest] = ROI_array_src->is_extrapolated[i_src];
}

void features_free_ROI_array(ROI_t* ROI_array) {
    free(ROI_array->id);
    free(ROI_array->frame);
    free(ROI_array->xmin);
    free(ROI_array->xmax);
    free(ROI_array->ymin);
    free(ROI_array->ymax);
    free(ROI_array->S);
    free(ROI_array->Sx);
    free(ROI_array->Sy);
    free(ROI_array->x);
    free(ROI_array->y);
    free(ROI_array->dx);
    free(ROI_array->dy);
    free(ROI_array->error);
    free(ROI_array->time);
    free(ROI_array->time_motion);
    free(ROI_array->prev_id);
    free(ROI_array->next_id);
    free(ROI_array->is_moving);
    free(ROI_array->is_extrapolated);
    free(ROI_array);
}

void features_clear_index_ROI_array(ROI_t* ROI_array, const size_t r) {
    ROI_array->id[r] = 0;
    ROI_array->frame[r] = 0;
    ROI_array->xmin[r] = 0;
    ROI_array->xmax[r] = 0;
    ROI_array->ymin[r] = 0;
    ROI_array->ymax[r] = 0;
    ROI_array->S[r] = 0;
    ROI_array->Sx[r] = 0;
    ROI_array->Sy[r] = 0;
    ROI_array->x[r] = 0;
    ROI_array->y[r] = 0;
    ROI_array->dx[r] = 0;
    ROI_array->dy[r] = 0;
    ROI_array->error[r] = 0;
    ROI_array->time[r] = 0;
    ROI_array->time_motion[r] = 0;
    ROI_array->prev_id[r] = 0;
    ROI_array->next_id[r] = 0;
    ROI_array->is_moving[r] = 0;
    ROI_array->is_extrapolated[r] = 0;
}

ROI_history_t* features_alloc_ROI_history(const size_t max_history_size, const size_t max_ROI_size) {
    ROI_history_t* ROI_hist = (ROI_history_t*)malloc(sizeof(ROI_history_t));
    ROI_hist->_max_size = max_history_size;
    ROI_hist->array = (ROI_t**)malloc(ROI_hist->_max_size * sizeof(ROI_t*));
    for (int i = 0; i < ROI_hist->_max_size; i++)
        ROI_hist->array[i] = features_alloc_ROI_array(max_ROI_size);
    return ROI_hist;
}

void features_free_ROI_history(ROI_history_t* ROI_hist) {
    for (int i = 0; i < ROI_hist->_max_size; i++)
        features_free_ROI_array(ROI_hist->array[i]);
    free(ROI_hist->array);
    free(ROI_hist);
}

void features_rotate_ROI_history(ROI_history_t* ROI_hist) {
    ROI_t* last_ROI_tmp = ROI_hist->array[ROI_hist->_max_size -1];
    for (int i = ROI_hist->_max_size -2; i >= 0; i--)
        ROI_hist->array[i + 1] = ROI_hist->array[i];
    ROI_hist->array[0] = last_ROI_tmp;
}

void features_init_ROI(ROI_t* stats, int n) {
    for (int i = 0; i < n; i++)
        memset(stats + i, 0, sizeof(ROI_t));
}

void features_extract(const uint32_t** img, const int i0, const int i1, const int j0, const int j1, const int n_ROI,
                      ROI_t* ROI_array) {
    features_init_ROI_array(ROI_array);
    ROI_array->_size = n_ROI;

    for (int i = 0; i < ROI_array->_size; i++) {
        ROI_array->xmin[i] = j1;
        ROI_array->xmax[i] = j0;
        ROI_array->ymin[i] = i1;
        ROI_array->ymax[i] = i0;
    }

    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            uint32_t e = img[i][j];
            if (e > 0) {
                uint32_t r = e - 1;
                ROI_array->S[r] += 1;
                ROI_array->id[r] = e;
                ROI_array->Sx[r] += j;
                ROI_array->Sy[r] += i;
                if (j < ROI_array->xmin[r])
                    ROI_array->xmin[r] = j;
                if (j > ROI_array->xmax[r])
                    ROI_array->xmax[r] = j;
                if (i < ROI_array->ymin[r])
                    ROI_array->ymin[r] = i;
                if (i > ROI_array->ymax[r])
                    ROI_array->ymax[r] = i;
            }
        }
    }

    for (int i = 0; i < ROI_array->_size; i++) {
        ROI_array->x[i] = (double)ROI_array->Sx[i] / (double)ROI_array->S[i];
        ROI_array->y[i] = (double)ROI_array->Sy[i] / (double)ROI_array->S[i];
    }
}

void features_merge_HI_CCL_v2(const uint32_t** M, const uint32_t** HI_in, uint32_t** HI_out, const int i0, const int i1,
                              const int j0, const int j1, ROI_t* ROI_array, const int S_min, const int S_max) {
    if ((void*)HI_in != (void*)HI_out)
        for (int i = i0; i <= i1; i++)
            memcpy(HI_out[i] + j0, HI_in[i] + j0, sizeof(uint32_t) * ((j1 - j0) + 1));

    int x0, x1, y0, y1, id;
    for (int i = 0; i < ROI_array->_size; i++) {
        if (ROI_array->S[i]) {
            id = ROI_array->id[i];
            x0 = ROI_array->ymin[i];
            x1 = ROI_array->ymax[i];
            y0 = ROI_array->xmin[i];
            y1 = ROI_array->xmax[i];
            if (S_min > ROI_array->S[i] || ROI_array->S[i] > S_max) {
                ROI_array->S[i] = 0;
                // JUSTE POUR DEBUG (Affichage frames)
                for (int k = x0; k <= x1; k++) {
                    for (int l = y0; l <= y1; l++) {
                        if (M[k][l] == id)
                            HI_out[k][l] = 0;
                    }
                }
                continue;
            }
            for (int k = x0; k <= x1; k++) {
                for (int l = y0; l <= y1; l++) {
                    if (HI_out[k][l]) {
                        for (k = x0; k <= x1; k++) {
                            for (l = y0; l <= y1; l++) {
                                if (M[k][l] == id)
                                    HI_out[k][l] = i + 1;
                            }
                        }
                        goto next;
                    }
                }
            }
            ROI_array->S[i] = 0;
        next:;
        }
    }
}

void features_filter_surface(ROI_t* ROI_array, uint32_t** img, uint32_t threshold_min, uint32_t threshold_max) {
    // Doit on vraiment modifier l'image de départ? ou juste les stats.
    uint32_t S, e;
    int i0, i1, j0, j1;
    uint16_t id;

    for (int i = 0; i < ROI_array->_size; i++) {
        S = ROI_array->S[i];
        id = ROI_array->id[i];

        if (S == 0)
            continue; // DEBUG

        if (S < threshold_min || S > threshold_max) {
            ROI_array->S[i] = 0;

            // pour affichage debbug
            i0 = ROI_array->ymin[i];
            i1 = ROI_array->ymax[i];
            j0 = ROI_array->xmin[i];
            j1 = ROI_array->xmax[i];
            for (int i = i0; i <= i1; i++) {
                for (int j = j0; j <= j1; j++) {
                    e = img[i][j];
                    if (e == id) {
                        img[i][j] = 0;
                    }
                }
            }
        }
    }
}

void features_shrink_stats(const ROI_t* ROI_array_src, ROI_t* ROI_array_dest) {
    int cpt = 0;
    for (int i = 0; i < ROI_array_src->_size; i++) {
        if (ROI_array_src->S[i] > 0) {
            features_copy_ROI_array(ROI_array_src, ROI_array_dest, i, cpt);
            ROI_array_dest->id[cpt] = cpt + 1;
            cpt++;
        }
    }
    ROI_array_dest->_size = cpt;
}

void features_rigid_registration(const ROI_t* ROI_array0, const ROI_t* ROI_array1, double* theta, double* tx,
                                 double* ty) {
    double Sx, Sxp, Sy, Syp, Sx_xp, Sxp_y, Sx_yp, Sy_yp;
    double x0, y0, x1, y1;
    double a, b;
    double xg, yg, xpg, ypg;
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
    for (int i = 0; i < ROI_array0->_size; i++) {
        asso = ROI_array0->next_id[i];

        if (ROI_array0->S[i] > 0 && asso) {
            cpt++;
            Sx += ROI_array0->x[i];
            Sy += ROI_array0->y[i];
            Sxp += ROI_array1->x[asso - 1];
            Syp += ROI_array1->y[asso - 1];
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
    for (int i = 0; i < ROI_array0->_size; i++) {
        asso = ROI_array0->next_id[i];

        if (ROI_array0->S[i] > 0 && asso) {
            // cpt++;
            x0 = ROI_array0->x[i] - xg;
            y0 = ROI_array0->y[i] - yg;
            x1 = ROI_array1->x[asso - 1] - xpg;
            y1 = ROI_array1->y[asso - 1] - ypg;

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

    *theta = atan2(a, b);
    *tx = xpg - cos(*theta) * xg + sin(*theta) * yg;
    *ty = ypg - sin(*theta) * xg - cos(*theta) * yg;
}

void features_rigid_registration_corrected(ROI_t* ROI_array0, const ROI_t* ROI_array1, double* theta, double* tx,
                                           double* ty, double mean_error, double std_deviation) {
    double Sx, Sxp, Sy, Syp, Sx_xp, Sxp_y, Sx_yp, Sy_yp;
    double x0, y0, x1, y1;
    double a, b;
    double xg, yg, xpg, ypg;
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

    int cpt1 = 0;
    // parcours tab assos
    for (int i = 0; i < ROI_array0->_size; i++) {
        if (fabs(ROI_array0->error[i] - mean_error) > std_deviation) {
            ROI_array0->is_moving[i] = 1;
            cpt1++;
            continue;
        }
        asso = ROI_array0->next_id[i]; // assos[i];
        if (ROI_array0->S[i] > 0 && asso) {
            Sx += ROI_array0->x[i];
            Sy += ROI_array0->y[i];
            Sxp += ROI_array1->x[asso - 1];
            Syp += ROI_array1->y[asso - 1];
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
    for (int i = 0; i < ROI_array0->_size; i++) {
        if (fabs(ROI_array0->error[i] - mean_error) > std_deviation)
            continue;
        asso = ROI_array0->next_id[i];
        if (ROI_array0->S[i] > 0 && asso) {
            // cpt++;
            x0 = ROI_array0->x[i] - xg;
            y0 = ROI_array0->y[i] - yg;
            x1 = ROI_array1->x[asso - 1] - xpg;
            y1 = ROI_array1->y[asso - 1] - ypg;

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

    *theta = atan2(a, b);
    *tx = xpg - cos(*theta) * xg + sin(*theta) * yg;
    *ty = ypg - sin(*theta) * xg - cos(*theta) * yg;
}

// TODO: Pour l'optimisation : faire une version errorMoy_corrected()
double features_compute_mean_error(const ROI_t* ROI_array) {
    double S = 0.0;
    int cpt = 0;
    for (int i = 0; i < ROI_array->_size; i++) {
        if (ROI_array->is_moving[i] || !ROI_array->next_id[i])
            continue;
        S += ROI_array->error[i];
        cpt++;
    }
    return S / cpt;
}

// TODO: Pour l'optimisation : faire une version ecartType_corrected()
double features_compute_std_deviation(const ROI_t* ROI_array, const double mean_error) {
    double S = 0.0;
    int cpt = 0;
    for (int i = 0; i < ROI_array->_size; i++) {
        if (ROI_array->is_moving[i] || !ROI_array->next_id[i])
            continue;
        float e = ROI_array->error[i];
        S += ((e - mean_error) * (e - mean_error));
        cpt++;
    }
    return sqrt(S / cpt);
}

void features_motion_extraction(ROI_t* ROI_array0, const ROI_t* ROI_array1, double theta, double tx, double ty) {
    int cc1;
    double x, y, xp, yp;
    float dx, dy;
    float e;

    for (int i = 0; i < ROI_array0->_size; i++) {
        cc1 = ROI_array0->next_id[i];
        if (cc1) {
            // coordonees du point dans l'image I+1
            xp = ROI_array1->x[cc1 - 1];
            yp = ROI_array1->y[cc1 - 1];
            // calcul de (x,y) pour l'image I
            x = cos(theta) * (xp - tx) + sin(theta) * (yp - ty);
            y = cos(theta) * (yp - ty) - sin(theta) * (xp - tx);

            // pas besoin de stocker dx et dy (juste pour l'affichage du debug)
            dx = x - ROI_array0->x[i];
            dy = y - ROI_array0->y[i];
            ROI_array0->dx[i] = dx;
            ROI_array0->dy[i] = dy;

            e = sqrt(dx * dx + dy * dy);
            ROI_array0->error[i] = e;
        }
    }
}

void features_compute_motion(const ROI_t* ROI_array1, ROI_t* ROI_array0, double* theta, double* tx, double* ty) {
    features_rigid_registration((const ROI_t*)ROI_array0, (const ROI_t*)ROI_array1, theta, tx, ty);
    features_motion_extraction(ROI_array0, (const ROI_t*)ROI_array1, *theta, *tx, *ty);

    double mean_error = features_compute_mean_error((const ROI_t*)ROI_array0);
    double std_deviation = features_compute_std_deviation((const ROI_t*)ROI_array0, mean_error);

    // saveErrorMoy("first_error.txt", mean_error, std_deviation);

    features_rigid_registration_corrected(ROI_array0, (const ROI_t*)ROI_array1, theta, tx, ty, mean_error,
                                          std_deviation);
    features_motion_extraction(ROI_array0, (const ROI_t*)ROI_array1, *theta, *tx, *ty);
}

void features_print_stats(ROI_t* stats, int n)
{
    int cpt = 0;
    for (int i = 0; i < n; i++) {
        if (stats->S[i] > 0) {
            cpt++;
        }
    }
    printf("Nombre de CC : %d\n", cpt);

    if (cpt == 0)
        return;

    for (int i = 0; i < n; i++) {
        if (stats->S[i] > 0)
            printf("%4d \t %4d \t %4d \t %4d \t %4d \t %3d \t %4d \t %4d \t %7.1f \t %7.1f \t %4d \t %4d \t %4d \t "
                   "%7.1lf \t %d\n",
                   stats->id[i], stats->xmin[i], stats->xmax[i], stats->ymin[i], stats->ymax[i], stats->S[i],
                   stats->Sx[i], stats->Sy[i], stats->x[i], stats->y[i], stats->prev_id[i], stats->next_id[i],
                   stats->time[i], stats->error[i], stats->is_moving[i]);
    }
    printf("\n");
}

void features_parse_stats(const char* filename, ROI_t* stats, int* n)
{
    char lines[200];
    int id, xmin, xmax, ymin, ymax, s, sx, sy, prev_id, next_id;
    double x, y;
    float dx, dy, error;
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "(EE) cannot open file '%s'\n", filename);
        return;
    }

    // pour l'instant, n représente l'id max des stas mais c'est à changer
    fgets(lines, 100, file);
    sscanf(lines, "%d", n);

    while (fgets(lines, 200, file)) {
        sscanf(lines, "%d %d %d %d %d %d %d %d %lf %lf %f %f %f %d %d", &id, &xmin, &xmax, &ymin, &ymax, &s, &sx, &sy,
               &x, &y, &dx, &dy, &error, &prev_id, &next_id);
        stats->id[id - 1] = id;
        stats->xmin[id - 1] = xmin;
        stats->xmax[id - 1] = xmax;
        stats->ymin[id - 1] = ymin;
        stats->ymax[id - 1] = ymax;
        stats->S[id - 1] = s;
        stats->Sx[id - 1] = sx;
        stats->Sy[id - 1] = sy;
        stats->x[id - 1] = x;
        stats->y[id - 1] = y;
        stats->dx[id - 1] = dx;
        stats->dy[id - 1] = dy;
        stats->error[id - 1] = error;
        stats->prev_id[id - 1] = prev_id;
        stats->next_id[id - 1] = next_id;
    }
    fclose(file);
}

int find_corresponding_track(const track_t* track_array, const ROI_t* ROI_array, const int ROI_id,
                             const unsigned age) {
    assert(age == 0 || age == 1);
    for (size_t t = track_array->_offset; t < track_array->_size; t++) {
        int cur_ROI_id = (age == 0) ? track_array->end->id[t] : ROI_array->id[track_array->end->prev_id[t]];
        if (cur_ROI_id <= 0)
            continue;
        if (ROI_id == cur_ROI_id)
            return t;
    }
    return -1;
}

void features_save_stats_file(FILE* f, const ROI_t* ROI_array, const track_t* track_array, const unsigned age) {
    int cpt = 0;
    for (int i = 0; i < ROI_array->_size; i++)
        if (ROI_array->S[i] != 0)
            cpt++;

    fprintf(f, "# Regions of interest (ROI) [%d]: \n", cpt);
    if (cpt) {
        fprintf(f, "# ------||----------------||---------------------------||---------------------------||-------------------||-----------------\n");
        fprintf(f, "#   ROI ||      Track     ||        Bounding Box       ||   Surface (S in pixels)   ||      Center       ||       Time      \n");
        fprintf(f, "# ------||----------------||---------------------------||---------------------------||-------------------||-----------------\n");
        fprintf(f, "# ------||------|---------||------|------|------|------||-----|----------|----------||---------|---------||--------|--------\n");
        fprintf(f, "#    ID ||   ID |    Type || xmin | xmax | ymin | ymax ||   S |       Sx |       Sy ||       x |       y ||    All | Motion \n");
        fprintf(f, "# ------||------|---------||------|------|------|------||-----|----------|----------||---------|---------||--------|--------\n");
    }

    for (int i = 0; i < ROI_array->_size; i++) {
        if (ROI_array->S[i] != 0) {
            int t = find_corresponding_track(track_array, ROI_array, ROI_array->id[i], age);
            char task_id_str[5];
            if (t == -1)
                strcpy(task_id_str, "   -");
            else
                sprintf(task_id_str, "%4d", track_array->id[t]);
            char task_obj_type[64];
            if (t == -1)
                strcpy(task_obj_type, "      -");
            else
                sprintf(task_obj_type, "%s", g_obj_to_string_with_spaces[track_array->obj_type[t]]);
            fprintf(f, "   %4d || %s | %s || %4d | %4d | %4d | %4d || %3d | %8d | %8d || %7.1f | %7.1f || %6d | %6d \n",
                    ROI_array->id[i], task_id_str, task_obj_type, ROI_array->xmin[i], ROI_array->xmax[i],
                    ROI_array->ymin[i], ROI_array->ymax[i], ROI_array->S[i], ROI_array->Sx[i], ROI_array->Sy[i],
                    ROI_array->x[i], ROI_array->y[i], ROI_array->time[i], ROI_array->time_motion[i]);
        }
    }
}

void features_save_stats(const char* filename, const ROI_t* ROI_array, const track_t* track_array, const unsigned age) {
    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }
    features_save_stats_file(f, ROI_array, track_array, age);
    fclose(f);
}

void features_save_motion(const char* filename, double theta, double tx, double ty, int frame) {
    FILE* f = fopen(filename, "a");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }

    fprintf(f, "%d - %d\n", frame, frame + 1);
    fprintf(f, "%6.7f \t %6.4f \t %6.4f \n", theta, tx, ty);
    fprintf(f, "---------------------------------------------------------------\n");
    fclose(f);
}

void features_save_error(const char* filename, const ROI_t* ROI_array) {
    double S = 0;
    int cpt = 0;
    FILE* f = fopen(filename, "a");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }

    for (int i = 0; i < ROI_array->_size; i++) {
        if (ROI_array->S[i] > 0) {
            S += ROI_array->error[i];
            cpt++;
        }
    }

    fprintf(f, "%.2f\n", S / cpt);
    fclose(f);
}

void features_save_error_moy(const char* filename, double mean_error, double std_deviation) {
    char path[200];
    FILE* f = fopen(filename, "a");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", path);
        exit(1);
    }
    fprintf(f, "%5.2f \t %5.2f \n", mean_error, std_deviation);
    fclose(f);
}

void features_save_motion_extraction(const char* filename, const ROI_t* ROI_array, const double theta, const double tx,
                                     const double ty, const int frame) {
    // Version DEBUG : il faut implémenter une version pour le main
    FILE* f = fopen(filename, "a");
    if (f == NULL) {
        fprintf(stderr, "motion : cannot open file\n");
        return;
    }

    double mean_error = features_compute_mean_error(ROI_array);
    double std_deviation = features_compute_std_deviation(ROI_array, mean_error);

    for (int i = 0; i < ROI_array->_size; i++) {
        float e = ROI_array->error[i];
        // si mouvement detecté
        if (fabs(e - mean_error) > 1.5 * std_deviation) {
            fprintf(f, "%d - %d\n", frame, frame + 1);
            fprintf(f,
                    "CC en mouvement: %2d \t dx:%.3f \t dy: %.3f \t xmin: %3d \t xmax: %3d \t ymin: %3d \t ymax: %3d\n",
                    ROI_array->id[i], ROI_array->dx[i], ROI_array->dy[i], ROI_array->xmin[i], ROI_array->xmax[i],
                    ROI_array->ymin[i], ROI_array->ymax[i]);
            fprintf(f, "---------------------------------------------------------------\n");
        }
    }
    fclose(f);
}