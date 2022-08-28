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

ROI_array_t* features_alloc_ROI_array(const size_t max_size) {
    ROI_array_t* ROI_array = (ROI_array_t*)malloc(sizeof(ROI_array_t));
    ROI_array->data = (ROI_t*)malloc(sizeof(ROI_t) * max_size);
    ROI_array->max_size = max_size;
    ROI_array->size = 0;
    return ROI_array;
}

void features_init_ROI_array(ROI_array_t* ROI_array) {
    ROI_array->size = 0;
    features_init_ROI(ROI_array->data, ROI_array->max_size);
}

void features_free_ROI_array(ROI_array_t* ROI_array) {
    free(ROI_array->data);
    free(ROI_array);
}

ROI_history_t* features_alloc_ROI_history(const size_t max_history_size, const size_t max_ROI_size) {
    ROI_history_t* ROI_hist = (ROI_history_t*) malloc(sizeof(ROI_history_t));
    ROI_hist->max_size = max_history_size;
    ROI_hist->array = (ROI_array_t**)malloc(ROI_hist->max_size * sizeof(ROI_array_t*));
    for (int i = 0; i < ROI_hist->max_size; i++)
        ROI_hist->array[i] = features_alloc_ROI_array(max_ROI_size);
    return ROI_hist;
}

void features_free_ROI_history(ROI_history_t* ROI_hist) {
    for (int i = 0; i < ROI_hist->max_size; i++)
        features_free_ROI_array(ROI_hist->array[i]);
    free(ROI_hist->array);
    free(ROI_hist);
}

void features_rotate_ROI_history(ROI_history_t* ROI_hist) {
    ROI_array_t* last_ROI_tmp = ROI_hist->array[ROI_hist->max_size -1];
    for (int i = ROI_hist->max_size -2; i >= 0; i--)
        ROI_hist->array[i + 1] = ROI_hist->array[i];
    ROI_hist->array[0] = last_ROI_tmp;
}

void features_init_ROI(ROI_t* stats, int n) {
    for (int i = 0; i < n; i++)
        memset(stats + i, 0, sizeof(ROI_t));
}

void features_extract(const uint32_t** img, const int i0, const int i1, const int j0, const int j1, const int n_ROI,
                      ROI_array_t* ROI_array) {
    ROI_array->size = n_ROI;

    for (int i = 1; i <= ROI_array->size; i++) {
        memset(ROI_array->data + i, 0, sizeof(ROI_t));
        ROI_array->data[i].xmin = j1;
        ROI_array->data[i].xmax = j0;
        ROI_array->data[i].ymin = i1;
        ROI_array->data[i].ymax = i0;
    }

    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            uint32_t e = img[i][j];
            if (e > 0) {
                ROI_array->data[e].S += 1;
                ROI_array->data[e].id = e;
                ROI_array->data[e].Sx += j;
                ROI_array->data[e].Sy += i;
                if (j < ROI_array->data[e].xmin)
                    ROI_array->data[e].xmin = j;
                if (j > ROI_array->data[e].xmax)
                    ROI_array->data[e].xmax = j;
                if (i < ROI_array->data[e].ymin)
                    ROI_array->data[e].ymin = i;
                if (i > ROI_array->data[e].ymax)
                    ROI_array->data[e].ymax = i;
            }
        }
    }

    for (int i = 1; i <= ROI_array->size; i++) {
        ROI_array->data[i].x = (double)ROI_array->data[i].Sx / (double)ROI_array->data[i].S;
        ROI_array->data[i].y = (double)ROI_array->data[i].Sy / (double)ROI_array->data[i].S;
    }
}

void features_merge_HI_CCL_v2(const uint32_t** M, const uint32_t** HI_in, uint32_t** HI_out, const int i0, const int i1,
                              const int j0, const int j1, ROI_array_t* ROI_array, const int S_min, const int S_max) {
    if ((void*)HI_in != (void*)HI_out)
        for (int i = i0; i <= i1; i++)
            memcpy(HI_out[i] + j0, HI_in[i] + j0, sizeof(uint32_t) * ((j1 - j0) + 1));

    int x0, x1, y0, y1, id;
    for (int i = 1; i <= ROI_array->size; i++) {
        ROI_t cc = ROI_array->data[i];
        if (cc.S) {
            id = cc.id;
            x0 = cc.ymin;
            x1 = cc.ymax;
            y0 = cc.xmin;
            y1 = cc.xmax;
            if (S_min > cc.S || cc.S > S_max) {
                ROI_array->data[i].S = 0;
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
                                    HI_out[k][l] = i;
                            }
                        }
                        goto next;
                    }
                }
            }
            ROI_array->data[i].S = 0;
        next:;
        }
    }
}

