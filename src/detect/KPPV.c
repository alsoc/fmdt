/*
 * Copyright (c) 2022, Clara Ciocan/ Mathuran Kandeepan
 * LIP6
 */

#include <nrutil.h>
#include <stdio.h>
#include <string.h>

#include "DebugUtil.h"
#include "Features.h"
#include "KPPV.h"
#include "macro_debug.h"

#define INF32 0xFFFFFFFF
#define MAX 100

uint32** g_nearest;
float32** g_distances;
uint32* g_conflicts; // debug

void kppv_init(int i0, int i1, int j0, int j1) {
    g_nearest = ui32matrix(i0, i1, j0, j1);
    g_distances = f32matrix(i0, i1, j0, j1);
    g_conflicts = ui32vector(j0, j1);

    zero_ui32matrix(g_nearest, i0, i1, j0, j1);
    zero_f32matrix(g_distances, i0, i1, j0, j1);
    zero_ui32vector(g_conflicts, j0, j1);
}

void kppv_free(int i0, int i1, int j0, int j1) {
    free_ui32matrix(g_nearest, i0, i1, j0, j1);
    free_f32matrix(g_distances, i0, i1, j0, j1);
    free_ui32vector(g_conflicts, j0, j1);
}
void distance_calc(MeteorROI* stats0, MeteorROI* stats1, int nc0, int nc1) {
    float32 d, x0, x1, y0, y1;

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

                    // if d > MAX, on peut economiser l'accès mémoire (a implementer)
                    g_distances[i][j] = d;
                }
            }
        }
    }
}

void kppv(MeteorROI* stats0, MeteorROI* stats1, int nc0, int nc1, int k) {
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
                if ((g_distances[i][j] != INF32) && (g_nearest[i][j] == 0) && (g_distances[i][j] < MAX)) {
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

void mis_en_correspondance(MeteorROI* stats0, MeteorROI* stats1, int nc0, int nc1) {
    float32 d;
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

void kppv_routine(MeteorROI* stats0, MeteorROI* stats1, int nc0, int nc1, int k) {
    kppv(stats0, stats1, nc0, nc1, k);
    mis_en_correspondance(stats0, stats1, nc0, nc1);
}