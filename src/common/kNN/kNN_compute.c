#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <nrc2.h>

#include "fmdt/tools.h"

#include "fmdt/kNN/kNN_compute.h"

kNN_data_t* kNN_alloc_and_init_data(const size_t max_size) {
    kNN_data_t* data = (kNN_data_t*)malloc(sizeof(kNN_data_t));
    data->max_size = max_size;
    data->nearest = (uint32_t**)ui32matrix(0, max_size - 1, 0, max_size - 1);
    data->distances = (float**)f32matrix(0, max_size - 1, 0, max_size - 1);
    zero_ui32matrix(data->nearest, 0, max_size - 1, 0, max_size - 1);
    zero_f32matrix(data->distances, 0, max_size - 1, 0, max_size - 1);
#ifdef FMDT_ENABLE_DEBUG
    data->conflicts = (uint32_t*)ui32vector(0, max_size - 1);
    zero_ui32vector(data->conflicts, 0, max_size - 1);
#else
    data->conflicts = NULL;
#endif
    return data;
}

void kNN_free_data(kNN_data_t* data) {
    free_ui32matrix(data->nearest, 0, data->max_size - 1, 0, data->max_size - 1);
    free_f32matrix(data->distances, 0, data->max_size - 1, 0, data->max_size - 1);
    free_ui32vector(data->conflicts, 0, data->max_size - 1);
    free(data);
}

void _compute_distance(const float* RoI0_x, const float* RoI0_y, const size_t n_RoI0, const float* RoI1_x,
                       const float* RoI1_y, const size_t n_RoI1, float** distances) {
    // parcours des stats 0
    for (size_t i = 0; i < n_RoI0; i++) {
        float x0 = RoI0_x[i];
        float y0 = RoI0_y[i];

        // parcours des stats 1
        for (size_t j = 0; j < n_RoI1; j++) {
            float x1 = RoI1_x[j];
            float y1 = RoI1_y[j];

            // distances au carré
            float d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);

            // if d > max_dist_square, on peut economiser l'accès mémoire (a implementer)
            distances[i][j] = d;
        }
    }
}

void _kNN_match1(const float* RoI0_x, const float* RoI0_y, const size_t n_RoI0, const float* RoI1_x,
                 const float* RoI1_y, const size_t n_RoI1, uint32_t** data_nearest, float** distances,
                 uint32_t* data_conflicts, const int k, const uint32_t max_dist) {
#ifdef FMDT_ENABLE_DEBUG
    // vecteur de conflits pour debug
    zero_ui32vector(data_conflicts, 0, n_RoI1);
#endif
    zero_ui32matrix(data_nearest, 0, n_RoI0, 0, n_RoI1);

    // calculs de toutes les distances euclidiennes au carré entre nc0 et nc1
    _compute_distance(RoI0_x, RoI0_y, n_RoI0, RoI1_x, RoI1_y, n_RoI1, distances);

    float max_dist_square = (float)max_dist * (float)max_dist;

    // les k plus proches voisins dans l'ordre croissant
    for (int rank = 1; rank <= k; rank++) {
        // parcours des distances
        for (size_t i = 0; i < n_RoI0; i++) {
            for (size_t j = 0; j < n_RoI1; j++) {
                // if une distance est calculée et ne fait pas pas déjà parti du tab data_nearest
                if (data_nearest[i][j] == 0 && distances[i][j] < max_dist_square) {
                    int dist_ij = distances[i][j];
                    int cpt = 0;
                    // compte le nombre de distances < dist_ij
                    for (size_t l = 0; l < n_RoI1; l++)
                        if (distances[i][l] < dist_ij)
                            cpt++;

                    // k-ième voisin
                    if (cpt < rank) {
                        data_nearest[i][j] = rank;
#ifdef FMDT_ENABLE_DEBUG
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

float _compute_ratio_S(const uint32_t S0, const uint32_t S1) {
    return S0 < S1 ? (float)S0 / (float)S1 : (float)S1 / (float)S0;
}

void _kNN_match2(const uint32_t** data_nearest, const float** distances, const uint32_t* RoI0_id,
                 const uint32_t* RoI0_S, uint32_t* RoI0_next_id, const size_t n_RoI0, const uint32_t* RoI1_id,
                 const uint32_t* RoI1_S, uint32_t* RoI1_prev_id, const size_t n_RoI1, const float min_ratio_S) {
    uint32_t rank = 1;
    for (size_t i = 0; i < n_RoI0; i++) {
    change:
        for (size_t j = 0; j < n_RoI1; j++) {
            // si pas encore associé
            if (!RoI1_prev_id[j]) {
                // si RoI_array1->data[j] est dans les voisins de RoI0
                if (data_nearest[i][j] == rank) {
                    float dist_ij = distances[i][j];
                    // test s'il existe une autre CC de RoI0 de mm rang et plus proche
                    for (size_t l = i + 1; l < n_RoI0; l++) {
                        if (data_nearest[l][j] == rank && distances[l][j] < dist_ij &&
                            _compute_ratio_S(RoI0_S[l], RoI1_S[j]) >= min_ratio_S) {
                            rank++;
                            goto change;
                        }
                    }

                    if (_compute_ratio_S(RoI0_S[i], RoI1_S[j]) >= min_ratio_S) {
                        // association
                        RoI0_next_id[i] = RoI1_id[j];
                        RoI1_prev_id[j] = RoI0_id[i];
                        break;
                    } else {
                        rank++;
                        goto change;
                    }
                }
            }
        }
        rank = 1;
    }
}

void _kNN_match(uint32_t** data_nearest, float** data_distances, uint32_t* data_conflicts, const uint32_t* RoI0_id,
                const uint32_t* RoI0_S, const float* RoI0_x, const float* RoI0_y, uint32_t* RoI0_next_id,
                const size_t n_RoI0, const uint32_t* RoI1_id, const uint32_t* RoI1_S, const float* RoI1_x,
                const float* RoI1_y, uint32_t* RoI1_prev_id, const size_t n_RoI1, const int k,
                const uint32_t max_dist, const float min_ratio_S) {
    memset(RoI0_next_id, 0, n_RoI0 * sizeof(int32_t));
    memset(RoI1_prev_id, 0, n_RoI1 * sizeof(int32_t));
    assert(min_ratio_S >= 0.f && min_ratio_S <= 1.f);

    _kNN_match1(RoI0_x, RoI0_y, n_RoI0, RoI1_x, RoI1_y, n_RoI1, data_nearest, data_distances, data_conflicts, k,
                max_dist);
    _kNN_match2((const uint32_t**)data_nearest, (const float**)data_distances, RoI0_id, RoI0_S, RoI0_next_id, n_RoI0,
                RoI1_id, RoI1_S, RoI1_prev_id, n_RoI1, min_ratio_S);
}

void kNN_match(kNN_data_t* data, const RoI_basic_t* RoI_basic_array0, const RoI_basic_t* RoI_basic_array1,
               RoI_asso_t* RoI_asso_array0, RoI_asso_t* RoI_asso_array1, const int k, const uint32_t max_dist,
               const float min_ratio_S) {
    _kNN_match(data->nearest, data->distances, data->conflicts, RoI_basic_array0->id, RoI_basic_array0->S,
               RoI_basic_array0->x, RoI_basic_array0->y, RoI_asso_array0->next_id, *RoI_basic_array0->_size,
               RoI_basic_array1->id, RoI_basic_array1->S, RoI_basic_array1->x, RoI_basic_array1->y,
               RoI_asso_array1->prev_id, *RoI_basic_array1->_size, k, max_dist, min_ratio_S);
}
