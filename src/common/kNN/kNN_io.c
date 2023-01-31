#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "fmdt/kNN/kNN_io.h"

void _kNN_conflicts_write(FILE* f, const uint32_t* kNN_data_conflicts, const float** kNN_data_distances,
                          const uint32_t** kNN_data_nearest, int n_asso, int n_conflicts) {
    // Conflicts
    if (kNN_data_conflicts != NULL) {
        size_t cpt = 0;
        for (int i = 0; i < n_conflicts; i++) {
            if (kNN_data_conflicts[i] > 1)
                cpt++;
        }

        fprintf(f, "#\n");
        if (cpt) {
            fprintf(f, "# Association conflicts [%d]:\n", (int)cpt);
            for (int j = 0; j < n_conflicts; j++) {
                if (kNN_data_conflicts[j] > 1) {
                    fprintf(f, "RoI ID (t) = %d, list of possible RoI IDs (t-1): { ", j + 1);
                    int first = 1;
                    for (int i = 0 ; i < n_asso; i++) {
                        if (kNN_data_nearest[i][j] == 1) {
                            if (!first)
                                fprintf(f, ", ");
                            fprintf(f, "%d [dist = %2.2f]", i + 1, sqrtf(kNN_data_distances[i][j]));
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

void _kNN_asso_conflicts_write(FILE* f, const uint32_t** kNN_data_nearest, const float** kNN_data_distances,
                               const uint32_t* kNN_data_conflicts, const uint32_t* RoI0_id, const uint32_t* RoI0_next_id,
                               const size_t n_RoI0, const float* RoI1_dx, const float* RoI1_dy, const float* RoI1_error,
                               const uint8_t* RoI1_is_moving, const size_t n_RoI1) {
    // Asso
    int cpt = 0;
    for (size_t i = 0; i < n_RoI0; i++) {
        if (RoI0_next_id[i] != 0)
            cpt++;
    }
    fprintf(f, "# Associations [%d]:\n", cpt);

    if (cpt) {
        fprintf(f, "# ------------||---------------||--------------------------------||-----------\n");
        fprintf(f, "#    RoI ID   ||    Distance   ||      Error (or velocity)       ||   Motion  \n");
        fprintf(f, "# ------------||---------------||--------------------------------||-----------\n");
        fprintf(f, "# -----|------||--------|------||----------|----------|----------||-----------\n");
        fprintf(f, "#  t-1 |    t || pixels | rank ||       dx |       dy |        e || is moving \n");
        fprintf(f, "# -----|------||--------|------||----------|----------|----------||-----------\n");
    }

    for (size_t i = 0; i < n_RoI0; i++) {
        if (RoI0_id[i] == 0)
            continue;
        if (RoI0_next_id[i]) {
            char moving_str[32];
            size_t j = (size_t)(RoI0_next_id[i] - 1);
            if (RoI1_is_moving[j])
                snprintf(moving_str, sizeof(moving_str), "      yes");
            else
                snprintf(moving_str, sizeof(moving_str), "       no");
            float dist_ij = sqrtf(kNN_data_distances[i][j]);
            fprintf(f, "  %4u | %4u || %6.3f | %4d || %8.4f | %8.4f | %8.4f || %s \n", RoI0_id[i], RoI0_next_id[i],
                    dist_ij, kNN_data_nearest[i][j], RoI1_dx[j], RoI1_dy[j], RoI1_error[j], moving_str);
        }
    }

    _kNN_conflicts_write(f, kNN_data_conflicts, kNN_data_distances, kNN_data_nearest, n_RoI0, n_RoI1);
}


void kNN_asso_conflicts_write(FILE* f, const kNN_data_t* kNN_data, const RoI_asso_t* RoI_asso_array0,
                              const RoI_asso_t* RoI_asso_array1, const RoI_motion_t* RoI_motion_array1) {
    _kNN_asso_conflicts_write(f, (const uint32_t**)kNN_data->nearest, (const float**)kNN_data->distances,
                              (const uint32_t*)kNN_data->conflicts, RoI_asso_array0->id, RoI_asso_array0->next_id,
                              *RoI_asso_array0->_size, RoI_motion_array1->dx, RoI_motion_array1->dy,
                              RoI_motion_array1->error, RoI_motion_array1->is_moving, *RoI_asso_array1->_size);
}
