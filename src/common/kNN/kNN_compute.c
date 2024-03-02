#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <nrc2.h>

#include "fmdt/tools.h"

#include "fmdt/kNN/kNN_compute.h"

kNN_data_t* kNN_alloc_data(const size_t max_size) {
    kNN_data_t* kNN_data = (kNN_data_t*)malloc(sizeof(kNN_data_t));
    kNN_data->_max_size = max_size;
    kNN_data->nearest = (uint32_t**)ui32matrix(0, max_size - 1, 0, max_size - 1);
    kNN_data->distances = (float**)f32matrix(0, max_size - 1, 0, max_size - 1);
#ifdef FMDT_ENABLE_DEBUG
    kNN_data->conflicts = (uint32_t*)ui32vector(0, max_size - 1);
#else
    kNN_data->conflicts = NULL;
#endif
    return kNN_data;
}

void kNN_init_data(kNN_data_t* kNN_data) {
    zero_ui32matrix(kNN_data->nearest, 0, kNN_data->_max_size - 1, 0, kNN_data->_max_size - 1);
    zero_f32matrix(kNN_data->distances, 0, kNN_data->_max_size - 1, 0, kNN_data->_max_size - 1);
#ifdef FMDT_ENABLE_DEBUG
    zero_ui32vector(kNN_data->conflicts, 0, kNN_data->_max_size - 1);
#endif
}

void kNN_free_data(kNN_data_t* kNN_data) {
    free_ui32matrix(kNN_data->nearest, 0, kNN_data->_max_size - 1, 0, kNN_data->_max_size - 1);
    free_f32matrix(kNN_data->distances, 0, kNN_data->_max_size - 1, 0, kNN_data->_max_size - 1);
    free_ui32vector(kNN_data->conflicts, 0, kNN_data->_max_size - 1);
    free(kNN_data);
}

void _compute_distance(const RoI_basic_t* RoIs0_basic, const size_t n_RoIs0, const RoI_basic_t* RoIs1_basic,
                       const size_t n_RoIs1, float** distances) {
    // parcours des stats 0
    for (size_t i = 0; i < n_RoIs0; i++) {
        float x0 = RoIs0_basic[i].x;
        float y0 = RoIs0_basic[i].y;

        // parcours des stats 1
        for (size_t j = 0; j < n_RoIs1; j++) {
            float x1 = RoIs1_basic[j].x;
            float y1 = RoIs1_basic[j].y;

            // distances au carré
            float d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);

            // if d > max_dist_square, on peut economiser l'accès mémoire (a implementer)
            distances[i][j] = d;
        }
    }
}

void _kNN_match1(const RoI_basic_t* RoIs0_basic, const size_t n_RoIs0, const RoI_basic_t* RoIs1_basic,
                 const size_t n_RoIs1, float** kNN_data_distances, uint32_t** kNN_data_nearest,
                 uint32_t* kNN_data_conflicts, const int k, const uint32_t max_dist) {
#ifdef FMDT_ENABLE_DEBUG
    // vecteur de conflits pour debug
    zero_ui32vector(kNN_data_conflicts, 0, n_RoIs1 - 1);
#endif
    zero_ui32matrix(kNN_data_nearest, 0, n_RoIs0 - 1, 0, n_RoIs1 - 1);

    // calculs de toutes les distances euclidiennes au carré entre nc0 et nc1
    _compute_distance(RoIs0_basic, n_RoIs0, RoIs1_basic, n_RoIs1, kNN_data_distances);

    float max_dist_square = (float)max_dist * (float)max_dist;

    // les k plus proches voisins dans l'ordre croissant
    for (int rank = 1; rank <= k; rank++) {
        // parcours des distances
        for (size_t i = 0; i < n_RoIs0; i++) {
            for (size_t j = 0; j < n_RoIs1; j++) {
                // if une distance est calculée et ne fait pas pas déjà parti du tab kNN_data_nearest
                if (kNN_data_nearest[i][j] == 0 && kNN_data_distances[i][j] < max_dist_square) {
                    int dist_ij = kNN_data_distances[i][j];
                    int cpt = 0;
                    // compte le nombre de kNN_data_distances < dist_ij
                    for (size_t l = 0; l < n_RoIs1; l++)
                        if (kNN_data_distances[i][l] < dist_ij)
                            cpt++;

                    // k-ième voisin
                    if (cpt < rank) {
                        kNN_data_nearest[i][j] = rank;
#ifdef FMDT_ENABLE_DEBUG
                        // vecteur de conflits
                        if (rank == 1)
                           kNN_data_conflicts[j]++;
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

void _kNN_match2(const float** kNN_data_distances, const uint32_t** kNN_data_nearest, const RoI_basic_t* RoIs0_basic,
                 RoI_asso_t* RoIs0_asso, const size_t n_RoIs0, const RoI_basic_t* RoIs1_basic,
                 RoI_asso_t* RoIs1_asso, const size_t n_RoIs1, const float min_ratio_S) {
    uint32_t rank = 1;
    for (size_t i = 0; i < n_RoIs0; i++) {
    change:
        for (size_t j = 0; j < n_RoIs1; j++) {
            // si pas encore associé
            if (!RoIs1_asso[j].prev_id) {
                // si RoIs1->kNN_data[j] est dans les voisins de RoIs0
                if (kNN_data_nearest[i][j] == rank) {
                    float dist_ij = kNN_data_distances[i][j];
                    // test s'il existe une autre CC de RoIs0 de mm rang et plus proche
                    for (size_t l = i + 1; l < n_RoIs0; l++) {
                        if (kNN_data_nearest[l][j] == rank && kNN_data_distances[l][j] < dist_ij &&
                            _compute_ratio_S(RoIs0_basic[l].S, RoIs1_basic[j].S) >= min_ratio_S) {
                            rank++;
                            goto change;
                        }
                    }

                    if (_compute_ratio_S(RoIs0_basic[i].S, RoIs1_basic[j].S) >= min_ratio_S) {
                        // association
                        RoIs0_asso[i].next_id = RoIs1_basic[j].id;
                        RoIs1_asso[j].prev_id = RoIs0_basic[i].id;
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

uint32_t kNN_match(kNN_data_t* kNN_data, const RoI_basic_t* RoIs0_basic, RoI_asso_t* RoIs0_asso, const size_t n_RoIs0,
                   const RoI_basic_t* RoIs1_basic, RoI_asso_t* RoIs1_asso, const size_t n_RoIs1, const int k,
                   const uint32_t max_dist, const float min_ratio_S) {
    for (size_t r0 = 0; r0 < n_RoIs0; r0++)
        RoIs0_asso[r0].next_id = 0;
    for (size_t r1 = 0; r1 < n_RoIs1; r1++)
        RoIs1_asso[r1].prev_id = 0;

    assert(min_ratio_S >= 0.f && min_ratio_S <= 1.f);

    _kNN_match1(RoIs0_basic, n_RoIs0, RoIs1_basic, n_RoIs1, kNN_data->distances, kNN_data->nearest, kNN_data->conflicts,
                k, max_dist);
    _kNN_match2((const float**)kNN_data->distances, (const uint32_t**)kNN_data->nearest, RoIs0_basic, RoIs0_asso,
                n_RoIs0, RoIs1_basic, RoIs1_asso, n_RoIs1, min_ratio_S);

    // compute the number of associations
    int n_assos = 0;
    for (size_t i = 0; i < n_RoIs0; i++)
        if (RoIs0_asso[i].next_id != 0)
            n_assos++;

    return n_assos;
}
