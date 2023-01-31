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

void _compute_distance(const float* RoIs0_x, const float* RoIs0_y, const size_t n_RoIs0, const float* RoIs1_x,
                       const float* RoIs1_y, const size_t n_RoIs1, float** distances) {
    // parcours des stats 0
    for (size_t i = 0; i < n_RoIs0; i++) {
        float x0 = RoIs0_x[i];
        float y0 = RoIs0_y[i];

        // parcours des stats 1
        for (size_t j = 0; j < n_RoIs1; j++) {
            float x1 = RoIs1_x[j];
            float y1 = RoIs1_y[j];

            // distances au carré
            float d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);

            // if d > max_dist_square, on peut economiser l'accès mémoire (a implementer)
            distances[i][j] = d;
        }
    }
}

void _kNN_match1(const float* RoIs0_x, const float* RoIs0_y, const size_t n_RoIs0, const float* RoIs1_x,
                 const float* RoIs1_y, const size_t n_RoIs1, uint32_t** data_nearest, float** distances,
                 uint32_t* data_conflicts, const int k, const uint32_t max_dist) {
#ifdef FMDT_ENABLE_DEBUG
    // vecteur de conflits pour debug
    zero_ui32vector(data_conflicts, 0, n_RoIs1);
#endif
    zero_ui32matrix(data_nearest, 0, n_RoIs0, 0, n_RoIs1);

    // calculs de toutes les distances euclidiennes au carré entre nc0 et nc1
    _compute_distance(RoIs0_x, RoIs0_y, n_RoIs0, RoIs1_x, RoIs1_y, n_RoIs1, distances);

    float max_dist_square = (float)max_dist * (float)max_dist;

    // les k plus proches voisins dans l'ordre croissant
    for (int rank = 1; rank <= k; rank++) {
        // parcours des distances
        for (size_t i = 0; i < n_RoIs0; i++) {
            for (size_t j = 0; j < n_RoIs1; j++) {
                // if une distance est calculée et ne fait pas pas déjà parti du tab data_nearest
                if (data_nearest[i][j] == 0 && distances[i][j] < max_dist_square) {
                    int dist_ij = distances[i][j];
                    int cpt = 0;
                    // compte le nombre de distances < dist_ij
                    for (size_t l = 0; l < n_RoIs1; l++)
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

void _kNN_match2(const uint32_t** data_nearest, const float** distances, const uint32_t* RoIs0_id,
                 const uint32_t* RoIs0_S, uint32_t* RoIs0_next_id, const size_t n_RoIs0, const uint32_t* RoIs1_id,
                 const uint32_t* RoIs1_S, uint32_t* RoIs1_prev_id, const size_t n_RoIs1, const float min_ratio_S) {
    uint32_t rank = 1;
    for (size_t i = 0; i < n_RoIs0; i++) {
    change:
        for (size_t j = 0; j < n_RoIs1; j++) {
            // si pas encore associé
            if (!RoIs1_prev_id[j]) {
                // si RoIs1->data[j] est dans les voisins de RoIs0
                if (data_nearest[i][j] == rank) {
                    float dist_ij = distances[i][j];
                    // test s'il existe une autre CC de RoIs0 de mm rang et plus proche
                    for (size_t l = i + 1; l < n_RoIs0; l++) {
                        if (data_nearest[l][j] == rank && distances[l][j] < dist_ij &&
                            _compute_ratio_S(RoIs0_S[l], RoIs1_S[j]) >= min_ratio_S) {
                            rank++;
                            goto change;
                        }
                    }

                    if (_compute_ratio_S(RoIs0_S[i], RoIs1_S[j]) >= min_ratio_S) {
                        // association
                        RoIs0_next_id[i] = RoIs1_id[j];
                        RoIs1_prev_id[j] = RoIs0_id[i];
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

void _kNN_match(uint32_t** data_nearest, float** data_distances, uint32_t* data_conflicts, const uint32_t* RoIs0_id,
                const uint32_t* RoIs0_S, const float* RoIs0_x, const float* RoIs0_y, uint32_t* RoIs0_next_id,
                const size_t n_RoIs0, const uint32_t* RoIs1_id, const uint32_t* RoIs1_S, const float* RoIs1_x,
                const float* RoIs1_y, uint32_t* RoIs1_prev_id, const size_t n_RoIs1, const int k,
                const uint32_t max_dist, const float min_ratio_S) {
    memset(RoIs0_next_id, 0, n_RoIs0 * sizeof(int32_t));
    memset(RoIs1_prev_id, 0, n_RoIs1 * sizeof(int32_t));
    assert(min_ratio_S >= 0.f && min_ratio_S <= 1.f);

    _kNN_match1(RoIs0_x, RoIs0_y, n_RoIs0, RoIs1_x, RoIs1_y, n_RoIs1, data_nearest, data_distances, data_conflicts, k,
                max_dist);
    _kNN_match2((const uint32_t**)data_nearest, (const float**)data_distances, RoIs0_id, RoIs0_S, RoIs0_next_id,
                n_RoIs0, RoIs1_id, RoIs1_S, RoIs1_prev_id, n_RoIs1, min_ratio_S);
}

void kNN_match(kNN_data_t* data, const RoIs_basic_t* RoIs0_basic, const RoIs_basic_t* RoIs1_basic,
               RoIs_asso_t* RoIs0_asso, RoIs_asso_t* RoIs1_asso, const int k, const uint32_t max_dist,
               const float min_ratio_S) {
    _kNN_match(data->nearest, data->distances, data->conflicts, RoIs0_basic->id, RoIs0_basic->S, RoIs0_basic->x,
               RoIs0_basic->y, RoIs0_asso->next_id, *RoIs0_basic->_size, RoIs1_basic->id, RoIs1_basic->S,
               RoIs1_basic->x, RoIs1_basic->y, RoIs1_asso->prev_id, *RoIs1_basic->_size, k, max_dist, min_ratio_S);
}
