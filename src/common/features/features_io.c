#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "vec.h"

#include "fmdt/tracking/tracking_struct.h"
#include "fmdt/tracking/tracking_global.h"
#include "fmdt/features/features_io.h"

void features_print_stats(ROI_t* stats, int n) {
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

void features_parse_stats(const char* filename, ROI_t* stats, int* n) {
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

int _find_corresponding_track(const int frame, const vec_track_t track_array, const uint32_t* ROI_id,
                              const int sel_ROI_id, const size_t n_ROI, const unsigned age) {
    assert(age == 0 || age == 1);

    size_t n_tracks = vector_size(track_array);
    for (size_t t = 0; t < n_tracks; t++) {
        if (track_array[t].id) {
            if (track_array[t].end.frame == frame + age) {
                int cur_ROI_id;
                if (age == 0)
                    cur_ROI_id = track_array[t].end.id;
                else {
                    if (track_array[t].end.prev_id == 0)
                        continue;
                    cur_ROI_id = ROI_id[track_array[t].end.prev_id - 1];
                }
                assert(cur_ROI_id <= (int)n_ROI);
                if (cur_ROI_id <= 0)
                    continue;
                if (sel_ROI_id == cur_ROI_id)
                    return t;
            }
        }
    }
    return -1;
}

int find_corresponding_track(const int frame, const vec_track_t track_array, const ROI_t* ROI_array,
                             const int sel_ROI_id, const size_t n_ROI, const unsigned age) {
    return _find_corresponding_track(frame, track_array, ROI_array->id, sel_ROI_id, n_ROI, age);
}

void _features_ROI_write(FILE* f, const int frame, const uint32_t* ROI_id, const uint32_t* ROI_xmin,
                         const uint32_t* ROI_xmax, const uint32_t* ROI_ymin, const uint32_t* ROI_ymax,
                         const uint32_t* ROI_S, const uint32_t* ROI_Sx, const uint32_t* ROI_Sy, const float* ROI_x,
                         const float* ROI_y, const uint32_t* ROI_magnitude, const size_t n_ROI,
                         const vec_track_t track_array, const unsigned age) {
    int cpt = 0;
    for (size_t i = 0; i < n_ROI; i++)
        if (ROI_id[i] != 0)
            cpt++;

    fprintf(f, "Regions of interest (ROI) [%d]: \n", cpt);
    // if (cpt) {  // for compare.py
        fprintf(f, "# ------||----------------||---------------------------||---------------------------||-------------------||-----------\n");
        fprintf(f, "#   ROI ||      Track     ||        Bounding Box       ||   Surface (S in pixels)   ||      Center       || Magnitude \n");
        fprintf(f, "# ------||----------------||---------------------------||---------------------------||-------------------||-----------\n");
        fprintf(f, "# ------||------|---------||------|------|------|------||-----|----------|----------||---------|---------||-----------\n");
        fprintf(f, "#    ID ||   ID |    Type || xmin | xmax | ymin | ymax ||   S |       Sx |       Sy ||       x |       y ||        -- \n");
        fprintf(f, "# ------||------|---------||------|------|------|------||-----|----------|----------||---------|---------||-----------\n");
    // }

    for (size_t i = 0; i < n_ROI; i++) {
        if (ROI_id[i] != 0) {
            int t = _find_corresponding_track(frame, track_array, ROI_id, ROI_id[i], n_ROI, age);
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
            if (ROI_magnitude != NULL)
                mag = ROI_magnitude[i];
            fprintf(f, "   %4d || %s | %s || %4d | %4d | %4d | %4d || %3d | %8d | %8d || %7.1f | %7.1f || %9d \n",
                    ROI_id[i], task_id_str, task_obj_type, ROI_xmin[i], ROI_xmax[i], ROI_ymin[i], ROI_ymax[i], ROI_S[i],
                    ROI_Sx[i], ROI_Sy[i], ROI_x[i], ROI_y[i], mag);
        }
    }
}

void features_ROI_write(FILE* f, const int frame, const ROI_basic_t* ROI_basic_array, const ROI_misc_t* ROI_misc_array,
                        const vec_track_t track_array, const unsigned age) {
    _features_ROI_write(f, frame, ROI_basic_array->id, ROI_basic_array->xmin, ROI_basic_array->xmax, ROI_basic_array->ymin,
                        ROI_basic_array->ymax, ROI_basic_array->S, ROI_basic_array->Sx, ROI_basic_array->Sy,
                        ROI_basic_array->x, ROI_basic_array->y, ROI_misc_array->magnitude, *ROI_basic_array->_size,
                        track_array, age);
}

void features_motion_write(FILE* f, const motion_t* motion_est1, const motion_t* motion_est2) {
    fprintf(f, "# Motion:\n");
    fprintf(f, "# ---------------------------------------------------||---------------------------------------------------\n");
    fprintf(f, "#  First motion estimation (with all associated ROIs)||   Second motion estimation (exclude moving ROIs)  \n");
    fprintf(f, "# ---------------------------------------------------||---------------------------------------------------\n");
    fprintf(f, "# ----------|---------|---------|----------|---------||----------|---------|---------|----------|---------\n");
    fprintf(f, "#     theta |      tx |      ty | mean err | std dev ||    theta |      tx |      ty | mean err | std dev \n");
    fprintf(f, "# ----------|---------|---------|----------|---------||----------|---------|---------|----------|---------\n");
    fprintf(f, "   %8.5f | %7.4f | %7.4f | %8.4f | %7.4f || %8.5f | %7.4f | %7.4f | %8.4f | %7.4f \n",
            motion_est1->theta, motion_est1->tx, motion_est1->ty, motion_est1->mean_error, motion_est1->std_deviation,
            motion_est2->theta, motion_est2->tx, motion_est2->ty, motion_est2->mean_error, motion_est2->std_deviation);
}

void _features_ROI0_ROI1_write(FILE* f, const int prev_frame, const int cur_frame, const uint32_t* ROI0_id,
                               const uint32_t* ROI0_xmin, const uint32_t* ROI0_xmax, const uint32_t* ROI0_ymin,
                               const uint32_t* ROI0_ymax, const uint32_t* ROI0_S, const uint32_t* ROI0_Sx,
                               const uint32_t* ROI0_Sy, const float* ROI0_x, const float* ROI0_y,
                               const uint32_t* ROI0_magnitude, const size_t n_ROI0, const uint32_t* ROI1_id,
                               const uint32_t* ROI1_xmin, const uint32_t* ROI1_xmax, const uint32_t* ROI1_ymin,
                               const uint32_t* ROI1_ymax, const uint32_t* ROI1_S, const uint32_t* ROI1_Sx,
                               const uint32_t* ROI1_Sy, const float* ROI1_x, const float* ROI1_y,
                               const uint32_t* ROI1_magnitude, const size_t n_ROI1, const vec_track_t track_array) {
    if (prev_frame >= 0) {
        fprintf(f, "# Frame n°%05d (t-1) -- ", prev_frame);
        _features_ROI_write(f, prev_frame, ROI0_id, ROI0_xmin, ROI0_xmax, ROI0_ymin, ROI0_ymax, ROI0_S, ROI0_Sx,
                            ROI0_Sy, ROI0_x, ROI0_y, ROI0_magnitude, n_ROI0, track_array, 1);
        fprintf(f, "#\n");
    }

    fprintf(f, "# Frame n°%05d (t) -- ", cur_frame);
    _features_ROI_write(f, cur_frame, ROI1_id, ROI1_xmin, ROI1_xmax, ROI1_ymin, ROI1_ymax, ROI1_S, ROI1_Sx, ROI1_Sy,
                        ROI1_x, ROI1_y, ROI1_magnitude, n_ROI1, track_array, 0);
}

void features_ROI0_ROI1_write(FILE* f, const int prev_frame, const int cur_frame, const ROI_basic_t* ROI_basic_array0,
                              const ROI_misc_t* ROI_misc_array0, const ROI_basic_t* ROI_basic_array1,
                              const ROI_misc_t* ROI_misc_array1, const vec_track_t track_array) {
    _features_ROI0_ROI1_write(f, prev_frame, cur_frame, ROI_basic_array0->id, ROI_basic_array0->xmin,
                              ROI_basic_array0->xmax, ROI_basic_array0->ymin, ROI_basic_array0->ymax,
                              ROI_basic_array0->S, ROI_basic_array0->Sx, ROI_basic_array0->Sy, ROI_basic_array0->x,
                              ROI_basic_array0->y, ROI_misc_array0->magnitude, *ROI_basic_array0->_size,
                              ROI_basic_array1->id, ROI_basic_array1->xmin, ROI_basic_array1->xmax,
                              ROI_basic_array1->ymin, ROI_basic_array1->ymax, ROI_basic_array1->S, ROI_basic_array1->Sx,
                              ROI_basic_array1->Sy, ROI_basic_array1->x, ROI_basic_array1->y,
                              ROI_misc_array1->magnitude, *ROI_basic_array1->_size, track_array);
}
