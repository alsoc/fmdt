/*
 * Copyright (c) 2022, Clara Ciocan/ Mathuran Kandeepan
 * LIP6
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <nrc2.h>

#include "tools.h"
#include "KPPV.h"

#define INF32 0xFFFFFFFF
#define MAX_DIST 100

KKPV_data_t* KPPV_alloc_and_init_data(int i0, int i1, int j0, int j1) {
    KKPV_data_t* data = (KKPV_data_t*)malloc(sizeof(KKPV_data_t));
    data->i0 = i0;
    data->i1 = i1;
    data->j0 = j0;
    data->j1 = j1;
    data->nearest = (uint32_t**)ui32matrix(data->i0, data->i1, data->j0, data->j1);
    data->distances = (float**)f32matrix(data->i0, data->i1, data->j0, data->j1);
    data->conflicts = (uint32_t*)ui32vector(data->j0, data->j1);
    zero_ui32matrix(data->nearest, data->i0, data->i1, data->j0, data->j1);
    zero_f32matrix(data->distances, data->i0, data->i1, data->j0, data->j1);
    zero_ui32vector(data->conflicts, data->j0, data->j1);
    return data;
}

void KPPV_free_data(KKPV_data_t* data) {
    free_ui32matrix(data->nearest, data->i0, data->i1, data->j0, data->j1);
    free_f32matrix(data->distances, data->i0, data->i1, data->j0, data->j1);
    free_ui32vector(data->conflicts, data->j0, data->j1);
    free(data);
}

void compute_distance(float** distances, const ROI_array_t* ROI_array0, const ROI_array_t* ROI_array1) {
    // parcours des stats 0
    for (int i = 0; i < ROI_array0->size; i++) {
        if (ROI_array0->data[i].S > 0) {
            float x0 = ROI_array0->data[i].x;
            float y0 = ROI_array0->data[i].y;

            // parcours des stats 1
            for (int j = 0; j < ROI_array1->size; j++) {
                if (ROI_array1->data[j].S > 0) {
                    float x1 = ROI_array1->data[j].x;
                    float y1 = ROI_array1->data[j].y;

                    // distances au carré
                    float d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);

                    // if d > MAX_DIST, on peut economiser l'accès mémoire (a implementer)
                    distances[i][j] = d;
                }
            }
        }
    }
}

void KPPV_match1(uint32_t** nearest, float** distances, uint32_t* conflicts, const ROI_array_t* ROI_array0,
                 const ROI_array_t* ROI_array1, const int k) {
    int k_index, val, cpt = 0;

    // vecteur de conflits pour debug
    // zero_ui32vector(conflicts, 0, ROI_array1->size);

    zero_ui32matrix(nearest, 0, ROI_array0->size, 0, ROI_array1->size);

    // calculs de toutes les distances euclidiennes au carré entre nc0 et nc1
    compute_distance(distances, ROI_array0, ROI_array1);

    // les k plus proches voisins dans l'ordre croissant
    for (k_index = 1; k_index <= k; k_index++) {
        // parcours des distances
        for (int i = 0; i < ROI_array0->size; i++) {
            for (int j = 0; j < ROI_array1->size; j++) {
                // if une distance est calculée et ne fait pas pas déjà parti du tab nearest
                if ((distances[i][j] != INF32) && (nearest[i][j] == 0) && (distances[i][j] < MAX_DIST)) {
                    val = distances[i][j];
                    cpt = 0;
                    // // compte le nombre de distances < val
                    for (int l = 0; l < ROI_array1->size; l++) {
                        if ((distances[i][l] < val) && (distances[i][l] != INF32)) {
                            cpt++;
                        }
                    }
                    // k_index-ième voisin
                    if (cpt < k_index) {
                        nearest[i][j] = k_index;
                        // vecteur de conflits
                        // if (k_index == 1){
                        //         conflicts[j]++;
                        // }
                        break;
                    }
                }
            }
        }
    }
}

void KPPV_match2(const uint32_t** nearest, const float** distances, ROI_array_t* ROI_array0, ROI_array_t* ROI_array1) {
    int rang = 1;
    for (int i = 0; i < ROI_array0->size; i++) {
    change:
        for (int j = 0; j < ROI_array1->size; j++) {
            // si pas encore associé
            if (!ROI_array1->data[j].prev_id) {
                // si ROI_array1->data[j] est dans les voisins de ROI_array0->data[i]
                if (nearest[i][j] == rang) {
                    float d = distances[i][j];
                    // test s'il existe une autre CC de ROI_array0->data de mm rang et plus proche
                    for (int k = i + 1; k < ROI_array0->size; k++) {
                        if (nearest[k][j] == rang && distances[k][j] < d) {
                            rang++;
                            goto change;
                        }
                    }
                    // association
                    ROI_array0->data[i].next_id = ROI_array1->data[j].id;
                    ROI_array1->data[j].prev_id = ROI_array0->data[i].id;
                    break;
                }
            }
        }
        rang = 1;
    }
}

void KPPV_match(KKPV_data_t* data, ROI_array_t* ROI_array0, ROI_array_t* ROI_array1, const int k) {
    KPPV_match1(data->nearest, data->distances, data->conflicts, (const ROI_array_t*)ROI_array0,
                (const ROI_array_t*)ROI_array1, k);
    KPPV_match2((const uint32_t**)data->nearest, (const float**)data->distances, ROI_array0, ROI_array1);
}

void KPPV_save_asso(const char* filename, const uint32_t** nearest, const float** distances, ROI_array_t* ROI_array) {
    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }

    // tmp (le temps de mettre à jour n)
    int cpt = 0;
    for (int i = 1; i <= ROI_array->size; i++) {
        if (ROI_array->data[i].S != 0 && ROI_array->data[i].next_id)
            cpt++;
    }
    if (cpt != 0) {

        fprintf(f, "%d\n", cpt);

        int j;
        for (int i = 1; i <= ROI_array->size; i++) {
            if (!ROI_array->data[i].next_id) {
                if (ROI_array->data[i].S > 0)
                    fprintf(f, "%4d \t ->   pas d'association\n", i);
            } else {
                j = ROI_array->data[i].next_id - 1;
                fprintf(f, "%4d \t -> %4d \t  : distance = %10.2f \t ; %4d-voisin\n", i, j, distances[i][j],
                        nearest[i][j]);
            }
        }
    }
    fclose(f);
}

void KPPV_save_asso_VT(const char* filename, int nc0, ROI_t* stats, int frame) {
    FILE* f = fopen(filename, "a");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }
    fprintf(f, "%05d_%05d\n", frame, frame + 1);

    int j;
    for (int i = 1; i <= nc0; i++) {
        if (!stats[i].next_id) {
            if (stats[i].S > 0)
                fprintf(f, "%4d \t ->   pas d'association\n", i);
        } else {
            j = stats[i].next_id - 1;
            fprintf(f, "%4d \t -> %4d \n", i, j);
        }
    }
    fprintf(f, "-------------------------------------------------------------------------------------------------------"
               "-----\n");
    fclose(f);
}

void KPPV_save_conflicts(const char* filename, uint32_t* conflicts, uint32_t** nearest, float** distances, int n_asso,
                         int n_conflict) {
    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }

    // tmp (le temps de mettre à jour n)
    int cpt = 0;
    for (int i = 1; i <= n_conflict; i++) {
        if (conflicts[i] != 1 && conflicts[i] != 0)
            cpt++;
    }
    if (cpt != 0) {

        fprintf(f, "%d\n", cpt);

        for (int j = 1; j <= n_conflict; j++) {
            if (conflicts[j] != 1 && conflicts[j] != 0) {
                fprintf(f, "conflit CC = %4d : ", j);
                for (int i = 1; i <= n_asso; i++) {
                    if (nearest[i][j] == 1) {
                        fprintf(f, "%4d\t", i);
                    }
                }
                fprintf(f, "\n");
            }
        }
    }
    fclose(f);
}

void KPPV_save_asso_conflicts(const char* path, const int frame, const KKPV_data_t* data, const ROI_array_t* ROI_array0,
                              const ROI_array_t* ROI_array1, const track_array_t* track_array) {
    assert(frame >= 0);
    char filename[1024];

    sprintf(filename, "%s/%05d_%05d.txt", path, frame, frame + 1);

    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }

    // stats
    fprintf(f, "# Frame n°%05d (cur)\n", frame);
    features_save_stats_file(f, ROI_array0, track_array, 1);
    fprintf(f, "#\n# Frame n°%05d (next)\n", frame + 1);
    features_save_stats_file(f, ROI_array1, track_array, 0);
    fprintf(f, "#\n");

    // Asso
    int cpt = 0;
    for (int i = 0; i < ROI_array0->size; i++) {
        if (ROI_array0->data[i].next_id != 0)
            cpt++;
    }
    fprintf(f, "# Associations [%d]:\n", cpt);
    int j;

    if (cpt) {
        double mean_error = features_compute_mean_error(ROI_array0);
        double std_deviation = features_compute_std_deviation(ROI_array0, mean_error);
        fprintf(f, "# * mean error    = %.3f\n", mean_error);
        fprintf(f, "# * std deviation = %.3f\n", std_deviation);

        fprintf(f, "# ------------||---------------||------------------------\n");
        fprintf(f, "#    ROI ID   ||    Distance   ||          Error         \n");
        fprintf(f, "# ------------||---------------||------------------------\n");
        fprintf(f, "# -----|------||--------|------||-------|-------|--------\n");
        fprintf(f, "#  cur | next || pixels | k-nn ||    dx |    dy |      e \n");
        fprintf(f, "# -----|------||--------|------||-------|-------|--------\n");
    }

    for (int i = 0; i < ROI_array0->size; i++) {
        if (ROI_array0->data[i].S == 0)
            continue;
        if (ROI_array0->data[i].next_id) {
            j = ROI_array0->data[i].next_id - 1;
            float dx = ROI_array0->data[i].dx;
            float dy = ROI_array0->data[i].dy;
            fprintf(f, "  %4d | %4d || %6.2f | %4d || %5.1f | %5.1f | %6.3f \n", i, j, data->distances[i][j],
                    data->nearest[i][j], dx, dy, ROI_array0->data[i].error);
        }
    }

    fprintf(f, "#\n");
    fprintf(f, "# tracks [%lu]:\n", track_array->size);
    if (track_array->size)
        tracking_print_tracks(f, track_array->data, track_array->size);

    // // Conflicts
    // cpt = 0;
    // for(int i = 1; i<= n_conflict; i++){
    //     if(conflicts[i] != 1 && conflicts[i] != 0)
    //         cpt++;
    // }

    // fprintf(f, "Conflicts\n%d\n", cpt);

    // for(int j = 1; j <= n_conflict; j++){
    //     if (conflicts[j] != 1 && conflicts[j] != 0){
    //         fprintf(f, "conflit CC = %4d : ", j);
    //         for(int i = 1 ; i <= ROI_array0->size; i++){
    //             if (nearest[i][j] == 1 ){
    //                 fprintf(f, "%4d\t", i);
    //             }
    //         }
    //         fprintf(f, "\n");
    //     }
    // }
    fclose(f);
}