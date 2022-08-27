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

size_t tracking_get_track_time(track_t* track) {
    return track->end.frame - track->begin.frame;
}

size_t tracking_count_objects(const track_array_t* track_array, unsigned* n_stars, unsigned* n_meteors,
                              unsigned* n_noise) {
    const track_t* tracks = track_array->data;
    const int n_tracks = track_array->size;
    (*n_stars) = (*n_meteors) = (*n_noise) = 0;
    for (int i = 0; i < n_tracks; i++)
        if (tracks[i].id)
            switch (tracks[i].obj_type) {
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
                fprintf(stderr, "(EE) This should never happen ('tracks[i].obj_type = %d', 'i = %d')\n",
                        tracks[i].obj_type, i);
                exit(1);
            }
    return (*n_stars) + (*n_meteors) + (*n_noise);
}

track_array_t* tracking_alloc_track_array(const size_t max_size) {
    track_array_t* track_array = (track_array_t*)malloc(sizeof(track_array_t));
    track_array->max_size = max_size;
    track_array->data = (track_t*)malloc(sizeof(track_t) * track_array->max_size);
    return track_array;
}

void tracking_init_track_array(track_array_t* track_array) {
    tracking_init_tracks(track_array->data, track_array->max_size);
    track_array->size = 0;
    track_array->offset = 0;
}

