#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <nrc2.h>

#include "fmdt/tools.h"
#include "fmdt/defines.h"
#include "fmdt/KNN.h"

#define INF32 0xFFFFFFFF

KNN_data_t* KNN_alloc_and_init_data(int i0, int i1, int j0, int j1) {
    KNN_data_t* data = (KNN_data_t*)malloc(sizeof(KNN_data_t));
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

void KNN_free_data(KNN_data_t* data) {
    free_ui32matrix(data->nearest, data->i0, data->i1, data->j0, data->j1);
    free_f32matrix(data->distances, data->i0, data->i1, data->j0, data->j1);
    free_ui32vector(data->conflicts, data->j0, data->j1);
    free(data);
}

void _compute_distance(const float* ROI0_x, const float* ROI0_y, const size_t n_ROI0, const float* ROI1_x,
                       const float* ROI1_y, const size_t n_ROI1, float** distances) {
    // parcours des stats 0
    for (size_t i = 0; i < n_ROI0; i++) {
        float x0 = ROI0_x[i];
        float y0 = ROI0_y[i];

        // parcours des stats 1
        for (size_t j = 0; j < n_ROI1; j++) {
            float x1 = ROI1_x[j];
            float y1 = ROI1_y[j];

            // distances au carré
            float d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);

            // if d > MAX_DIST, on peut economiser l'accès mémoire (a implementer)
            distances[i][j] = d;
        }
    }
}

void compute_distance(float** distances, const ROI_t* ROI_array0, const ROI_t* ROI_array1) {
    _compute_distance(ROI_array0->x, ROI_array0->y, ROI_array0->_size, ROI_array1->x, ROI_array1->y, ROI_array1->_size,
                      distances);
}

void KNN_match1(const float* ROI0_x, const float* ROI0_y, const size_t n_ROI0, const float* ROI1_x,
                const float* ROI1_y, const size_t n_ROI1, uint32_t** data_nearest, float** distances,
                uint32_t* data_conflicts, const int k, const uint32_t max_dist_square) {
    int k_index, val, cpt = 0;

    // vecteur de conflits pour debug
    // zero_ui32vector(data_conflicts, 0, ROI_array1->size);

    zero_ui32matrix(data_nearest, 0, n_ROI0, 0, n_ROI1);

    // calculs de toutes les distances euclidiennes au carré entre nc0 et nc1
    _compute_distance(ROI0_x, ROI0_y, n_ROI0, ROI1_x, ROI1_y, n_ROI1, distances);

    // les k plus proches voisins dans l'ordre croissant
    for (k_index = 1; k_index <= k; k_index++) {
        // parcours des distances
        for (size_t i = 0; i < n_ROI0; i++) {
            for (size_t j = 0; j < n_ROI1; j++) {
                // if une distance est calculée et ne fait pas pas déjà parti du tab data_nearest
                if ((distances[i][j] != INF32) && (data_nearest[i][j] == 0) && (distances[i][j] < max_dist_square)) {
                    val = distances[i][j];
                    cpt = 0;
                    // // compte le nombre de distances < val
                    for (size_t l = 0; l < n_ROI1; l++) {
                        if ((distances[i][l] < val) && (distances[i][l] != INF32)) {
                            cpt++;
                        }
                    }
                    // k_index-ième voisin
                    if (cpt < k_index) {
                        data_nearest[i][j] = k_index;
                        // vecteur de conflits
                        // if (k_index == 1){
                        //         data_conflicts[j]++;
                        // }
                        break;
                    }
                }
            }
        }
    }
}

