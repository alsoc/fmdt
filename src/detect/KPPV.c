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

uint32_t** g_nearest;
float** g_distances;
uint32_t* g_conflicts; // debug

void KPPV_init(int i0, int i1, int j0, int j1) {
    g_nearest = ui32matrix(i0, i1, j0, j1);
    g_distances = f32matrix(i0, i1, j0, j1);
    g_conflicts = ui32vector(j0, j1);

    zero_ui32matrix(g_nearest, i0, i1, j0, j1);
    zero_f32matrix(g_distances, i0, i1, j0, j1);
    zero_ui32vector(g_conflicts, j0, j1);
}

void KPPV_free(int i0, int i1, int j0, int j1) {
    free_ui32matrix(g_nearest, i0, i1, j0, j1);
    free_f32matrix(g_distances, i0, i1, j0, j1);
    free_ui32vector(g_conflicts, j0, j1);
}

void distance_calc(ROI_t* stats0, ROI_t* stats1, int nc0, int nc1) {
    float d, x0, x1, y0, y1;

    // parcours des stats 0
    for (int i = 1; i <= nc0; i++) {
        if (stats0[i].S > 0) {
            x0 = stats0[i].x;
            y0 = stats0[i].y;

            // parcours des stats 1
            for (int j = 1; j <= nc1; j++) {
                if (stats1[j].S > 0) {
                    x1 = stats1[j].x;
                    y1 = stats1[j].y;

                    // distances au carré
                    d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);

                    // if d > MAX_DIST, on peut economiser l'accès mémoire (a implementer)
                    g_distances[i][j] = d;
                }
            }
        }
    }
}

void KPPV_match1(ROI_t* stats0, ROI_t* stats1, int nc0, int nc1, int k) {
    int k_index, val, cpt;
    cpt = 0;

    // vecteur de conflits pour debug
    // zero_ui32vector(g_conflicts, 0, nc1);

    zero_ui32matrix(g_nearest, 0, nc0, 0, nc1);

    // calculs de toutes les distances euclidiennes au carré entre nc0 et nc1
    distance_calc(stats0, stats1, nc0, nc1);

    // les k plus proches voisins dans l'ordre croissant
    for (k_index = 1; k_index <= k; k_index++) {
        // parcours des distances
        for (int i = 1; i <= nc0; i++) {
            for (int j = 1; j <= nc1; j++) {
                // if une distance est calculée et ne fait pas pas déjà parti du tab nearest
                if ((g_distances[i][j] != INF32) && (g_nearest[i][j] == 0) && (g_distances[i][j] < MAX_DIST)) {
                    val = g_distances[i][j];
                    cpt = 0;
                    // // compte le nombre de distances < val
                    for (int l = 1; l <= nc1; l++) {
                        if ((g_distances[i][l] < val) && (g_distances[i][l] != INF32)) {
                            cpt++;
                        }
                    }
                    // k_index-ième voisin
                    if (cpt < k_index) {
                        g_nearest[i][j] = k_index;
                        // vecteur de conflits
                        // if (k_index == 1){
                        //         g_conflicts[j]++;
                        // }
                        break;
                    }
                }
            }
        }
    }
}

void KPPV_match2(ROI_t* stats0, ROI_t* stats1, int nc0, int nc1) {
    float d;
    int rang = 1;

    for (int i = 1; i <= nc0; i++) {
    change:
        for (int j = 1; j <= nc1; j++) {

            // si pas encore associé
            if (!stats1[j].prev) {

                // si stats1[j] est dans les voisins de stats0[i]
                if (g_nearest[i][j] == rang) {
                    d = g_distances[i][j];

                    // test s'il existe une autre CC de stats0 de mm rang et plus proche
                    for (int k = i + 1; k <= nc0; k++) {
                        if (g_nearest[k][j] == rang && g_distances[k][j] < d) {
                            rang++;
                            goto change;
                        }
                    }

                    // association
                    stats0[i].next = j;
                    stats1[j].prev = i;
                    break;
                }
            }
        }
        rang = 1;
    }
}

