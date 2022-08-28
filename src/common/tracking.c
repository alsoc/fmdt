/**
 * Copyright (c) 2021-2022, Clara CIOCAN, LIP6 Sorbonne University
 * Copyright (c) 2021-2022, Mathuran KANDEEPAN, LIP6 Sorbonne University
 */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "tools.h"
#include "macros.h"
#include "tracking.h"

#define INF 9999999

enum color_e g_obj_to_color[N_OBJECTS];
char g_obj_to_string[N_OBJECTS][64];
char g_obj_to_string_with_spaces[N_OBJECTS][64];

void tracking_init_global_data() {
    g_obj_to_color[UNKNOWN] = UNKNOWN_COLOR;
    g_obj_to_color[STAR] = STAR_COLOR;
    g_obj_to_color[METEOR] = METEOR_COLOR;
    g_obj_to_color[NOISE] = NOISE_COLOR;

    char str_unknown[64] = UNKNOWN_STR;
    sprintf(g_obj_to_string[UNKNOWN], "%s", str_unknown);
    char str_star[64] = STAR_STR;
    sprintf(g_obj_to_string[STAR], "%s", str_star);
    char str_meteor[64] = METEOR_STR;
    sprintf(g_obj_to_string[METEOR], "%s", str_meteor);
    char str_noise[64] = NOISE_STR;
    sprintf(g_obj_to_string[NOISE], "%s", str_noise);

    unsigned max = 0;
    for (int i = 0; i < N_OBJECTS; i++)
        if (strlen(g_obj_to_string[i]) > max)
            max = strlen(g_obj_to_string[i]);

    for (int i = 0; i < N_OBJECTS; i++) {
        int len = strlen(g_obj_to_string[i]);
        int diff = max - len;
        for (int c = len; c >= 0; c--)
            g_obj_to_string_with_spaces[i][diff + c] = g_obj_to_string[i][c];
        for (int c = 0; c < diff; c++)
            g_obj_to_string_with_spaces[i][c] = ' ';
    }
}

enum obj_e tracking_string_to_obj_type(const char* string) {
    enum obj_e obj = UNKNOWN;
    for (int i = 0; i < N_OBJECTS; i++)
        if (!strcmp(string, g_obj_to_string[i])) {
            obj = (enum obj_e)i;
            break;
        }
    return obj;
}

size_t tracking_get_track_time(const track_t* track_array, const size_t t) {
    return track_array->end->frame[t] - track_array->begin->frame[t];
}

size_t tracking_count_objects(const track_t* track_array, unsigned* n_stars, unsigned* n_meteors,
                              unsigned* n_noise) {
    (*n_stars) = (*n_meteors) = (*n_noise) = 0;
    for (int i = 0; i < track_array->_size; i++)
        if (track_array->id[i])
            switch (track_array->obj_type[i]) {
            case STAR:
                (*n_stars)++;
                break;
            case METEOR:
                (*n_meteors)++;
                break;
            case NOISE:
                (*n_noise)++;
                break;
            default:
                fprintf(stderr, "(EE) This should never happen ('track_array->obj_type[i] = %d', 'i = %d')\n",
                        track_array->obj_type[i], i);
                exit(1);
            }
    return (*n_stars) + (*n_meteors) + (*n_noise);
}

track_t* tracking_alloc_track_array(const size_t max_size) {
    track_t* track_array = (track_t*)malloc(sizeof(track_t));
    track_array->id = (uint16_t*)malloc(max_size * sizeof(uint16_t));
    track_array->begin = features_alloc_ROI_array(max_size);
    track_array->end = features_alloc_ROI_array(max_size);
    track_array->extrapol_x = (float*)malloc(max_size * sizeof(float));
    track_array->extrapol_y = (float*)malloc(max_size * sizeof(float));
    track_array->state = (enum state_e*)malloc(max_size * sizeof(enum state_e));
    track_array->obj_type = (enum obj_e*)malloc(max_size * sizeof(enum obj_e));
    track_array->change_state_reason = (enum change_state_reason_e*)malloc(max_size * sizeof(enum change_state_reason_e));
    track_array->_max_size = max_size;
    return track_array;
}

