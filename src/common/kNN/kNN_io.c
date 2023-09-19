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

void _kNN_asso_conflicts_write(FILE* f, const float** kNN_data_distances, const uint32_t** kNN_data_nearest,
                               const uint32_t* kNN_data_conflicts, const uint32_t* RoIs0_id,
                               const uint32_t* RoIs0_next_id, const size_t n_RoIs0, const float* RoIs1_dx,
                               const float* RoIs1_dy, const float* RoIs1_error, const uint8_t* RoIs1_is_moving,
                               const size_t n_RoIs1) {
    // Asso
    int cpt = 0;
    for (size_t i = 0; i < n_RoIs0; i++) {
        if (RoIs0_next_id[i] != 0)
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

    for (size_t i = 0; i < n_RoIs0; i++) {
        if (RoIs0_id[i] == 0)
            continue;
        if (RoIs0_next_id[i]) {
            char moving_str[32];
            size_t j = (size_t)(RoIs0_next_id[i] - 1);
            if (RoIs1_is_moving) {
                if (RoIs1_is_moving[j])
                    snprintf(moving_str, sizeof(moving_str), "      yes");
                else
                    snprintf(moving_str, sizeof(moving_str), "       no");
            } else {
                snprintf(moving_str, sizeof(moving_str), "        -");
            }
            float dist_ij = sqrtf(kNN_data_distances[i][j]);

            char dx_str[16];
            if (RoIs1_dx != NULL)
                snprintf(dx_str, sizeof(dx_str), "%8.4f", RoIs1_dx[j]);
            else
                strcpy(dx_str, "       -");

            char dy_str[16];
            if (RoIs1_dy != NULL)
                snprintf(dy_str, sizeof(dy_str), "%8.4f", RoIs1_dy[j]);
            else
                strcpy(dy_str, "       -");

            char error_str[16];
            if (RoIs1_error != NULL)
                snprintf(error_str, sizeof(error_str), "%8.4f", RoIs1_error[j]);
            else
                strcpy(error_str, "       -");

            fprintf(f, "  %4u | %4u || %6.3f | %4d || %s | %s | %s || %s \n", RoIs0_id[i], RoIs0_next_id[i],
                    dist_ij, kNN_data_nearest[i][j], dx_str, dy_str, error_str, moving_str);
        }
    }

    _kNN_conflicts_write(f, kNN_data_conflicts, kNN_data_distances, kNN_data_nearest, n_RoIs0, n_RoIs1);
}


void kNN_asso_conflicts_write(FILE* f, const kNN_data_t* kNN_data, const RoIs_asso_t* RoIs0_asso,
                              const RoIs_asso_t* RoIs1_asso, const RoIs_motion_t* RoIs1_motion) {
    _kNN_asso_conflicts_write(f, (const float**)kNN_data->distances, (const uint32_t**)kNN_data->nearest,
                              (const uint32_t*)kNN_data->conflicts, RoIs0_asso->id, RoIs0_asso->next_id,
                              *RoIs0_asso->_size, RoIs1_motion ? RoIs1_motion->dx : NULL,
                              RoIs1_motion ? RoIs1_motion->dy : NULL, RoIs1_motion ? RoIs1_motion->error : NULL,
                              RoIs1_motion ? RoIs1_motion->is_moving : NULL, *RoIs1_asso->_size);
}
