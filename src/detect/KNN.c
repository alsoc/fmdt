#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <nrc2.h>

#include "fmdt/tools.h"
#include "fmdt/KNN.h"

KNN_data_t* KNN_alloc_and_init_data(int i0, int i1, int j0, int j1) {
    KNN_data_t* data = (KNN_data_t*)malloc(sizeof(KNN_data_t));
    data->i0 = i0;
    data->i1 = i1;
    data->j0 = j0;
    data->j1 = j1;
    data->nearest = (uint32_t**)ui32matrix(data->i0, data->i1, data->j0, data->j1);
    data->distances = (float**)f32matrix(data->i0, data->i1, data->j0, data->j1);
    zero_ui32matrix(data->nearest, data->i0, data->i1, data->j0, data->j1);
    zero_f32matrix(data->distances, data->i0, data->i1, data->j0, data->j1);
#ifdef ENABLE_DEBUG
    data->conflicts = (uint32_t*)ui32vector(data->j0, data->j1);
    zero_ui32vector(data->conflicts, data->j0, data->j1);
#else
    data->conflicts = NULL;
#endif
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

            // if d > max_dist_square, on peut economiser l'accès mémoire (a implementer)
            distances[i][j] = d;
        }
    }
}

void compute_distance(float** distances, const ROI_t* ROI_array0, const ROI_t* ROI_array1) {
    _compute_distance(ROI_array0->x, ROI_array0->y, ROI_array0->_size, ROI_array1->x, ROI_array1->y, ROI_array1->_size,
                      distances);
}

void _KNN_match1(const float* ROI0_x, const float* ROI0_y, const size_t n_ROI0, const float* ROI1_x,
                 const float* ROI1_y, const size_t n_ROI1, uint32_t** data_nearest, float** distances,
                 uint32_t* data_conflicts, const int k, const uint32_t max_dist_square) {
#ifdef ENABLE_DEBUG
    // vecteur de conflits pour debug
    zero_ui32vector(data_conflicts, 0, n_ROI1);
#endif
    zero_ui32matrix(data_nearest, 0, n_ROI0, 0, n_ROI1);

    // calculs de toutes les distances euclidiennes au carré entre nc0 et nc1
    _compute_distance(ROI0_x, ROI0_y, n_ROI0, ROI1_x, ROI1_y, n_ROI1, distances);

    // les k plus proches voisins dans l'ordre croissant
    for (int rank = 1; rank <= k; rank++) {
        // parcours des distances
        for (size_t i = 0; i < n_ROI0; i++) {
            for (size_t j = 0; j < n_ROI1; j++) {
                // if une distance est calculée et ne fait pas pas déjà parti du tab data_nearest
                if (data_nearest[i][j] == 0 && distances[i][j] < max_dist_square) {
                    int dist_ij = distances[i][j];
                    int cpt = 0;
                    // compte le nombre de distances < dist_ij
                    for (size_t l = 0; l < n_ROI1; l++)
                        if (distances[i][l] < dist_ij)
                            cpt++;

                    // k-ième voisin
                    if (cpt < rank) {
                        data_nearest[i][j] = rank;
#ifdef ENABLE_DEBUG
                        // vecteur de conflits
                        if (rank == 1)
                           data_conflicts[j]++;
#endif
                        break;
                    }
                }
            }
        }
    }
}

