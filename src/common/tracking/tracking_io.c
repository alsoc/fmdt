#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vec.h"

#include "fmdt/tracking/tracking_global.h"
#include "fmdt/tracking/tracking_io.h"

void tracking_tracks_write(FILE* f, const vec_track_t tracks) {
    size_t real_n_tracks = 0;
    size_t n_tracks = vector_size(tracks);
    for (size_t i = 0; i < n_tracks; i++)
        if (tracks[i].id)
            real_n_tracks++;

    fprintf(f, "# Tracks [%lu]:\n", (unsigned long)real_n_tracks);
    fprintf(f, "# -------||---------------------------||---------------------------||---------\n");
    fprintf(f, "#  Track ||           Begin           ||            End            ||  Object \n");
    fprintf(f, "# -------||---------------------------||---------------------------||---------\n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------\n");
    fprintf(f, "#     Id || Frame # |      x |      y || Frame # |      x |      y ||    Type \n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------\n");

    for (size_t i = 0; i < n_tracks; i++)
        if (tracks[i].id) {
            fprintf(f, "   %5d || %7u | %6.1f | %6.1f || %7u | %6.1f | %6.1f || %s \n", tracks[i].id,
                    tracks[i].begin.frame, tracks[i].begin.x, tracks[i].begin.y, tracks[i].end.frame, tracks[i].end.x,
                    tracks[i].end.y, g_obj_to_string_with_spaces[tracks[i].obj_type]);
        }
}

void tracking_tracks_write_full(FILE* f, const vec_track_t tracks) {
    size_t real_n_tracks = 0;
    size_t n_tracks = vector_size(tracks);
    for (size_t i = 0; i < n_tracks; i++)
        if (tracks[i].id)
            real_n_tracks++;

    fprintf(f, "# Tracks [%lu]:\n", (unsigned long)real_n_tracks);
    fprintf(f, "# -------||---------------------------||---------------------------||---------||-------------------\n");
    fprintf(f, "#  Track ||           Begin           ||            End            ||  Object || Reason of changed \n");
    fprintf(f, "# -------||---------------------------||---------------------------||---------||    state (from    \n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------||  meteor to noise  \n");
    fprintf(f, "#     Id || Frame # |      x |      y || Frame # |      x |      y ||    Type ||    object only)   \n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------||-------------------\n");

    for (size_t i = 0; i < n_tracks; i++)
        if (tracks[i].id) {
            char reason[64] = "               --";
            if (tracks[i].obj_type == OBJ_NOISE)
                snprintf(reason, sizeof(reason), "%s",
                    g_change_state_to_string_with_spaces[tracks[i].change_state_reason]);
            fprintf(f, "   %5d || %7u | %6.1f | %6.1f || %7u | %6.1f | %6.1f || %s || %s \n", tracks[i].id,
                    tracks[i].begin.frame, tracks[i].begin.x, tracks[i].begin.y, tracks[i].end.frame, tracks[i].end.x,
                    tracks[i].end.y, g_obj_to_string_with_spaces[tracks[i].obj_type], reason);
        }
}

void tracking_tracks_RoIs_id_write(FILE* f, const vec_track_t tracks) {
    size_t n_tracks = vector_size(tracks);
    for (size_t i = 0; i < n_tracks; i++)
        if (tracks[i].id) {
            fprintf(f, " %5d %s ", tracks[i].id, g_obj_to_string_with_spaces[tracks[i].obj_type]);
            if (tracks[i].RoIs_id != NULL) {
                size_t vs = vector_size(tracks[i].RoIs_id);
                for (size_t j = 0; j < vs; j++)
                    fprintf(f, " %5u ", tracks[i].RoIs_id[j]);
                fprintf(f, "\n");
            }
        }
}

void tracking_parse_tracks(const char* filename, vec_track_t* tracks) {
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "(EE) Can't open '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    (*tracks) = (vec_track_t)vector_create();

    int tid, t0, t1;
    float x0, x1, y0, y1;
    char obj_type_str[1024];

    while ((read = getline(&line, &len, fp)) != -1) {
        // printf("Retrieved line of length %zu:\n", read);
        if (line[0] != '#') {
            sscanf(line, "%d || %d | %f | %f || %d | %f | %f || %s ", &tid, &t0, &x0, &y0, &t1, &x1, &y1, obj_type_str);
            track_t* tmp_track = vector_add_asg(tracks);
            tmp_track->id = tid;
            tmp_track->begin.frame = t0;
            tmp_track->end.frame = t1;
            tmp_track->state = STATE_FINISHED;
            tmp_track->begin.x = x0;
            tmp_track->begin.y = y0;
            tmp_track->end.x = x1;
            tmp_track->end.y = y1;
            tmp_track->obj_type = tracking_string_to_obj_type((const char*)obj_type_str);
            tmp_track->RoIs_id = NULL;
            tmp_track = NULL; // stop using temp now that the element is initialized
        }
    }
    fclose(fp);
    if (line)
        free(line);
}