void tracking_init_track_array(track_t* track_array) {
    memset(track_array->id, 0, track_array->_max_size * sizeof(uint16_t));
    features_init_ROI_array(track_array->begin);
    features_init_ROI_array(track_array->end);
    memset(track_array->extrapol_x, 0, track_array->_max_size * sizeof(float));
    memset(track_array->extrapol_y, 0, track_array->_max_size * sizeof(float));
    memset(track_array->state, 0, track_array->_max_size * sizeof(enum state_e));
    memset(track_array->obj_type, 0, track_array->_max_size * sizeof(enum obj_e));
    memset(track_array->change_state_reason, 0, track_array->_max_size * sizeof(enum change_state_reason_e));
    track_array->_size = 0;
    track_array->_offset = 0;
}

void tracking_clear_index_track_array(track_t* track_array, const size_t t) {
    track_array->id[t] = 0;
    features_clear_index_ROI_array(track_array->begin, t);
    features_clear_index_ROI_array(track_array->end, t);
    track_array->extrapol_x[t] = 0;
    track_array->extrapol_y[t] = 0;
    track_array->state[t] = (enum state_e)0;
    track_array->obj_type[t] = (enum obj_e)0;
    track_array->change_state_reason[t] = (enum change_state_reason_e)0;
}

void tracking_free_track_array(track_t* track_array) {
    free(track_array->id);
    features_free_ROI_array(track_array->begin);
    features_free_ROI_array(track_array->end);
    free(track_array->extrapol_x);
    free(track_array->extrapol_y);
    free(track_array->state);
    free(track_array->obj_type);
    free(track_array->change_state_reason);
    free(track_array);
}

void tracking_init_BB_array(BB_t** BB_array) {
    for (int i = 0; i < MAX_N_FRAMES; i++)
        BB_array[i] = NULL;
}

void tracking_free_BB_array(BB_t** BB_array) {
    unsigned i = 0;
    while (BB_array[i] != NULL) {
        BB_t* cur = BB_array[i];
        while (cur != NULL) {
            BB_t* next = cur->next;
            free(cur);
            cur = NULL;
            cur = next;
        }
        i++;
    }
}

void add_to_BB_array(BB_t** BB_array, uint16_t rx, uint16_t ry, uint16_t bb_x, uint16_t bb_y, uint16_t track_id,
                     int frame) {
    assert(frame < MAX_N_FRAMES);
    BB_t* newE = (BB_t*)malloc(sizeof(BB_t));
    newE->rx = rx;
    newE->ry = ry;
    newE->bb_x = bb_x;
    newE->bb_y = bb_y;
    newE->track_id = track_id;
    newE->next = BB_array[frame];
    BB_array[frame] = newE;
}

void track_extrapolate(track_t* track_array, const size_t t, int theta, int tx, int ty) {
    // compensation du mouvement + calcul vitesse entre t-1 et t
    float u = track_array->end->x[t] - track_array->end->dx[t] - track_array->extrapol_x[t];
    float v = track_array->end->y[t] - track_array->end->dy[t] - track_array->extrapol_y[t];

    float x = tx + track_array->end->x[t] * cos(theta) - track_array->end->y[t] * sin(theta);
    float y = ty + track_array->end->x[t] * sin(theta) + track_array->end->y[t] * cos(theta);

    track_array->extrapol_x[t] = x + u;
    track_array->extrapol_y[t] = y + v;
}

void update_bounding_box(BB_t** BB_array, const int track_id, const ROI_t* ROI_array, const size_t r, int frame) {
    assert(ROI_array->xmin[r] || ROI_array->xmax[r] || ROI_array->ymin[r] || ROI_array->ymax[r]);

    uint16_t bb_x = (uint16_t)ceil((double)((ROI_array->xmin[r] + ROI_array->xmax[r])) / 2);
    uint16_t bb_y = (uint16_t)ceil((double)((ROI_array->ymin[r] + ROI_array->ymax[r])) / 2);
    uint16_t rx = (bb_x - ROI_array->xmin[r]);
    uint16_t ry = (bb_y - ROI_array->ymin[r]);

    add_to_BB_array(BB_array, rx, ry, bb_x, bb_y, track_id, frame - 1);
}