void _KNN_match2(const uint32_t** data_nearest, const float** distances, const uint16_t* ROI0_id, int32_t* ROI0_next_id,
                 const size_t n_ROI0, const uint16_t* ROI1_id, int32_t* ROI1_prev_id, const size_t n_ROI1) {
    uint32_t rank = 1;
    for (size_t i = 0; i < n_ROI0; i++) {
    change:
        for (size_t j = 0; j < n_ROI1; j++) {
            // si pas encore associé
            if (!ROI1_prev_id[j]) {
                // si ROI_array1->data[j] est dans les voisins de ROI0
                if (data_nearest[i][j] == rank) {
                    float dist_ij = distances[i][j];
                    // test s'il existe une autre CC de ROI0 de mm rang et plus proche
                    for (size_t l = i + 1; l < n_ROI0; l++) {
                        if (data_nearest[l][j] == rank && distances[l][j] < dist_ij) {
                            rank++;
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
        rank = 1;
    }
}

void _KNN_match(uint32_t** data_nearest, float** data_distances, uint32_t* data_conflicts, const uint16_t* ROI0_id,
                const float* ROI0_x, const float* ROI0_y, int32_t* ROI0_next_id, const size_t n_ROI0,
                const uint16_t* ROI1_id, const float* ROI1_x, const float* ROI1_y, int32_t* ROI1_prev_id, const
                size_t n_ROI1, const int k, const uint32_t max_dist_square) {
    memset(ROI0_next_id, 0, n_ROI0 * sizeof(int32_t));
    memset(ROI1_prev_id, 0, n_ROI1 * sizeof(int32_t));

    _KNN_match1(ROI0_x, ROI0_y, n_ROI0, ROI1_x, ROI1_y, n_ROI1, data_nearest, data_distances, data_conflicts, k,
                max_dist_square);
    _KNN_match2((const uint32_t**)data_nearest, (const float**)data_distances, ROI0_id, ROI0_next_id, n_ROI0, ROI1_id,
                ROI1_prev_id, n_ROI1);
}

void KNN_match(KNN_data_t* data, ROI_t* ROI_array0, ROI_t* ROI_array1, const int k, const uint32_t max_dist_square) {
    _KNN_match(data->nearest, data->distances, data->conflicts, ROI_array0->id, ROI_array0->x, ROI_array0->y,
               ROI_array0->next_id, ROI_array0->_size, ROI_array1->id, ROI_array1->x, ROI_array1->y,
               ROI_array1->prev_id, ROI_array1->_size, k, max_dist_square);
}

void _KNN_conflicts_write(FILE* f, const uint32_t* KNN_data_conflicts, const float** KNN_data_distances,
                          const uint32_t** KNN_data_nearest, int n_asso, int n_conflicts) {
    // Conflicts
    if (KNN_data_conflicts != NULL) {
        size_t cpt = 0;
        for (int i = 0; i < n_conflicts; i++) {
            if (KNN_data_conflicts[i] > 1)
                cpt++;
        }

        fprintf(f, "#\n");
        if (cpt) {
            fprintf(f, "# Association conflicts [%d]:\n", (int)cpt);
            for (int j = 0; j < n_conflicts; j++) {
                if (KNN_data_conflicts[j] > 1) {
                    fprintf(f, "ROI ID (t) = %d, list of possible ROI IDs (t-1): { ", j + 1);
                    int first = 1;
                    for (int i = 0 ; i < n_asso; i++) {
                        if (KNN_data_nearest[i][j] == 1) {
                            if (!first)
                                fprintf(f, ", ");
                            fprintf(f, "%d [dist = %2.2f]", i + 1, sqrtf(KNN_data_distances[i][j]));
                            first = 0;
                        }
                    }
                    fprintf(f, " }\n");
                }
            }
        } else {
            fprintf(f, "# No conflict found\n");
        }
    }
}

void _KNN_asso_conflicts_write(FILE* f, const uint32_t** KNN_data_nearest, const float** KNN_data_distances,
                               const uint32_t* KNN_data_conflicts, const uint16_t* ROI0_id, const int32_t* ROI0_next_id,
                               const size_t n_ROI0, const float* ROI1_dx, const float* ROI1_dy, const float* ROI1_error,
                               const uint8_t* ROI1_is_moving, const size_t n_ROI1) {
    // Asso
    int cpt = 0;
    for (size_t i = 0; i < n_ROI0; i++) {
        if (ROI0_next_id[i] != 0)
            cpt++;
    }
    fprintf(f, "# Associations [%d]:\n", cpt);

    if (cpt) {
        fprintf(f, "# ------------||---------------||------------------------||-----------\n");
        fprintf(f, "#    ROI ID   ||    Distance   ||  Error (or velocity)   ||   Motion  \n");
        fprintf(f, "# ------------||---------------||------------------------||-----------\n");
        fprintf(f, "# -----|------||--------|------||-------|-------|--------||-----------\n");
        fprintf(f, "#  t-1 |    t || pixels | rank ||    dx |    dy |      e || is moving \n");
        fprintf(f, "# -----|------||--------|------||-------|-------|--------||-----------\n");
    }

    for (size_t i = 0; i < n_ROI0; i++) {
        if (ROI0_id[i] == 0)
            continue;
        if (ROI0_next_id[i]) {
            char moving_str[32];
            size_t j = (size_t)(ROI0_next_id[i] - 1);
            if (ROI1_is_moving[j])
                snprintf(moving_str, sizeof(moving_str), "      yes");
            else
                snprintf(moving_str, sizeof(moving_str), "       no");
            float dist_ij = sqrtf(KNN_data_distances[i][j]);
            fprintf(f, "  %4u | %4u || %6.3f | %4d || %5.1f | %5.1f | %6.3f || %s \n", ROI0_id[i], ROI0_next_id[i],
                    dist_ij, KNN_data_nearest[i][j], ROI1_dx[j], ROI1_dy[j], ROI1_error[j], moving_str);
        }
    }

    _KNN_conflicts_write(f, KNN_data_conflicts, KNN_data_distances, KNN_data_nearest, n_ROI0, n_ROI1);
}



void KNN_asso_conflicts_write(FILE* f, const KNN_data_t* KNN_data, const ROI_t* ROI_array0, const ROI_t* ROI_array1) {
    _KNN_asso_conflicts_write(f, (const uint32_t**)KNN_data->nearest, (const float**)KNN_data->distances,
                              (const uint32_t*)KNN_data->conflicts, ROI_array0->id, ROI_array0->next_id,
                              ROI_array0->_size, ROI_array1->dx, ROI_array1->dy, ROI_array1->error,
                              ROI_array1->is_moving, ROI_array1->_size);
}
