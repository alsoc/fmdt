#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "fmdt/macros.h"
#include "vec.h"

#include "fmdt/validation/validation_global.h"
#include "fmdt/validation/validation_compute.h"

#define TOLERANCE_DISTANCEMIN 20 // 8

int validation_init(const char* val_objects_file) {
    assert(val_objects_file != NULL);

    FILE* file = fopen(val_objects_file, "r");
    if (!file) {
        fprintf(stderr, "(EE) Impossible to open '%s'\n", val_objects_file);
        exit(1);
    }

    // count the number of lines
    char tmp_obj_type[1024];
    int16_t tmp_t0;
    float tmp_x0;
    float tmp_y0;
    int16_t tmp_t1;
    float tmp_x1;
    float tmp_y1;
    g_n_val_objects = 0;
    while (!feof(file) && fscanf(file, "%s %hu \t %f \t %f \t %hu \t %f \t %f \n", tmp_obj_type, &tmp_t0, &tmp_x0,
                                 &tmp_y0, &tmp_t1, &tmp_x1, &tmp_y1))
        g_n_val_objects++;
    fseek(file, 0, SEEK_SET);

    if (g_n_val_objects < 1) {
        VERBOSE(fprintf(stderr, "(DBG) [Validation] aucun meteore a suivre dans le fichier input donne !\n"););
        return 0;
    } else {
        VERBOSE(fprintf(stderr, "(DBG) [Validation] %4hu entrees dans le fichier d'input\n",
                        (unsigned short)g_n_val_objects););
    }

    g_val_objects = (validation_obj_t*)malloc(g_n_val_objects * sizeof(validation_obj_t));

    unsigned i = 0;
    while (i < g_n_val_objects && !feof(file)) {
        if (fscanf(file, "%s %hu \t %f \t %f \t %hu \t %f \t %f \n", tmp_obj_type, &g_val_objects[i].t0,
                   &g_val_objects[i].x0, &g_val_objects[i].y0, &g_val_objects[i].t1, &g_val_objects[i].x1,
                   &g_val_objects[i].y1)) {
            g_val_objects[i].t0_min = g_val_objects[i].t0 - 5;
            g_val_objects[i].t1_max = g_val_objects[i].t1 + 5;

            g_val_objects[i].a =
                (float)(g_val_objects[i].y1 - g_val_objects[i].y0) / (float)(g_val_objects[i].x1 - g_val_objects[i].x0);
            g_val_objects[i].b = g_val_objects[i].y1 - g_val_objects[i].a * g_val_objects[i].x1;

            VERBOSE(fprintf(stderr,
                            "(DBG) [Validation] Input %-2d : t0=%-4d x0=%6.1f y0=%6.1f t1=%-4d x1=%6.1f "
                            "y1=%6.1f\tf(x)=%-3.3f*x+%-3.3f\n",
                            i, g_val_objects[i].t0, g_val_objects[i].x0, g_val_objects[i].y0, g_val_objects[i].t1,
                            g_val_objects[i].x1, g_val_objects[i].y1, g_val_objects[i].a, g_val_objects[i].b););

            g_val_objects[i].track = NULL;
            g_val_objects[i].xt = g_val_objects[i].x0;
            g_val_objects[i].yt = g_val_objects[i].y0;

            g_val_objects[i].nb_tracks = 0;
            g_val_objects[i].hits = 0;
            g_val_objects[i].hits = 0; // tmp

            g_val_objects[i].dirX = g_val_objects[i].x1 > g_val_objects[i].x0; // vers la droite
            g_val_objects[i].dirY = g_val_objects[i].y0 < g_val_objects[i].y1; // vers le bas

            if (g_val_objects[i].dirX) {
                if (g_val_objects[i].dirY) {
                    g_val_objects[i].bb_y0 = g_val_objects[i].y0 - TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_x0 = g_val_objects[i].x0 - TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_y1 = g_val_objects[i].y1 + TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_x1 = g_val_objects[i].x1 + TOLERANCE_DISTANCEMIN;
                } else {
                    g_val_objects[i].bb_y0 = g_val_objects[i].y1 - TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_x0 = g_val_objects[i].x0 - TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_y1 = g_val_objects[i].y0 + TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_x1 = g_val_objects[i].x1 + TOLERANCE_DISTANCEMIN;
                }
            } else {
                if (g_val_objects[i].dirY) {
                    g_val_objects[i].bb_y0 = g_val_objects[i].y0 - TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_x0 = g_val_objects[i].x1 - TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_y1 = g_val_objects[i].y1 + TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_x1 = g_val_objects[i].x0 + TOLERANCE_DISTANCEMIN;
                } else {
                    g_val_objects[i].bb_y0 = g_val_objects[i].y1 - TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_x0 = g_val_objects[i].x1 - TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_y1 = g_val_objects[i].y0 + TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_x1 = g_val_objects[i].x0 + TOLERANCE_DISTANCEMIN;
                }
            }

            if (!strcmp(tmp_obj_type, "noise"))
                g_val_objects[i].obj_type = NOISE;
            else if (!strcmp(tmp_obj_type, "meteor"))
                g_val_objects[i].obj_type = METEOR;
            else if (!strcmp(tmp_obj_type, "star"))
                g_val_objects[i].obj_type = STAR;
            else
                g_val_objects[i].obj_type = UNKNOWN;
            i++;
        }
    }
    g_n_val_objects = i; // petit risque de memoire pas utilisee

    fclose(file);

    return g_n_val_objects;
}

