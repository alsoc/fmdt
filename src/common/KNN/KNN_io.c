#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "fmdt/KNN/KNN_io.h"

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
                               const uint32_t* KNN_data_conflicts, const uint32_t* ROI0_id, const uint32_t* ROI0_next_id,
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
        fprintf(f, "# ------------||---------------||--------------------------------||-----------\n");
        fprintf(f, "#    ROI ID   ||    Distance   ||      Error (or velocity)       ||   Motion  \n");
        fprintf(f, "# ------------||---------------||--------------------------------||-----------\n");
        fprintf(f, "# -----|------||--------|------||----------|----------|----------||-----------\n");
        fprintf(f, "#  t-1 |    t || pixels | rank ||       dx |       dy |        e || is moving \n");
        fprintf(f, "# -----|------||--------|------||----------|----------|----------||-----------\n");
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
            fprintf(f, "  %4u | %4u || %6.3f | %4d || %8.4f | %8.4f | %8.4f || %s \n", ROI0_id[i], ROI0_next_id[i],
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