void update_existing_tracks(const ROI_t* ROI_array0, ROI_t* ROI_array1, const ROI_t** ROI_hist, track_t* track_array,
                            BB_t** BB_array, int frame, int theta, int tx, int ty, int r_extrapol, float angle_max,
                            int track_all, int fra_meteor_max) {
    int i;
    for (i = track_array->_offset; i < track_array->_size; i++) {
        int next_id = track_array->end->next_id[i];
        if (!next_id) {
            track_array->_offset = i;
            break;
        }
    }
    for (i = track_array->_offset; i < track_array->_size; i++) {
        if (track_array->id[i] && track_array->state[i] != TRACK_FINISHED) {
            if (track_array->state[i] == TRACK_EXTRAPOLATED) {
                // TODO: why 'ROI_array1->_size' and not 'ROI_array0->_size'?
                for (int j = 0; j < ROI_array1->_size; j++) {
                    if ((ROI_array0->x[j] > track_array->extrapol_x[i] - r_extrapol) &&
                        (ROI_array0->x[j] < track_array->extrapol_x[i] + r_extrapol) &&
                        (ROI_array0->y[j] < track_array->extrapol_y[i] + r_extrapol) &&
                        (ROI_array0->y[j] > track_array->extrapol_y[i] - r_extrapol)) {
                        features_copy_ROI_array(ROI_array0, track_array->end, j, i);
                        track_array->state[i] = TRACK_UPDATED;
                        update_bounding_box(BB_array, track_array->id[i], ROI_array0, j, frame - 1);
                    }
                }
            }
            if (track_array->state[i] == TRACK_LOST) {
                for (int j = 0; j < ROI_array1->_size; j++) {
                    if (!ROI_array1->prev_id[j]) {
                        if ((ROI_array1->x[j] > track_array->extrapol_x[i] - r_extrapol) &&
                            (ROI_array1->x[j] < track_array->extrapol_x[i] + r_extrapol) &&
                            (ROI_array1->y[j] < track_array->extrapol_y[i] + r_extrapol) &&
                            (ROI_array1->y[j] > track_array->extrapol_y[i] - r_extrapol)) {
                            track_array->state[i] = TRACK_EXTRAPOLATED;
                            ROI_array1->is_extrapolated[j] = 1;
                        }
                    }
                }
                if (track_array->state[i] != TRACK_EXTRAPOLATED)
                    track_array->state[i] = TRACK_FINISHED;
            }
            if (track_array->state[i] == TRACK_UPDATED || track_array->state[i] == TRACK_NEW) {
                int next_id = ROI_array0->next_id[track_array->end->id[i] - 1];
                if (next_id) {
                    if (track_array->obj_type[i] == METEOR) {
                        if (ROI_array0->prev_id[track_array->end->id[i] - 1]) {
                            int k = ROI_array0->prev_id[track_array->end->id[i] - 1] - 1;
                            float u_x = ROI_array0->x[track_array->end->id[i] - 1] - ROI_hist[0]->x[k];
                            float u_y = ROI_array0->y[track_array->end->id[i] - 1] - ROI_hist[0]->y[k];
                            float v_x = ROI_array1->x[next_id - 1] - ROI_hist[0]->x[k];
                            float v_y = ROI_array1->y[next_id - 1] - ROI_hist[0]->y[k];
                            float scalar_prod_uv = u_x * v_x + u_y * v_y;
                            float norm_u = sqrtf(u_x * u_x + u_y * u_y);
                            float norm_v = sqrtf(v_x * v_x + v_y * v_y);
                            float cos_uv = scalar_prod_uv / (norm_u * norm_v);
                            float angle_rad = acosf(cos_uv >= 1 ? 0.99999f : cos_uv);
                            float angle_degree = angle_rad * (180.f / (float)M_PI);
                            // angle_degree = fmodf(angle_degree, 360.f);
                            if (angle_degree >= angle_max || norm_u > norm_v) {
                                track_array->change_state_reason[i] = (angle_degree >= angle_max) ?
                                                                      REASON_TOO_BIG_ANGLE : REASON_WRONG_DIRECTION;
                                track_array->obj_type[i] = NOISE;
                                if (!track_all) {
                                    tracking_clear_index_track_array(track_array, i);
                                    continue;
                                }
                            }
                        }
                    }
                    track_array->extrapol_x[i] = track_array->end->x[i];
                    track_array->extrapol_y[i] = track_array->end->y[i];
                    features_copy_ROI_array(ROI_array1, track_array->end, next_id - 1, i);
                    if (track_array->state[i] == TRACK_NEW) // because the right time has been set in 'insert_new_track'
                        track_array->state[i] = TRACK_UPDATED;
                    update_bounding_box(BB_array, track_array->id[i], ROI_array1, next_id - 1, frame + 1);
                } else {
                    // on extrapole si pas finished
                    track_extrapolate(track_array, i, theta, tx, ty);
                    track_array->state[i] = TRACK_LOST;
                }
            }
            if (track_array->obj_type[i] == METEOR && tracking_get_track_time(track_array, i) >= fra_meteor_max) {
                track_array->obj_type[i] = NOISE;
                track_array->change_state_reason[i] = REASON_TOO_LONG_DURATION;
                if (!track_all) {
                    tracking_clear_index_track_array(track_array, i);
                    continue;
                }
            }
        }
    }
}