void features_filter_surface(ROI_t* stats, int n, uint32_t** img, uint32_t threshold_min, uint32_t threshold_max) {
    // Doit on vraiment modifier l'image de départ? ou juste les stats.
    uint32_t S, e;
    int i0, i1, j0, j1;
    uint16_t id;

    for (int i = 1; i <= n; i++) {
        S = stats[i].S;
        id = stats[i].id;

        if (S == 0)
            continue; // DEBUG

        if (S < threshold_min || S > threshold_max) {
            stats[i].S = 0;

            // pour affichage debbug
            i0 = stats[i].ymin;
            i1 = stats[i].ymax;
            j0 = stats[i].xmin;
            j1 = stats[i].xmax;
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

void features_shrink_stats(const ROI_array_t* ROI_array_src, ROI_array_t* ROI_array_dest) {
    int cpt = 0;
    for (int i = 1; i <= ROI_array_src->size; i++) {
        if (ROI_array_src->data[i].S > 0) {
            cpt++;
            memcpy(ROI_array_dest->data + cpt, ROI_array_src->data + i, sizeof(ROI_t));
            ROI_array_dest->data[cpt].id = cpt;
        }
    }
    ROI_array_dest->size = cpt;
}

void features_rigid_registration(const ROI_array_t* ROI_array0, const ROI_array_t* ROI_array1, double* theta,
                                 double* tx, double* ty) {
    double Sx, Sxp, Sy, Syp, Sx_xp, Sxp_y, Sx_yp, Sy_yp;
    ROI_t cc0, cc1;
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
    for (int i = 1; i <= ROI_array0->size; i++) {
        cc0 = ROI_array0->data[i];
        asso = ROI_array0->data[i].next; // assos[i];

        if (cc0.S > 0 && asso) {
            cpt++;
            cc1 = ROI_array1->data[ROI_array0->data[i].next];

            Sx += cc0.x;
            Sy += cc0.y;
            Sxp += cc1.x;
            Syp += cc1.y;
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
    for (int i = 1; i <= ROI_array0->size; i++) {
        cc0 = ROI_array0->data[i];
        asso = ROI_array0->data[i].next;

        if (cc0.S > 0 && asso) {
            // cpt++;
            cc1 = ROI_array1->data[ROI_array0->data[i].next];

            x0 = cc0.x - xg;
            y0 = cc0.y - yg;
            x1 = cc1.x - xpg;
            y1 = cc1.y - ypg;

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

void features_rigid_registration_corrected(ROI_array_t* ROI_array0, const ROI_array_t* ROI_array1, double* theta,
                                           double* tx, double* ty, double mean_error, double std_deviation) {
    double Sx, Sxp, Sy, Syp, Sx_xp, Sxp_y, Sx_yp, Sy_yp;
    ROI_t cc0, cc1;
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
    for (int i = 1; i <= ROI_array0->size; i++) {
        cc0 = ROI_array0->data[i];

        if (fabs(ROI_array0->data[i].error - mean_error) > std_deviation) {
            ROI_array0->data[i].is_moving = 1;
            cpt1++;
            continue;
        }
        asso = ROI_array0->data[i].next; // assos[i];

        if (cc0.S > 0 && asso) {
            cpt++;
            cc1 = ROI_array1->data[ROI_array0->data[i].next];

            Sx += cc0.x;
            Sy += cc0.y;
            Sxp += cc1.x;
            Syp += cc1.y;
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
    for (int i = 1; i <= ROI_array0->size; i++) {
        cc0 = ROI_array0->data[i];

        if (fabs(ROI_array0->data[i].error - mean_error) > std_deviation)
            continue;

        asso = ROI_array0->data[i].next;

        if (cc0.S > 0 && asso) {
            // cpt++;
            cc1 = ROI_array1->data[ROI_array0->data[i].next];

            x0 = cc0.x - xg;
            y0 = cc0.y - yg;
            x1 = cc1.x - xpg;
            y1 = cc1.y - ypg;

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
double features_compute_mean_error(const ROI_array_t* ROI_array) {
    const ROI_t* stats = ROI_array->data;
    int n = ROI_array->size;

    double S = 0.0;
    int cpt = 0;

    for (int i = 1; i <= n; i++) {

        if (stats[i].is_moving || !stats[i].next)
            continue;

        S += stats[i].error;
        cpt++;
    }
    return S / cpt;
}

// TODO: Pour l'optimisation : faire une version ecartType_corrected()
double features_compute_std_deviation(const ROI_array_t* ROI_array, const double mean_error) {
    const ROI_t* stats = ROI_array->data;
    int n = ROI_array->size;

    double S = 0.0;
    int cpt = 0;
    float e;

    for (int i = 1; i <= n; i++) {

        if (stats[i].is_moving || !stats[i].next)
            continue;

        e = stats[i].error;
        S += ((e - mean_error) * (e - mean_error));
        cpt++;
    }
    return sqrt(S / cpt);
}

void features_motion_extraction(ROI_array_t* ROI_array0, const ROI_array_t* ROI_array1, double theta, double tx,
                                double ty) {
    int cc1;
    double x, y, xp, yp;
    float dx, dy;
    float e;

    for (int i = 1; i <= ROI_array0->size; i++) {
        cc1 = ROI_array0->data[i].next; // assos[i];
        if (cc1) {
            // coordonees du point dans l'image I+1
            xp = ROI_array1->data[cc1].x;
            yp = ROI_array1->data[cc1].y;
            // calcul de (x,y) pour l'image I
            x = cos(theta) * (xp - tx) + sin(theta) * (yp - ty);
            y = cos(theta) * (yp - ty) - sin(theta) * (xp - tx);

            // pas besoin de stocker dx et dy (juste pour l'affichage du debug)
            dx = x - ROI_array0->data[i].x;
            dy = y - ROI_array0->data[i].y;
            ROI_array0->data[i].dx = dx;
            ROI_array0->data[i].dy = dy;

            e = sqrt(dx * dx + dy * dy);
            ROI_array0->data[i].error = e;
        }
    }
}

void features_compute_motion(const ROI_array_t* ROI_array1, ROI_array_t* ROI_array0, double* theta, double* tx,
                             double* ty) {
    features_rigid_registration((const ROI_array_t*)ROI_array0, (const ROI_array_t*)ROI_array1, theta, tx, ty);
    features_motion_extraction(ROI_array0, (const ROI_array_t*)ROI_array1, *theta, *tx, *ty);

    double mean_error = features_compute_mean_error((const ROI_array_t*)ROI_array0);
    double std_deviation = features_compute_std_deviation((const ROI_array_t*)ROI_array0, mean_error);

    // saveErrorMoy("first_error.txt", mean_error, std_deviation);

    features_rigid_registration_corrected(ROI_array0, (const ROI_array_t*)ROI_array1, theta, tx, ty, mean_error,
                                          std_deviation);
    features_motion_extraction(ROI_array0, (const ROI_array_t*)ROI_array1, *theta, *tx, *ty);
}

void features_print_stats(ROI_t* stats, int n)
{
    int cpt = 0;
    for (int i = 1; i <= n; i++) {
        if (stats[i].S > 0) {
            cpt++;
        }
    }
    printf("Nombre de CC : %d\n", cpt);

    if (cpt == 0)
        return;

    for (int i = 1; i <= n; i++) {
        if (stats[i].S > 0)
            printf("%4d \t %4d \t %4d \t %4d \t %4d \t %3d \t %4d \t %4d \t %7.1f \t %7.1f \t %4d \t %4d \t %4d \t "
                   "%7.1lf \t %d\n",
                   stats[i].id, stats[i].xmin, stats[i].xmax, stats[i].ymin, stats[i].ymax, stats[i].S, stats[i].Sx,
                   stats[i].Sy, stats[i].x, stats[i].y, stats[i].prev, stats[i].next, stats[i].time, stats[i].error,
                   stats[i].is_moving);
    }
    printf("\n");
}

void features_parse_stats(const char* filename, ROI_t* stats, int* n)
{
    char lines[200];
    int id, xmin, xmax, ymin, ymax, s, sx, sy, prev, next;
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
               &x, &y, &dx, &dy, &error, &prev, &next);
        stats[id].id = id;
        stats[id].xmin = xmin;
        stats[id].xmax = xmax;
        stats[id].ymin = ymin;
        stats[id].ymax = ymax;
        stats[id].S = s;
        stats[id].Sx = sx;
        stats[id].Sy = sy;
        stats[id].x = x;
        stats[id].y = y;
        stats[id].dx = dx;
        stats[id].dy = dy;
        stats[id].error = error;
        stats[id].prev = prev;
        stats[id].next = next;
    }
    fclose(file);
}

int find_corresponding_track(const track_array_t* track_array, const ROI_array_t* ROI_array, const ROI_t *ROI,
                             const unsigned age) {
    assert(age == 0 || age == 1);
    for (size_t t = track_array->offset; t < track_array->size; t++) {
        int cur_ROI_id = age == 0 ? track_array->data[t].end.id : ROI_array->data[track_array->data[t].end.prev].id;
        if (cur_ROI_id <= 0)
            continue;
        if (ROI->id == cur_ROI_id)
            return t;
    }
    return -1;
}

void features_save_stats_file(FILE* f, const ROI_array_t* ROI_array, const track_array_t* track_array, const unsigned age) {
    int cpt = 0;
    for (int i = 1; i <= ROI_array->size; i++)
        if (ROI_array->data[i].S != 0)
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

    for (int i = 1; i <= ROI_array->size; i++) {
        const ROI_t *ROI = &ROI_array->data[i];
        if (ROI->S != 0) {
            int t = find_corresponding_track(track_array, ROI_array, ROI, age);
            char task_id_str[5];
            if (t == -1)
                strcpy(task_id_str, "   -");
            else
                sprintf(task_id_str, "%4d", track_array->data[t].id);
            char task_obj_type[64];
            if (t == -1)
                strcpy(task_obj_type, "      -");
            else
                sprintf(task_obj_type, "%s", g_obj_to_string_with_spaces[track_array->data[t].obj_type]);
            fprintf(f, "   %4d || %s | %s || %4d | %4d | %4d | %4d || %3d | %8d | %8d || %7.1f | %7.1f || %6d | %6d \n",
                    ROI->id, task_id_str, task_obj_type, ROI->xmin, ROI->xmax, ROI->ymin, ROI->ymax, ROI->S, ROI->Sx,
                    ROI->Sy, ROI->x, ROI->y, ROI->time, ROI->time_motion);
        }
    }
}

void features_save_stats(const char* filename, const ROI_array_t* ROI_array, const track_array_t* track_array,
                         const unsigned age) {
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

void features_save_error(const char* filename, ROI_t* stats, int n) {
    double S = 0;
    int cpt = 0;
    FILE* f = fopen(filename, "a");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }

    for (int i = 1; i <= n; i++) {
        if (stats[i].S > 0) {
            S += stats[i].error;
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

void features_save_motion_extraction(const char* filename, const ROI_array_t* ROI_array, const double theta,
                                     const double tx, const double ty, const int frame) {
    // Version DEBUG : il faut implémenter une version pour le main
    FILE* f = fopen(filename, "a");
    if (f == NULL) {
        fprintf(stderr, "motion : cannot open file\n");
        return;
    }

    double mean_error = features_compute_mean_error(ROI_array);
    double std_deviation = features_compute_std_deviation(ROI_array, mean_error);

    for (int i = 1; i <= ROI_array->size; i++) {
        float e = ROI_array->data[i].error;
        // si mouvement detecté
        if (fabs(e - mean_error) > 1.5 * std_deviation) {
            fprintf(f, "%d - %d\n", frame, frame + 1);
            fprintf(f,
                    "CC en mouvement: %2d \t dx:%.3f \t dy: %.3f \t xmin: %3d \t xmax: %3d \t ymin: %3d \t ymax: %3d\n",
                    ROI_array->data[i].id, ROI_array->data[i].dx, ROI_array->data[i].dy, ROI_array->data[i].xmin,
                    ROI_array->data[i].xmax, ROI_array->data[i].ymin, ROI_array->data[i].ymax);
            fprintf(f, "---------------------------------------------------------------\n");
        }
    }
    fclose(f);
}