void validation_process(const vec_track_t track_array) {
    size_t n_tracks = vector_size(track_array);
    for (size_t t = 0; t < n_tracks; t++) {
        validation_obj_t* val_obj = NULL;
        for (unsigned i = 0; i < g_n_val_objects; i++) {
            if ((size_t)g_val_objects[i].t0_min <= track_array[t].begin.frame &&
                track_array[t].begin.frame + tracking_get_track_time(track_array, t) <=
                (size_t)g_val_objects[i].t1_max &&
                g_val_objects[i].bb_x0 <= track_array[t].begin.x &&
                track_array[t].end.x <= g_val_objects[i].bb_x1 &&
                g_val_objects[i].bb_y0 <= track_array[t].begin.y &&
                track_array[t].end.y <= g_val_objects[i].bb_y1 &&
                track_array[t].obj_type == g_val_objects[i].obj_type) {
#ifdef FMDT_ENABLE_DEBUG
                g_val_objects[i].track_t0 = track_array[t].begin.frame;
                g_val_objects[i].track_t1 = track_array[t].end.frame;
                g_val_objects[i].track_x0 = track_array[t].begin.x;
                g_val_objects[i].track_y0 = track_array[t].begin.y;
                g_val_objects[i].track_x1 = track_array[t].end.x;
                g_val_objects[i].track_y1 = track_array[t].end.y;
#endif
                val_obj = &g_val_objects[i];
                if (g_val_objects[i].nb_tracks == 0)
                    break; // maybe
            }
        }

        // Piste matche avec un input
        if (val_obj) {
            val_obj->nb_tracks++;
            val_obj->hits = tracking_get_track_time(track_array, t) + val_obj->hits + 1;
            g_true_positive[track_array[t].obj_type]++;
            if (track_array[t].obj_type == METEOR)
                g_is_valid_track[t] = 1;
        } else { // Piste ne matche pas avec input
            g_false_positive[track_array[t].obj_type]++;
            if (track_array[t].obj_type == METEOR)
                g_is_valid_track[t] = 2;
        }
    }

    for (unsigned i = 0; i < g_n_val_objects; i++)
        if (!g_val_objects[i].nb_tracks)
            g_false_negative[g_val_objects[i].obj_type]++;

    for (size_t t = 0; t < n_tracks; t++)
        for (int ot = 1; ot < N_OBJECTS; ot++)
            if (ot != track_array[t].obj_type)
                g_true_negative[ot]++;
}

void validation_free(void) {
    if (g_val_objects)
        free(g_val_objects);
}