void KNN_match2(const uint32_t** data_nearest, const float** distances, const uint16_t* ROI0_id, int32_t* ROI0_next_id,
                const size_t n_ROI0, const uint16_t* ROI1_id, int32_t* ROI1_prev_id, const size_t n_ROI1) {
    uint32_t rang = 1;
    for (size_t i = 0; i < n_ROI0; i++) {
    change:
        for (size_t j = 0; j < n_ROI1; j++) {
            // si pas encore associé
            if (!ROI1_prev_id[j]) {
                // si ROI_array1->data[j] est dans les voisins de ROI0
                if (data_nearest[i][j] == rang) {
                    float d = distances[i][j];
                    // test s'il existe une autre CC de ROI0 de mm rang et plus proche
                    for (size_t k = i + 1; k < n_ROI0; k++) {
                        if (data_nearest[k][j] == rang && distances[k][j] < d) {
                            rang++;
                            goto change;
                        }
                    }
                    // association
                    ROI0_next_id[i] = ROI1_id[j];
                    ROI1_prev_id[j] = ROI0_id[i];
                    break;
                }
            }
        }
        rang = 1;
    }
}

void _KNN_match(uint32_t** data_nearest, float** data_distances, uint32_t* data_conflicts, const uint16_t* ROI0_id,
                const float* ROI0_x, const float* ROI0_y, int32_t* ROI0_next_id, const size_t n_ROI0,
                const uint16_t* ROI1_id, const float* ROI1_x, const float* ROI1_y, int32_t* ROI1_prev_id, const
                size_t n_ROI1, const int k, const uint32_t max_dist_square) {
    assert(n_ROI0 < MAX_KNN_SIZE);
    assert(n_ROI1 < MAX_KNN_SIZE);

    memset(ROI0_next_id, 0, n_ROI0 * sizeof(int32_t));
    memset(ROI1_prev_id, 0, n_ROI1 * sizeof(int32_t));

    KNN_match1(ROI0_x, ROI0_y, n_ROI0, ROI1_x, ROI1_y, n_ROI1, data_nearest, data_distances, data_conflicts, k,
               max_dist_square);
    KNN_match2((const uint32_t**)data_nearest, (const float**)data_distances, ROI0_id, ROI0_next_id, n_ROI0, ROI1_id,
               ROI1_prev_id, n_ROI1);
}

void KNN_match(KNN_data_t* data, ROI_t* ROI_array0, ROI_t* ROI_array1, const int k, const uint32_t max_dist_square) {
    _KNN_match(data->nearest, data->distances, data->conflicts, ROI_array0->id, ROI_array0->x, ROI_array0->y,
               ROI_array0->next_id, ROI_array0->_size, ROI_array1->id, ROI_array1->x, ROI_array1->y,
               ROI_array1->prev_id, ROI_array1->_size, k, max_dist_square);
}

void KNN_save_asso(const char* filename, const uint32_t** data_nearest, const float** data_distances,
                   ROI_t* ROI_array) {
    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }

    // tmp (le temps de mettre à jour n)
    int cpt = 0;
    for (size_t i = 0; i < ROI_array->_size; i++) {
        if (ROI_array->S[i] != 0 && ROI_array->next_id[i])
            cpt++;
    }
    if (cpt != 0) {

        fprintf(f, "%d\n", cpt);

        size_t j;
        for (size_t i = 0; i < ROI_array->_size; i++) {
            if (!ROI_array->next_id[i]) {
                if (ROI_array->S[i] > 0)
                    fprintf(f, "%4lu \t ->   pas d'association\n", (unsigned long)i);
            } else {
                j = (size_t)(ROI_array->next_id[i] - 1);
                fprintf(f, "%4lu \t -> %4lu \t  : distance = %10.2f \t ; %4d-voisin\n", (unsigned long)i,
                        (unsigned long)j, data_distances[i][j], data_nearest[i][j]);
            }
        }
    }
    fclose(f);
}

void KNN_save_asso_VT(const char* filename, int nc0, ROI_t* ROI_array, int frame) {
    FILE* f = fopen(filename, "a");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }
    fprintf(f, "%05d_%05d\n", frame, frame + 1);

    size_t j;
    for (size_t i = 0; i <= ROI_array->_size; i++) {
        if (!ROI_array->next_id[i]) {
            if (ROI_array->S[i] > 0)
                fprintf(f, "%4lu \t ->   pas d'association\n", (unsigned long)i);
        } else {
            j = (size_t)(ROI_array->next_id[i] - 1);
            fprintf(f, "%4lu \t -> %4lu \n", (unsigned long)i, (unsigned long)j);
        }
    }
    fprintf(f, "-------------------------------------------------------------------------------------------------------"
               "-----\n");
    fclose(f);
}