void insert_new_track(const ROI_t* ROI_list, unsigned n_ROI, track_t* track_array, BB_t** BB_array,
                      int frame, enum obj_e type) {
    assert(track_array->_size < track_array->_max_size);
    assert(n_ROI >= 1);

    size_t cur_track = track_array->_size;
    track_array->id[cur_track] = cur_track + 1;
    features_copy_ROI_array(ROI_list, track_array->begin, n_ROI - 1, cur_track);
    features_copy_ROI_array(ROI_list, track_array->end, 0, cur_track);
    track_array->state[cur_track] = TRACK_NEW;
    track_array->obj_type[cur_track] = type;

    for (unsigned n = 0; n < n_ROI; n++)
        update_bounding_box(BB_array, track_array->id[cur_track], ROI_list, n, frame - n);

    track_array->_size++;
}

void fill_ROI_list(const ROI_t** ROI_hist, ROI_t* ROI_list, const ROI_t* ROI_array, const size_t r) {
    features_copy_ROI_array(ROI_array, ROI_list, r, 0);
    for (int i = 1; i < ROI_list->_size; i++)
        features_copy_ROI_array(ROI_hist[i - 1], ROI_list, ROI_list->prev_id[i - 1] - 1, i);
}

void create_new_tracks(const ROI_t* ROI_array0, ROI_t* ROI_array1, const ROI_t** ROI_hist, track_t* track_array,
                       BB_t** BB_array, int frame, float diff_dev, int track_all, int fra_star_min,
                       int fra_meteor_min) {
    // TODO: bad idea to allocate 'ROI_list' each time
    ROI_t* ROI_list = features_alloc_ROI_array(MAX(fra_star_min, 3));
    features_init_ROI_array(ROI_list);

    double mean_error = features_compute_mean_error(ROI_array0);
    double std_deviation = features_compute_std_deviation(ROI_array0, mean_error);

    for (int i = 0; i < ROI_array0->_size; i++) {
        float e = ROI_array0->error[i];
        int asso = ROI_array0->next_id[i];
        if (asso) {
            int is_new_meteor = 0;
            // if motion detected
            if (fabs(e - mean_error) > diff_dev * std_deviation) {
                if (ROI_array0->is_extrapolated[i])
                    continue; // Extrapolated
                is_new_meteor = 1;
            } // else it is a new star

            int fra_min;
            int time;
            if (is_new_meteor) {
                time = ROI_array0->time_motion[i] + 1;
                ROI_array1->time_motion[asso - 1] = time;
                fra_min = fra_meteor_min;
            }
            else if (track_all) {
                time = ROI_array0->time[i] + 1;
                ROI_array1->time[asso - 1] = time;
                fra_min = fra_star_min;
            }
            if (is_new_meteor || track_all) {
                if (time == fra_min - 1) {
                    // this loop prevent adding duplicated tracks
                    int j = track_array->_offset;
                    while (j < track_array->_size && (track_array->end->id[j] != ROI_array0->id[i] ||
                           track_array->end->x[j] != ROI_array0->x[i] || track_array->end->y[j] != ROI_array0->y[i]))
                        j++;

                    if (j == track_array->_size || track_array->_size == 0) {
                        ROI_list->_size = fra_min - 1;
                        fill_ROI_list(ROI_hist, ROI_list, ROI_array0, i);
                        insert_new_track(ROI_list, fra_min - 1, track_array, BB_array, frame,
                                         is_new_meteor ? METEOR : STAR);
                    }
                }
            }
        }
    }
}