void KPPV_match(ROI_t* stats0, ROI_t* stats1, int nc0, int nc1, int k) {
    KPPV_match1(stats0, stats1, nc0, nc1, k);
    KPPV_match2(stats0, stats1, nc0, nc1);
}

void KPPV_save_asso(const char* filename, uint32_t** Nearest, float** distances, int nc0, ROI_t* stats) {
    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }

    // tmp (le temps de mettre à jour n)
    int cpt = 0;
    for (int i = 1; i <= nc0; i++) {
        if (stats[i].S != 0 && stats[i].next)
            cpt++;
    }
    if (cpt != 0) {

        fprintf(f, "%d\n", cpt);

        int j;
        for (int i = 1; i <= nc0; i++) {
            j = stats[i].next;
            if (j == 0) {
                if (stats[i].S > 0)
                    fprintf(f, "%4d \t ->   pas d'association\n", i);
            } else {
                fprintf(f, "%4d \t -> %4d \t  : distance = %10.2f \t ; %4d-voisin\n", i, j, distances[i][j],
                        Nearest[i][j]);
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
        j = stats[i].next;
        if (j == 0) {
            if (stats[i].S > 0)
                fprintf(f, "%4d \t ->   pas d'association\n", i);
        } else {
            fprintf(f, "%4d \t -> %4d \n", i, j);
        }
    }
    fprintf(f, "-------------------------------------------------------------------------------------------------------"
               "-----\n");
    fclose(f);
}

void KPPV_save_conflicts(const char* filename, uint32_t* conflicts, uint32_t** Nearest, float** distances, int n_asso,
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
                    if (Nearest[i][j] == 1) {
                        fprintf(f, "%4d\t", i);
                    }
                }
                fprintf(f, "\n");
            }
        }
    }
    fclose(f);
}

void KPPV_save_asso_conflicts(const char* path, int frame, uint32_t* conflicts, uint32_t** Nearest, float** distances,
                              int n_asso, int n_conflict, ROI_t* stats0, ROI_t* stats1, track_t* tracks, int n_tracks) {
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
    features_save_stats_file(f, stats0, n_asso, tracks);
    fprintf(f, "#\n# Frame n°%05d (next)\n", frame + 1);
    features_save_stats_file(f, stats1, n_conflict, tracks);
    fprintf(f, "#\n");

    // Asso
    int cpt = 0;
    for (int i = 1; i <= n_asso; i++) {
        if (stats0[i].next != 0)
            cpt++;
    }
    fprintf(f, "# Associations [%d]:\n", cpt);
    int j;

    if (cpt) {
        double errMoy = features_error_moy(stats0, n_asso);
        double eType = features_ecart_type(stats0, n_asso, errMoy);
        fprintf(f, "# * mean error    = %.3f\n", errMoy);
        fprintf(f, "# * std deviation = %.3f\n", eType);

        fprintf(f, "# ------------||---------------||------------------------\n");
        fprintf(f, "#    ROI ID   ||    Distance   ||          Error         \n");
        fprintf(f, "# ------------||---------------||------------------------\n");
        fprintf(f, "# -----|------||--------|------||-------|-------|--------\n");
        fprintf(f, "#  cur | next || pixels | k-nn ||    dx |    dy |      e \n");
        fprintf(f, "# -----|------||--------|------||-------|-------|--------\n");
    }

    for (int i = 1; i <= n_asso; i++) {
        if (stats0[i].S == 0)
            continue;

        j = stats0[i].next;
        if (j != 0) {
            float dx = stats0[i].dx;
            float dy = stats0[i].dy;
            fprintf(f, "  %4d | %4d || %6.2f | %4d || %5.1f | %5.1f | %6.3f \n", i, j, distances[i][j], Nearest[i][j],
                    dx, dy, stats0[i].error);
        }
    }

    fprintf(f, "#\n");
    fprintf(f, "# tracks [%d]:\n", n_tracks);
    if (n_tracks)
        tracking_print_tracks(f, tracks, n_tracks);

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
    //         for(int i = 1 ; i <= n_asso; i++){
    //             if (Nearest[i][j] == 1 ){
    //                 fprintf(f, "%4d\t", i);
    //             }
    //         }
    //         fprintf(f, "\n");
    //     }
    // }
    fclose(f);
}