void tracking_free_track_array(track_array_t* track_array) {
    free(track_array->data);
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

void clear_index_tracks(track_t* track) { memset(track, 0, sizeof(track_t)); }

void tracking_init_tracks(track_t* tracks, int n) {
    for (int i = 0; i < n; i++)
        clear_index_tracks(tracks + i);
}

void track_extrapolate(track_t* t, int theta, int tx, int ty) {
    // compensation du mouvement + calcul vitesse entre t-1 et t
    float u = t->end.x - t->end.dx - t->extrapol_x;
    float v = t->end.y - t->end.dy - t->extrapol_y;

    float x = tx + t->end.x * cos(theta) - t->end.y * sin(theta);
    float y = ty + t->end.x * sin(theta) + t->end.y * cos(theta);

    t->extrapol_x = x + u;
    t->extrapol_y = y + v;
}

void update_bounding_box(BB_t** BB_array, track_t* track, ROI_t stats, int frame) {
    assert(stats.xmin || stats.xmax || stats.ymin || stats.ymax);

    uint16_t bb_x = (uint16_t)ceil((double)((stats.xmin + stats.xmax)) / 2);
    uint16_t bb_y = (uint16_t)ceil((double)((stats.ymin + stats.ymax)) / 2);
    uint16_t rx = (bb_x - stats.xmin);
    uint16_t ry = (bb_y - stats.ymin);

    add_to_BB_array(BB_array, rx, ry, bb_x, bb_y, track->id, frame - 1);
}

void update_existing_tracks(ROI_history_t* ROI_hist, track_array_t* track_array, BB_t** BB_array, int frame, int theta,
                            int tx, int ty, int r_extrapol, float angle_max, int track_all, int fra_meteor_max) {
    const ROI_t* stats0 = (const ROI_t*)ROI_hist->array[1].data;
    ROI_t* stats1 = ROI_hist->array[0].data;
    int nc1 = ROI_hist->array[0].size;

    int i;
    for (i = track_array->offset; i < track_array->size; i++) {
        int next = track_array->data[i].end.next;
        if (!next) {
            track_array->offset = i;
            break;
        }
    }
    for (i = track_array->offset; i < track_array->size; i++) {
        track_t* track = track_array->data + i;
        if (track_array->data[i].id && track_array->data[i].state != TRACK_FINISHED) {
            if (track_array->data[i].state == TRACK_EXTRAPOLATED) {
                for (int j = 1; j <= nc1; j++) {
                    if ((stats0[j].x > track->extrapol_x - r_extrapol) &&
                        (stats0[j].x < track->extrapol_x + r_extrapol) &&
                        (stats0[j].y < track->extrapol_y + r_extrapol) &&
                        (stats0[j].y > track->extrapol_y - r_extrapol)) {
                        track->end = stats0[j];
                        track->state = TRACK_UPDATED;
                        update_bounding_box(BB_array, track, stats0[j], frame - 1);
                    }
                }
            }
            if (track->state == TRACK_LOST) {
                for (int j = 1; j <= nc1; j++) {
                    if (!stats1[j].prev) {
                        if ((stats1[j].x > track->extrapol_x - r_extrapol) &&
                            (stats1[j].x < track->extrapol_x + r_extrapol) &&
                            (stats1[j].y < track->extrapol_y + r_extrapol) &&
                            (stats1[j].y > track->extrapol_y - r_extrapol)) {
                            track->state = TRACK_EXTRAPOLATED;
                            stats1[j].is_extrapolated = 1;
                        }
                    }
                }
                if (track->state != TRACK_EXTRAPOLATED)
                    track->state = TRACK_FINISHED;
            }
            if (track->state == TRACK_UPDATED || track->state == TRACK_NEW) {
                int next = stats0[track->end.id].next;
                if (next) {
                    if (track->obj_type == METEOR) {
                        if (stats0[track->end.id].prev) {
                            int k = stats0[track->end.id].prev;
                            float u_x = stats0[track->end.id].x - ROI_hist->array[2].data[k].x;
                            float u_y = stats0[track->end.id].y - ROI_hist->array[2].data[k].y;
                            float v_x = stats1[next].x - ROI_hist->array[2].data[k].x;
                            float v_y = stats1[next].y - ROI_hist->array[2].data[k].y;
                            float scalar_prod_uv = u_x * v_x + u_y * v_y;
                            float norm_u = sqrtf(u_x * u_x + u_y * u_y);
                            float norm_v = sqrtf(v_x * v_x + v_y * v_y);
                            float cos_uv = scalar_prod_uv / (norm_u * norm_v);
                            float angle_rad = acosf(cos_uv >= 1 ? 0.99999f : cos_uv);
                            float angle_degree = angle_rad * (180.f / (float)M_PI);
                            // angle_degree = fmodf(angle_degree, 360.f);
                            if (angle_degree >= angle_max || norm_u > norm_v) {
                                track->change_state_reason = (angle_degree >= angle_max) ? REASON_TOO_BIG_ANGLE :
                                                                                           REASON_WRONG_DIRECTION;
                                track->obj_type = NOISE;
                                if (!track_all) {
                                    clear_index_tracks(track);
                                    continue;
                                }
                            }
                        }
                    }
                    track->extrapol_x = track->end.x;
                    track->extrapol_y = track->end.y;
                    track->end = stats1[next];
                    if (track->state == TRACK_NEW) // because the right time has been set in 'insert_new_track'
                        track->state = TRACK_UPDATED;
                    update_bounding_box(BB_array, track, stats1[next], frame + 1);
                } else {
                    // on extrapole si pas finished
                    track_extrapolate(track, theta, tx, ty);
                    track->state = TRACK_LOST;
                }
            }
            if (tracking_get_track_time(track) >= fra_meteor_max) {
                if (track->obj_type == METEOR)
                    track->obj_type = NOISE;
                    track->change_state_reason = REASON_TOO_LONG_DURATION;
                if (!track_all) {
                    clear_index_tracks(track);
                    continue;
                }
            }
        }
    }
}

void insert_new_track(const ROI_t* ROI_list[256], unsigned n_ROI, track_array_t* track_array, BB_t** BB_array,
                      int frame, enum obj_e type) {
    assert(track_array->size < track_array->max_size);
    assert(n_ROI >= 1);

    const ROI_t* first_ROI = ROI_list[n_ROI - 1];
    const ROI_t* last_ROI = ROI_list[0];

    size_t cur_track = track_array->size;
    track_array->data[cur_track].id = cur_track + 1;
    track_array->data[cur_track].begin = *first_ROI;
    track_array->data[cur_track].end = *last_ROI;
    track_array->data[cur_track].state = TRACK_NEW;
    track_array->data[cur_track].obj_type = type;

    for (unsigned n = 0; n < n_ROI; n++)
        update_bounding_box(BB_array, &track_array->data[cur_track], *ROI_list[n], frame - n);

    track_array->size++;
}

void fill_ROI_list(const ROI_history_t* ROI_hist, const ROI_t* ROI_list[256], const unsigned n_ROI,
                   const ROI_t* last_ROI) {
    assert(n_ROI < 256);
    ROI_list[0] = last_ROI;
    for (int i = 1; i < n_ROI; i++)
        ROI_list[i] = (ROI_t*)&ROI_hist->array[i+1].data[ROI_list[i - 1]->prev];
}

void create_new_tracks(ROI_history_t* ROI_hist, track_array_t* track_array, BB_t** BB_array, int frame, float diff_dev,
                       int track_all, int fra_star_min, int fra_meteor_min) {
    const ROI_t* ROI_list[256];
    const ROI_t* stats0 = (const ROI_t*)ROI_hist->array[1].data;
    ROI_t* stats1 = ROI_hist->array[0].data;
    int nc0 = ROI_hist->array[1].size;

    double mean_error = features_compute_mean_error(&ROI_hist->array[1]);
    double std_deviation = features_compute_std_deviation(&ROI_hist->array[1], mean_error);

    for (int i = 1; i <= nc0; i++) {
        float e = stats0[i].error;
        int asso = stats0[i].next;
        if (asso) {
            int is_new_meteor = 0;
            // if motion detected
            if (fabs(e - mean_error) > diff_dev * std_deviation) {
                if (stats0[i].is_extrapolated)
                    continue; // Extrapolated
                is_new_meteor = 1;
            } // else it is a new star

            int fra_min;
            int time;
            if (is_new_meteor) {
                time = stats0[i].time_motion + 1;
                stats1[stats0[i].next].time_motion = time;
                fra_min = fra_meteor_min;
            }
            else if (track_all) {
                time = stats0[i].time + 1;
                stats1[stats0[i].next].time = time;
                fra_min = fra_star_min;
            }
            if (is_new_meteor || track_all) {
                if (time == fra_min - 1) {
                    // this loop prevent adding duplicated tracks
                    int j = track_array->offset;
                    while (j < track_array->size && (track_array->data[j].end.id != stats0[i].id ||
                           track_array->data[j].end.x != stats0[i].x || track_array->data[j].end.y != stats0[i].y))
                        j++;

                    if (j == track_array->size || track_array->size == 0) {
                        fill_ROI_list((const ROI_history_t*)ROI_hist, ROI_list, fra_min - 1, &stats0[i]);
                        insert_new_track(ROI_list, fra_min - 1, track_array, BB_array, frame,
                                         is_new_meteor ? METEOR : STAR);
                    }
                }
            }
        }
    }
}

void tracking_perform(ROI_history_t* ROI_hist, track_array_t* track_array, BB_t** BB_array, int frame, int theta,
                      int tx, int ty, int r_extrapol, float angle_max, float diff_dev, int track_all, int fra_star_min,
                      int fra_meteor_min, int fra_meteor_max) {
    create_new_tracks(ROI_hist, track_array, BB_array, frame, diff_dev, track_all, fra_star_min, fra_meteor_min);
    update_existing_tracks(ROI_hist, track_array, BB_array, frame, theta, tx, ty, r_extrapol, angle_max, track_all,
                           fra_meteor_max);
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

void tracking_print_tracks(FILE* f, const track_t* tracks, const int n) {
    fprintf(f, "# -------||---------------------------||---------------------------||---------\n");
    fprintf(f, "#  Track ||           Begin           ||            End            ||  Object \n");
    fprintf(f, "# -------||---------------------------||---------------------------||---------\n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------\n");
    fprintf(f, "#     Id || Frame # |      x |      y || Frame # |      x |      y ||    Type \n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------\n");

    unsigned track_id = 0;
    for (int i = 0; i < n; i++)
        if (tracks[i].id) {
            fprintf(f, "   %5d || %7lu | %6.1f | %6.1f || %7lu | %6.1f | %6.1f || %s \n", tracks[i].id,
                    tracks[i].begin.frame, tracks[i].begin.x, tracks[i].begin.y, tracks[i].end.frame,
                    tracks[i].end.x, tracks[i].end.y, g_obj_to_string_with_spaces[tracks[i].obj_type]);
            track_id++;
        }
}

void tracking_parse_tracks(const char* filename, track_t* tracks, size_t* n) {
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

    *n = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        // printf("Retrieved line of length %zu:\n", read);
        if (line[0] != '#') {
            sscanf(line, "%d || %d | %f | %f || %d | %f | %f || %s ", &tid, &t0, &x0, &y0, &t1, &x1, &y1, obj_type_str);
            tracks[*n].id = tid;
            tracks[*n].begin.frame = t0;
            tracks[*n].end.frame = t1;
            tracks[*n].state = TRACK_FINISHED;
            tracks[*n].begin.x = x0;
            tracks[*n].begin.y = y0;
            tracks[*n].end.x = x1;
            tracks[*n].end.y = y1;
            tracks[*n].obj_type = tracking_string_to_obj_type((const char*)obj_type_str);
            (*n)++;
        }
    }
    fclose(fp);
    if (line)
        free(line);
}

void tracking_save_array_BB(const char* filename, BB_t** tabBB, track_t* tracks, int n, int track_all) {
    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        if (tabBB[i] != NULL) {
            for (BB_t* current = tabBB[i]; current != NULL; current = current->next) {
                if (track_all || (!track_all && tracks[(current->track_id) - 1].obj_type == METEOR))
                    fprintf(f, "%d %d %d %d %d %d \n", i, current->rx, current->ry, current->bb_x, current->bb_y,
                            current->track_id);
            }
        }
    }

    fclose(f);
}
