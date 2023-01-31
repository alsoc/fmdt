#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "vec.h"

#include "fmdt/tracking/tracking_struct.h"
#include "fmdt/tracking/tracking_global.h"
#include "fmdt/features/features_io.h"

void features_print_stats(RoIs_t* stats, int n) {
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

void features_parse_stats(const char* filename, RoIs_t* stats, int* n) {
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

int _find_corresponding_track(const int frame, const vec_track_t track_array, const uint32_t* RoIs_id,
                              const int sel_RoIs_id, const size_t n_RoIs, const unsigned age) {
    assert(age == 0 || age == 1);

    size_t n_tracks = vector_size(track_array);
    for (size_t t = 0; t < n_tracks; t++) {
        if (track_array[t].id) {
            if (track_array[t].end.frame == frame + age) {
                int cur_RoIs_id;
                if (age == 0)
                    cur_RoIs_id = track_array[t].end.id;
                else {
                    if (track_array[t].end.prev_id == 0)
                        continue;
                    cur_RoIs_id = RoIs_id[track_array[t].end.prev_id - 1];
                }
                assert(cur_RoIs_id <= (int)n_RoIs);
                if (cur_RoIs_id <= 0)
                    continue;
                if (sel_RoIs_id == cur_RoIs_id)
                    return t;
            }
        }
    }
    return -1;
}

int find_corresponding_track(const int frame, const vec_track_t track_array, const RoIs_t* RoIs_array,
                             const int sel_RoIs_id, const size_t n_RoIs, const unsigned age) {
    return _find_corresponding_track(frame, track_array, RoIs_array->id, sel_RoIs_id, n_RoIs, age);
}

void _features_RoIs_write(FILE* f, const int frame, const uint32_t* RoIs_id, const uint32_t* RoIs_xmin,
                         const uint32_t* RoIs_xmax, const uint32_t* RoIs_ymin, const uint32_t* RoIs_ymax,
                         const uint32_t* RoIs_S, const uint32_t* RoIs_Sx, const uint32_t* RoIs_Sy, const float* RoIs_x,
                         const float* RoIs_y, const uint32_t* RoIs_magnitude, const size_t n_RoIs,
                         const vec_track_t track_array, const unsigned age) {
    int cpt = 0;
    for (size_t i = 0; i < n_RoIs; i++)
        if (RoIs_id[i] != 0)
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

    for (size_t i = 0; i < n_RoIs; i++) {
        if (RoIs_id[i] != 0) {
            int t = _find_corresponding_track(frame, track_array, RoIs_id, RoIs_id[i], n_RoIs, age);
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
            if (RoIs_magnitude != NULL)
                mag = RoIs_magnitude[i];
            fprintf(f, "   %4d || %s | %s || %4d | %4d | %4d | %4d || %3d | %8d | %8d || %7.1f | %7.1f || %9d \n",
                    RoIs_id[i], task_id_str, task_obj_type, RoIs_xmin[i], RoIs_xmax[i], RoIs_ymin[i], RoIs_ymax[i],
                    RoIs_S[i], RoIs_Sx[i], RoIs_Sy[i], RoIs_x[i], RoIs_y[i], mag);
        }
    }
}

void features_RoIs_write(FILE* f, const int frame, const RoIs_basic_t* RoIs_basic_array,
                        const RoIs_misc_t* RoIs_misc_array, const vec_track_t track_array, const unsigned age) {
    _features_RoIs_write(f, frame, RoIs_basic_array->id, RoIs_basic_array->xmin, RoIs_basic_array->xmax,
                         RoIs_basic_array->ymin, RoIs_basic_array->ymax, RoIs_basic_array->S, RoIs_basic_array->Sx,
                         RoIs_basic_array->Sy, RoIs_basic_array->x, RoIs_basic_array->y, RoIs_misc_array->magnitude,
                         *RoIs_basic_array->_size, track_array, age);
}

void _features_RoIs0_RoIs1_write(FILE* f, const int prev_frame, const int cur_frame, const uint32_t* RoIs0_id,
                                 const uint32_t* RoIs0_xmin, const uint32_t* RoIs0_xmax, const uint32_t* RoIs0_ymin,
                                 const uint32_t* RoIs0_ymax, const uint32_t* RoIs0_S, const uint32_t* RoIs0_Sx,
                                 const uint32_t* RoIs0_Sy, const float* RoIs0_x, const float* RoIs0_y,
                                 const uint32_t* RoIs0_magnitude, const size_t n_RoIs0, const uint32_t* RoIs1_id,
                                 const uint32_t* RoIs1_xmin, const uint32_t* RoIs1_xmax, const uint32_t* RoIs1_ymin,
                                 const uint32_t* RoIs1_ymax, const uint32_t* RoIs1_S, const uint32_t* RoIs1_Sx,
                                 const uint32_t* RoIs1_Sy, const float* RoIs1_x, const float* RoIs1_y,
                                 const uint32_t* RoIs1_magnitude, const size_t n_RoIs1, const vec_track_t track_array) {
    if (prev_frame >= 0) {
        fprintf(f, "# Frame n°%05d (t-1) -- ", prev_frame);
        _features_RoIs_write(f, prev_frame, RoIs0_id, RoIs0_xmin, RoIs0_xmax, RoIs0_ymin, RoIs0_ymax, RoIs0_S, RoIs0_Sx,
                             RoIs0_Sy, RoIs0_x, RoIs0_y, RoIs0_magnitude, n_RoIs0, track_array, 1);
        fprintf(f, "#\n");
    }

    fprintf(f, "# Frame n°%05d (t) -- ", cur_frame);
    _features_RoIs_write(f, cur_frame, RoIs1_id, RoIs1_xmin, RoIs1_xmax, RoIs1_ymin, RoIs1_ymax, RoIs1_S, RoIs1_Sx,
                         RoIs1_Sy, RoIs1_x, RoIs1_y, RoIs1_magnitude, n_RoIs1, track_array, 0);
}

void features_RoIs0_RoIs1_write(FILE* f, const int prev_frame, const int cur_frame, const RoIs_basic_t* RoIs_basic_array0,
                              const RoIs_misc_t* RoIs_misc_array0, const RoIs_basic_t* RoIs_basic_array1,
                              const RoIs_misc_t* RoIs_misc_array1, const vec_track_t track_array) {
    _features_RoIs0_RoIs1_write(f, prev_frame, cur_frame, RoIs_basic_array0->id, RoIs_basic_array0->xmin,
                                RoIs_basic_array0->xmax, RoIs_basic_array0->ymin, RoIs_basic_array0->ymax,
                                RoIs_basic_array0->S, RoIs_basic_array0->Sx, RoIs_basic_array0->Sy,
                                RoIs_basic_array0->x, RoIs_basic_array0->y, RoIs_misc_array0->magnitude,
                                *RoIs_basic_array0->_size, RoIs_basic_array1->id, RoIs_basic_array1->xmin,
                                RoIs_basic_array1->xmax, RoIs_basic_array1->ymin, RoIs_basic_array1->ymax,
                                RoIs_basic_array1->S, RoIs_basic_array1->Sx, RoIs_basic_array1->Sy,
                                RoIs_basic_array1->x, RoIs_basic_array1->y, RoIs_misc_array1->magnitude,
                                *RoIs_basic_array1->_size, track_array);
}
