#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "vec.h"

#include "fmdt/tracking/tracking_struct.h"
#include "fmdt/tracking/tracking_global.h"
#include "fmdt/features/features_io.h"

void features_print_stats(RoI_t* stats, int n) {
    int cpt = 0;
    for (int i = 0; i < n; i++) {
        if (stats->id[i] > 0) {
            cpt++;
        }
    }
    printf("Nombre de CC : %d\n", cpt);

    if (cpt == 0)
        return;

    for (int i = 0; i < n; i++) {
        if (stats->id[i] > 0)
            printf("%4d \t %4d \t %4d \t %4d \t %4d \t %3d \t %4d \t %4d \t %7.1f \t %7.1f \t %4d \t %4d \t "
                   "%7.1f \t %d\n",
                   stats->id[i], stats->basic->xmin[i], stats->basic->xmax[i], stats->basic->ymin[i],
                   stats->basic->ymax[i], stats->basic->S[i], stats->basic->Sx[i], stats->basic->Sy[i],
                   stats->basic->x[i], stats->basic->y[i], stats->asso->prev_id[i], stats->asso->next_id[i],
                   stats->motion->error[i], stats->motion->is_moving[i]);
    }
    printf("\n");
}

void features_parse_stats(const char* filename, RoI_t* stats, int* n) {
    char lines[200];
    int id, xmin, xmax, ymin, ymax, s, sx, sy, prev_id, next_id;
    float x, y;
    float dx, dy, error;
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "(EE) cannot open file '%s'\n", filename);
        exit(-1);
    }

    // pour l'instant, n représente l'id max des stas mais c'est à changer
    if (fgets(lines, 100, file) == NULL) {
        fprintf(stderr, "(EE) something went wrong when reading '%s'\n", filename);
        exit(-1);
    }
    sscanf(lines, "%d", n);

    while (fgets(lines, 200, file)) {
        sscanf(lines, "%d %d %d %d %d %d %d %d %f %f %f %f %f %d %d", &id, &xmin, &xmax, &ymin, &ymax, &s, &sx, &sy,
               &x, &y, &dx, &dy, &error, &prev_id, &next_id);
        stats->id[id - 1] = id;
        stats->basic->xmin[id - 1] = xmin;
        stats->basic->xmax[id - 1] = xmax;
        stats->basic->ymin[id - 1] = ymin;
        stats->basic->ymax[id - 1] = ymax;
        stats->basic->S[id - 1] = s;
        stats->basic->Sx[id - 1] = sx;
        stats->basic->Sy[id - 1] = sy;
        stats->basic->x[id - 1] = x;
        stats->basic->y[id - 1] = y;
        stats->motion->dx[id - 1] = dx;
        stats->motion->dy[id - 1] = dy;
        stats->motion->error[id - 1] = error;
        stats->asso->prev_id[id - 1] = prev_id;
        stats->asso->next_id[id - 1] = next_id;
    }
    fclose(file);
}

int _find_corresponding_track(const int frame, const vec_track_t track_array, const uint32_t* RoI_id,
                              const int sel_RoI_id, const size_t n_RoI, const unsigned age) {
    assert(age == 0 || age == 1);

    size_t n_tracks = vector_size(track_array);
    for (size_t t = 0; t < n_tracks; t++) {
        if (track_array[t].id) {
            if (track_array[t].end.frame == frame + age) {
                int cur_RoI_id;
                if (age == 0)
                    cur_RoI_id = track_array[t].end.id;
                else {
                    if (track_array[t].end.prev_id == 0)
                        continue;
                    cur_RoI_id = RoI_id[track_array[t].end.prev_id - 1];
                }
                assert(cur_RoI_id <= (int)n_RoI);
                if (cur_RoI_id <= 0)
                    continue;
                if (sel_RoI_id == cur_RoI_id)
                    return t;
            }
        }
    }
    return -1;
}

int find_corresponding_track(const int frame, const vec_track_t track_array, const RoI_t* RoI_array,
                             const int sel_RoI_id, const size_t n_RoI, const unsigned age) {
    return _find_corresponding_track(frame, track_array, RoI_array->id, sel_RoI_id, n_RoI, age);
}