void KNN_conflicts_write(FILE* f, uint32_t* data_conflicts, uint32_t** data_nearest, float** data_distances,
                         int n_asso, int n_conflict) {
    // tmp (le temps de mettre à jour n)
    int cpt = 0;
    for (int i = 0; i < n_conflict; i++) {
        if (data_conflicts[i] != 1 && data_conflicts[i] != 0)
            cpt++;
    }
    if (cpt != 0) {

        fprintf(f, "%d\n", cpt);

        for (int j = 0; j < n_conflict; j++) {
            if (data_conflicts[j] != 1 && data_conflicts[j] != 0) {
                fprintf(f, "conflit CC = %4d : ", j);
                for (int i = 0; i < n_asso; i++) {
                    if (data_nearest[i][j] == 1) {
                        fprintf(f, "%4d\t", i);
                    }
                }
                fprintf(f, "\n");
            }
        }
    }
}

void _KNN_asso_conflicts_write(FILE* f, const uint32_t** KNN_data_nearest, const float** KNN_data_distances,
                               const uint16_t* ROI_id, const uint32_t* ROI_S, const float* ROI_dx, const float* ROI_dy,
                               const float* ROI_error, const int32_t* ROI_next_id, const size_t n_ROI) {
    // Asso
    int cpt = 0;
    for (size_t i = 0; i < n_ROI; i++) {
        if (ROI_next_id[i] != 0)
            cpt++;
    }
    fprintf(f, "# Associations [%d]:\n", cpt);
    size_t j;

    if (cpt) {
        fprintf(f, "# ------------||---------------||------------------------\n");
        fprintf(f, "#    ROI ID   ||    Distance   ||          Error         \n");
        fprintf(f, "# ------------||---------------||------------------------\n");
        fprintf(f, "# -----|------||--------|------||-------|-------|--------\n");
        fprintf(f, "#  cur | next || pixels | k-nn ||    dx |    dy |      e \n");
        fprintf(f, "# -----|------||--------|------||-------|-------|--------\n");
    }

    for (size_t i = 0; i < n_ROI; i++) {
        if (ROI_S[i] == 0)
            continue;
        if (ROI_next_id[i]) {
            j = (size_t)(ROI_next_id[i] - 1);
            fprintf(f, "  %4u | %4u || %6.2f | %4d || %5.1f | %5.1f | %6.3f \n", ROI_id[i], ROI_next_id[i],
                    KNN_data_distances[i][j], KNN_data_nearest[i][j], ROI_dx[i], ROI_dy[i], ROI_error[i]);
        }
    }

    // // Conflicts
    // cpt = 0;
    // for(int i = 0; i < n_conflict; i++){
    //     if(KNN_data_conflicts[i] != 1 && KNN_data_conflicts[i] != 0)
    //         cpt++;
    // }

    // fprintf(f, "Conflicts\n%d\n", cpt);

    // for(int j = 1; j <= n_conflict; j++){
    //     if (KNN_data_conflicts[j] != 1 && KNN_data_conflicts[j] != 0){
    //         fprintf(f, "conflit CC = %4d : ", j);
    //         for(int i = 0 ; i < n_ROI; i++){
    //             if (KNN_data_nearest[i][j] == 1 ){
    //                 fprintf(f, "%4d\t", i);
    //             }
    //         }
    //         fprintf(f, "\n");
    //     }
    // }
}

void KNN_asso_conflicts_write(FILE* f, const KNN_data_t* KNN_data, const ROI_t* ROI_array) {
    _KNN_asso_conflicts_write(f, (const uint32_t**)KNN_data->nearest, (const float**)KNN_data->distances,
                              ROI_array->id, ROI_array->S, ROI_array->dx, ROI_array->dy, ROI_array->error,
                              ROI_array->next_id, ROI_array->_size);
}