#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vec.h"

#include "fmdt/tracking/tracking_global.h"
#include "fmdt/tracking/tracking_io.h"

void tracking_track_array_write(FILE* f, const vec_track_t track_array) {
    size_t real_n_tracks = 0;
    size_t n_tracks = vector_size(track_array);
    for (size_t i = 0; i < n_tracks; i++)
        if (track_array[i].id)
            real_n_tracks++;

    fprintf(f, "# Tracks [%lu]:\n", (unsigned long)real_n_tracks);
    fprintf(f, "# -------||---------------------------||---------------------------||---------\n");
    fprintf(f, "#  Track ||           Begin           ||            End            ||  Object \n");
    fprintf(f, "# -------||---------------------------||---------------------------||---------\n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------\n");
    fprintf(f, "#     Id || Frame # |      x |      y || Frame # |      x |      y ||    Type \n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------\n");

    for (size_t i = 0; i < n_tracks; i++)
        if (track_array[i].id) {
            fprintf(f, "   %5d || %7u | %6.1f | %6.1f || %7u | %6.1f | %6.1f || %s \n", track_array[i].id,
                    track_array[i].begin.frame, track_array[i].begin.x, track_array[i].begin.y,
                    track_array[i].end.frame, track_array[i].end.x, track_array[i].end.y,
                    g_obj_to_string_with_spaces[track_array[i].obj_type]);
        }
}

void tracking_track_array_write_full(FILE* f, const vec_track_t track_array) {
    size_t real_n_tracks = 0;
    size_t n_tracks = vector_size(track_array);
    for (size_t i = 0; i < n_tracks; i++)
        if (track_array[i].id)
            real_n_tracks++;

    fprintf(f, "# Tracks [%lu]:\n", (unsigned long)real_n_tracks);
    fprintf(f, "# -------||---------------------------||---------------------------||---------||-------------------\n");
    fprintf(f, "#  Track ||           Begin           ||            End            ||  Object || Reason of changed \n");
    fprintf(f, "# -------||---------------------------||---------------------------||---------||    state (from    \n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------||  meteor to noise  \n");
    fprintf(f, "#     Id || Frame # |      x |      y || Frame # |      x |      y ||    Type ||    object only)   \n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------||-------------------\n");

    for (size_t i = 0; i < n_tracks; i++)
        if (track_array[i].id) {
            char reason[64] = "               --";
            if (track_array[i].obj_type == NOISE)
                snprintf(reason, sizeof(reason), "%s",
                    g_change_state_to_string_with_spaces[track_array[i].change_state_reason]);
            fprintf(f, "   %5d || %7u | %6.1f | %6.1f || %7u | %6.1f | %6.1f || %s || %s \n", track_array[i].id,
                    track_array[i].begin.frame, track_array[i].begin.x, track_array[i].begin.y,
                    track_array[i].end.frame, track_array[i].end.x, track_array[i].end.y,
                    g_obj_to_string_with_spaces[track_array[i].obj_type], reason);
        }
}

void tracking_BB_array_write(FILE* file, const vec_BB_t* BB_array, const vec_track_t track_array) {
    assert(BB_array != NULL);

    vec_BB_t* BB_array_hack = (vec_BB_t*)BB_array;
    size_t vs1 = vector_size(BB_array_hack);
    for (size_t f = 0; f < vs1; f++) {
        size_t vs2 = vector_size(BB_array[f]);
        for (size_t t = 0; t < vs2; t++) {
            if (track_array[BB_array[f][t].track_id - 1].id) {
                fprintf(file, "%d %d %d %d %d %d %d \n", BB_array[f][t].frame_id, BB_array[f][t].rx, BB_array[f][t].ry,
                        BB_array[f][t].bb_x, BB_array[f][t].bb_y, BB_array[f][t].track_id,
                        BB_array[f][t].is_extrapolated);
            }
        }
    }
}

void tracking_track_array_magnitude_write(FILE* f, const vec_track_t track_array) {
    size_t n_tracks = vector_size(track_array);
    for (size_t i = 0; i < n_tracks; i++)
        if (track_array[i].id) {
            fprintf(f, " %5d %s ", track_array[i].id, g_obj_to_string_with_spaces[track_array[i].obj_type]);
            if (track_array[i].magnitude != NULL) {
                size_t vs = vector_size(track_array[i].magnitude);
                for (size_t j = 0; j < vs; j++)
                    fprintf(f, " %5u ", track_array[i].magnitude[j]);
                fprintf(f, "\n");
            }
        }
}

void tracking_parse_tracks(const char* filename, vec_track_t* track_array) {
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "(EE) Can't open '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    (*track_array) = (vec_track_t)vector_create();

    int tid, t0, t1;
    float x0, x1, y0, y1;
    char obj_type_str[1024];

    while ((read = getline(&line, &len, fp)) != -1) {
        // printf("Retrieved line of length %zu:\n", read);
        if (line[0] != '#') {
            sscanf(line, "%d || %d | %f | %f || %d | %f | %f || %s ", &tid, &t0, &x0, &y0, &t1, &x1, &y1, obj_type_str);
            track_t* tmp_track = vector_add_asg(track_array);
            tmp_track->id = tid;
            tmp_track->begin.frame = t0;
            tmp_track->end.frame = t1;
            tmp_track->state = TRACK_FINISHED;
            tmp_track->begin.x = x0;
            tmp_track->begin.y = y0;
            tmp_track->end.x = x1;
            tmp_track->end.y = y1;
            tmp_track->obj_type = tracking_string_to_obj_type((const char*)obj_type_str);
            tmp_track->magnitude = NULL;
            tmp_track = NULL; // stop using temp now that the element is initialized
        }
    }
    fclose(fp);
    if (line)
        free(line);
}