void _features_RoI_write(FILE* f, const int frame, const uint32_t* RoI_id, const uint32_t* RoI_xmin,
                         const uint32_t* RoI_xmax, const uint32_t* RoI_ymin, const uint32_t* RoI_ymax,
                         const uint32_t* RoI_S, const uint32_t* RoI_Sx, const uint32_t* RoI_Sy, const float* RoI_x,
                         const float* RoI_y, const uint32_t* RoI_magnitude, const size_t n_RoI,
                         const vec_track_t track_array, const unsigned age) {
    int cpt = 0;
    for (size_t i = 0; i < n_RoI; i++)
        if (RoI_id[i] != 0)
            cpt++;

    fprintf(f, "Regions of interest (RoI) [%d]: \n", cpt);
    // if (cpt) {  // for compare.py
        fprintf(f, "# ------||----------------||---------------------------||---------------------------||-------------------||-----------\n");
        fprintf(f, "#   RoI ||      Track     ||        Bounding Box       ||   Surface (S in pixels)   ||      Center       || Magnitude \n");
        fprintf(f, "# ------||----------------||---------------------------||---------------------------||-------------------||-----------\n");
        fprintf(f, "# ------||------|---------||------|------|------|------||-----|----------|----------||---------|---------||-----------\n");
        fprintf(f, "#    ID ||   ID |    Type || xmin | xmax | ymin | ymax ||   S |       Sx |       Sy ||       x |       y ||        -- \n");
        fprintf(f, "# ------||------|---------||------|------|------|------||-----|----------|----------||---------|---------||-----------\n");
    // }

    for (size_t i = 0; i < n_RoI; i++) {
        if (RoI_id[i] != 0) {
            int t = _find_corresponding_track(frame, track_array, RoI_id, RoI_id[i], n_RoI, age);
            char task_id_str[16];
            if (t == -1)
                strcpy(task_id_str, "   -");
            else
                snprintf(task_id_str, sizeof(task_id_str), "%4d", track_array[t].id);
            char task_obj_type[64];
            if (t == -1)
                strcpy(task_obj_type, "      -");
            else
                snprintf(task_obj_type, sizeof(task_obj_type), "%s",
                    g_obj_to_string_with_spaces[track_array[t].obj_type]);
            uint32_t mag = 0;
            if (RoI_magnitude != NULL)
                mag = RoI_magnitude[i];
            fprintf(f, "   %4d || %s | %s || %4d | %4d | %4d | %4d || %3d | %8d | %8d || %7.1f | %7.1f || %9d \n",
                    RoI_id[i], task_id_str, task_obj_type, RoI_xmin[i], RoI_xmax[i], RoI_ymin[i], RoI_ymax[i], RoI_S[i],
                    RoI_Sx[i], RoI_Sy[i], RoI_x[i], RoI_y[i], mag);
        }
    }
}

void features_RoI_write(FILE* f, const int frame, const RoI_basic_t* RoI_basic_array, const RoI_misc_t* RoI_misc_array,
                        const vec_track_t track_array, const unsigned age) {
    _features_RoI_write(f, frame, RoI_basic_array->id, RoI_basic_array->xmin, RoI_basic_array->xmax, RoI_basic_array->ymin,
                        RoI_basic_array->ymax, RoI_basic_array->S, RoI_basic_array->Sx, RoI_basic_array->Sy,
                        RoI_basic_array->x, RoI_basic_array->y, RoI_misc_array->magnitude, *RoI_basic_array->_size,
                        track_array, age);
}

void _features_RoI0_RoI1_write(FILE* f, const int prev_frame, const int cur_frame, const uint32_t* RoI0_id,
                               const uint32_t* RoI0_xmin, const uint32_t* RoI0_xmax, const uint32_t* RoI0_ymin,
                               const uint32_t* RoI0_ymax, const uint32_t* RoI0_S, const uint32_t* RoI0_Sx,
                               const uint32_t* RoI0_Sy, const float* RoI0_x, const float* RoI0_y,
                               const uint32_t* RoI0_magnitude, const size_t n_RoI0, const uint32_t* RoI1_id,
                               const uint32_t* RoI1_xmin, const uint32_t* RoI1_xmax, const uint32_t* RoI1_ymin,
                               const uint32_t* RoI1_ymax, const uint32_t* RoI1_S, const uint32_t* RoI1_Sx,
                               const uint32_t* RoI1_Sy, const float* RoI1_x, const float* RoI1_y,
                               const uint32_t* RoI1_magnitude, const size_t n_RoI1, const vec_track_t track_array) {
    if (prev_frame >= 0) {
        fprintf(f, "# Frame n°%05d (t-1) -- ", prev_frame);
        _features_RoI_write(f, prev_frame, RoI0_id, RoI0_xmin, RoI0_xmax, RoI0_ymin, RoI0_ymax, RoI0_S, RoI0_Sx,
                            RoI0_Sy, RoI0_x, RoI0_y, RoI0_magnitude, n_RoI0, track_array, 1);
        fprintf(f, "#\n");
    }

    fprintf(f, "# Frame n°%05d (t) -- ", cur_frame);
    _features_RoI_write(f, cur_frame, RoI1_id, RoI1_xmin, RoI1_xmax, RoI1_ymin, RoI1_ymax, RoI1_S, RoI1_Sx, RoI1_Sy,
                        RoI1_x, RoI1_y, RoI1_magnitude, n_RoI1, track_array, 0);
}

void features_RoI0_RoI1_write(FILE* f, const int prev_frame, const int cur_frame, const RoI_basic_t* RoI_basic_array0,
                              const RoI_misc_t* RoI_misc_array0, const RoI_basic_t* RoI_basic_array1,
                              const RoI_misc_t* RoI_misc_array1, const vec_track_t track_array) {
    _features_RoI0_RoI1_write(f, prev_frame, cur_frame, RoI_basic_array0->id, RoI_basic_array0->xmin,
                              RoI_basic_array0->xmax, RoI_basic_array0->ymin, RoI_basic_array0->ymax,
                              RoI_basic_array0->S, RoI_basic_array0->Sx, RoI_basic_array0->Sy, RoI_basic_array0->x,
                              RoI_basic_array0->y, RoI_misc_array0->magnitude, *RoI_basic_array0->_size,
                              RoI_basic_array1->id, RoI_basic_array1->xmin, RoI_basic_array1->xmax,
                              RoI_basic_array1->ymin, RoI_basic_array1->ymax, RoI_basic_array1->S, RoI_basic_array1->Sx,
                              RoI_basic_array1->Sy, RoI_basic_array1->x, RoI_basic_array1->y,
                              RoI_misc_array1->magnitude, *RoI_basic_array1->_size, track_array);
}