void tracking_perform(const ROI_t* ROI_array0, ROI_t* ROI_array1, const ROI_t** ROI_hist, track_t* track_array,
                      BB_t** BB_array, int frame, int theta, int tx, int ty, int r_extrapol, float angle_max,
                      float diff_dev, int track_all, int fra_star_min, int fra_meteor_min, int fra_meteor_max) {
    create_new_tracks(ROI_array0, ROI_array1, ROI_hist, track_array, BB_array,  frame, diff_dev, track_all,
                      fra_star_min, fra_meteor_min);
    update_existing_tracks(ROI_array0, ROI_array1, ROI_hist, track_array, BB_array, frame, theta, tx, ty, r_extrapol,
                           angle_max, track_all, fra_meteor_max);
}

void tracking_print_array_BB(BB_t** BB_array, int n) {
    for (int i = 0; i < n; i++) {
        if (BB_array[i] != NULL) {
            for (BB_t* current = BB_array[i]; current != NULL; current = current->next) {
                printf("%d %d %d %d %d %d \n", i, current->rx, current->ry, current->bb_x, current->bb_y,
                       current->track_id);
            }
        }
    }
}

void tracking_print_track_array(FILE* f, const track_t* track_array) {
    fprintf(f, "# -------||---------------------------||---------------------------||---------\n");
    fprintf(f, "#  Track ||           Begin           ||            End            ||  Object \n");
    fprintf(f, "# -------||---------------------------||---------------------------||---------\n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------\n");
    fprintf(f, "#     Id || Frame # |      x |      y || Frame # |      x |      y ||    Type \n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------\n");

    for (int i = 0; i < track_array->_size; i++)
        if (track_array->id[i]) {
            fprintf(f, "   %5d || %7lu | %6.1f | %6.1f || %7lu | %6.1f | %6.1f || %s \n", track_array->id[i],
                    track_array->begin->frame[i], track_array->begin->x[i], track_array->begin->y[i],
                    track_array->end->frame[i], track_array->end->x[i], track_array->end->y[i],
                    g_obj_to_string_with_spaces[track_array->obj_type[i]]);
        }
}

void tracking_parse_tracks(const char* filename, track_t* track_array) {
    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "(EE) Can't open '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    int tid, t0, t1;
    float x0, x1, y0, y1;
    char obj_type_str[1024];

    while ((read = getline(&line, &len, fp)) != -1) {
        // printf("Retrieved line of length %zu:\n", read);
        if (line[0] != '#') {
            sscanf(line, "%d || %d | %f | %f || %d | %f | %f || %s ", &tid, &t0, &x0, &y0, &t1, &x1, &y1, obj_type_str);
            track_array->id[track_array->_size] = tid;
            track_array->begin->frame[track_array->_size] = t0;
            track_array->end->frame[track_array->_size] = t1;
            track_array->state[track_array->_size] = TRACK_FINISHED;
            track_array->begin->x[track_array->_size] = x0;
            track_array->begin->y[track_array->_size] = y0;
            track_array->end->x[track_array->_size] = x1;
            track_array->end->y[track_array->_size] = y1;
            track_array->obj_type[track_array->_size] = tracking_string_to_obj_type((const char*)obj_type_str);
            track_array->_size++;
        }
    }
    fclose(fp);
    if (line)
        free(line);
}

void tracking_save_array_BB(const char* filename, BB_t** tabBB, track_t* track_array, int N, int track_all) {
    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }

    for (int i = 0; i < N; i++) {
        if (tabBB[i] != NULL) {
            for (BB_t* current = tabBB[i]; current != NULL; current = current->next) {
                if (track_all || (!track_all && track_array->obj_type[(current->track_id) - 1] == METEOR))
                    fprintf(f, "%d %d %d %d %d %d \n", i, current->rx, current->ry, current->bb_x, current->bb_y,
                            current->track_id);
            }
        }
    }

    fclose(f);
}
