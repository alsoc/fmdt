/**
 * Copyright (c) 2021-2022, Clara CIOCAN, LIP6 Sorbonne University
 * Copyright (c) 2021-2022, Mathuran KANDEEPAN, LIP6 Sorbonne University
 */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmdt/defines.h"
#include "fmdt/tools.h"
#include "fmdt/macros.h"
#include "fmdt/tracking.h"

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

size_t _tracking_get_track_time(const ROI_t* track_begin, const ROI_t* track_end, const size_t t) {
    return track_end->frame[t] - track_begin->frame[t];
}

size_t tracking_get_track_time(const track_t* track_array, const size_t t) {
    return _tracking_get_track_time(track_array->begin, track_array->end, t);
}

size_t tracking_count_objects(const track_t* track_array, unsigned* n_stars, unsigned* n_meteors,
                              unsigned* n_noise) {
    (*n_stars) = (*n_meteors) = (*n_noise) = 0;
    for (size_t i = 0; i < track_array->_size; i++)
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
                fprintf(stderr, "(EE) This should never happen ('track_array->obj_type[i] = %d', 'i = %lu')\n",
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

void _tracking_clear_index_track_array(uint16_t* track_id, const size_t t) {
    track_id[t] = 0;
}

void tracking_clear_index_track_array(track_t* track_array, const size_t t) {
    _tracking_clear_index_track_array(track_array->id, t);
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

ROI_history_t* alloc_ROI_history(const size_t max_history_size, const size_t max_ROI_size) {
    ROI_history_t* ROI_hist = (ROI_history_t*)malloc(sizeof(ROI_history_t));
    ROI_hist->_max_size = max_history_size;
    ROI_hist->array = (ROI_t**)malloc(ROI_hist->_max_size * sizeof(ROI_t*));
    for (size_t i = 0; i < ROI_hist->_max_size; i++)
        ROI_hist->array[i] = features_alloc_ROI_array(max_ROI_size);
    return ROI_hist;
}

void free_ROI_history(ROI_history_t* ROI_hist) {
    for (size_t i = 0; i < ROI_hist->_max_size; i++)
        features_free_ROI_array(ROI_hist->array[i]);
    free(ROI_hist->array);
    free(ROI_hist);
}

void rotate_ROI_history(ROI_history_t* ROI_hist) {
    ROI_t* last_ROI_tmp = ROI_hist->array[ROI_hist->_max_size -1];
    for (int i = (int)(ROI_hist->_max_size -2); i >= 0; i--)
        ROI_hist->array[i + 1] = ROI_hist->array[i];
    ROI_hist->array[0] = last_ROI_tmp;
}


tracking_data_t* tracking_alloc_data(const size_t max_history_size, const size_t max_ROI_size) {
    tracking_data_t* tracking_data = (tracking_data_t*)malloc(sizeof(tracking_data_t));
    tracking_data->ROI_history = alloc_ROI_history(max_history_size, max_ROI_size);
    tracking_data->ROI_list = features_alloc_ROI_array(max_history_size);
    return tracking_data;
}

void tracking_init_data(tracking_data_t* tracking_data) {
    features_init_ROI_array(tracking_data->ROI_list);
    for (size_t i = 0; i < tracking_data->ROI_history->_max_size; i++)
        features_init_ROI_array(tracking_data->ROI_history->array[i]);
    tracking_data->ROI_history->_size = 0;
}

void tracking_free_data(tracking_data_t* tracking_data) {
    free_ROI_history(tracking_data->ROI_history);
    features_free_ROI_array(tracking_data->ROI_list);
    free(tracking_data);
}

void _track_extrapolate(const ROI_t* track_end, float* track_extrapol_x, float* track_extrapol_y, const size_t t,
                        double theta, double tx, double ty) {
    // compensation du mouvement + calcul vitesse entre t-1 et t
    float u = track_end->x[t] - track_end->dx[t] - track_extrapol_x[t];
    float v = track_end->y[t] - track_end->dy[t] - track_extrapol_y[t];

    float x = (float)tx + track_end->x[t] * (float)cos(theta) - track_end->y[t] * (float)sin(theta);
    float y = (float)ty + track_end->x[t] * (float)sin(theta) + track_end->y[t] * (float)cos(theta);

    track_extrapol_x[t] = x + u;
    track_extrapol_y[t] = y + v;
}

void track_extrapolate(track_t* track_array, const size_t t, double theta, double tx, double ty) {
    _track_extrapolate(track_array->end, track_array->extrapol_x, track_array->extrapol_y, t, theta, tx, ty);
}

void _update_bounding_box(BB_t** BB_array, const int track_id, const uint16_t* ROI_xmin, const uint16_t* ROI_xmax,
                          const uint16_t* ROI_ymin, const uint16_t* ROI_ymax, const size_t r, int frame) {
    assert(ROI_xmin[r] || ROI_xmax[r] || ROI_ymin[r] || ROI_ymax[r]);

    uint16_t bb_x = (uint16_t)ceil((double)((ROI_xmin[r] + ROI_xmax[r])) / 2);
    uint16_t bb_y = (uint16_t)ceil((double)((ROI_ymin[r] + ROI_ymax[r])) / 2);
    uint16_t rx = (bb_x - ROI_xmin[r]);
    uint16_t ry = (bb_y - ROI_ymin[r]);

    add_to_BB_array(BB_array, rx, ry, bb_x, bb_y, track_id, frame - 1);
}

void update_bounding_box(BB_t** BB_array, const int track_id, const ROI_t* ROI_array, const size_t r, int frame) {
    _update_bounding_box(BB_array, track_id, ROI_array->xmin, ROI_array->xmax, ROI_array->ymin, ROI_array->ymax, r,
                         frame);
}

void _light_copy_elmt_ROI_array(const uint16_t* ROI_src_id, const size_t* ROI_src_frame, const uint16_t* ROI_src_xmin,
                                const uint16_t* ROI_src_xmax, const uint16_t* ROI_src_ymin,
                                const uint16_t* ROI_src_ymax, const float* ROI_src_x, const float* ROI_src_y,
                                const int32_t* ROI_src_prev_id, const int32_t* ROI_src_next_id, const size_t i_src,
                                uint16_t* ROI_dest_id, size_t* ROI_dest_frame, uint16_t* ROI_dest_xmin,
                                uint16_t* ROI_dest_xmax, uint16_t* ROI_dest_ymin, uint16_t* ROI_dest_ymax,
                                float* ROI_dest_x, float* ROI_dest_y, int32_t* ROI_dest_prev_id,
                                int32_t* ROI_dest_next_id, const size_t i_dest) {
    ROI_dest_id[i_dest] = ROI_src_id[i_src];
    ROI_dest_frame[i_dest] = ROI_src_frame[i_src];
    ROI_dest_xmin[i_dest] = ROI_src_xmin[i_src];
    ROI_dest_xmax[i_dest] = ROI_src_xmax[i_src];
    ROI_dest_ymin[i_dest] = ROI_src_ymin[i_src];
    ROI_dest_ymax[i_dest] = ROI_src_ymax[i_src];
    ROI_dest_x[i_dest] = ROI_src_x[i_src];
    ROI_dest_y[i_dest] = ROI_src_y[i_src];
    ROI_dest_prev_id[i_dest] = ROI_src_prev_id[i_src];
    ROI_dest_next_id[i_dest] = ROI_src_next_id[i_src];
}

void light_copy_elmt_ROI_array(const ROI_t* ROI_array_src, ROI_t* ROI_array_dest, const int i_src, const int i_dest) {
    _light_copy_elmt_ROI_array(ROI_array_src->id, ROI_array_src->frame, ROI_array_src->xmin, ROI_array_src->xmax,
                               ROI_array_src->ymin, ROI_array_src->ymax, ROI_array_src->x, ROI_array_src->y,
                               ROI_array_src->prev_id, ROI_array_src->next_id, i_src, ROI_array_dest->id,
                               ROI_array_dest->frame, ROI_array_dest->xmin, ROI_array_dest->xmax, ROI_array_dest->ymin,
                               ROI_array_dest->ymax, ROI_array_dest->x, ROI_array_dest->y, ROI_array_dest->prev_id,
                               ROI_array_dest->next_id, i_dest);
}

void _update_existing_tracks(const ROI_t** ROI_hist, const uint16_t* ROI0_id, const size_t* ROI0_frame,
                             const uint16_t* ROI0_xmin, const uint16_t* ROI0_xmax, const uint16_t* ROI0_ymin,
                             const uint16_t* ROI0_ymax, const float* ROI0_x, const float* ROI0_y,
                             const int32_t* ROI0_prev_id, const int32_t* ROI0_next_id, const size_t n_ROI0,
                             const uint16_t* ROI1_id, const size_t* ROI1_frame, const uint16_t* ROI1_xmin,
                             const uint16_t* ROI1_xmax, const uint16_t* ROI1_ymin, const uint16_t* ROI1_ymax,
                             const float* ROI1_x, const float* ROI1_y, const int32_t* ROI1_prev_id,
                             const int32_t* ROI1_next_id, uint8_t* ROI1_is_extrapolated, const size_t n_ROI1,
                             uint16_t* track_id, const ROI_t* track_begin, ROI_t* track_end, float* track_extrapol_x,
                             float* track_extrapol_y, enum state_e* track_state, enum obj_e* track_obj_type,
                             enum change_state_reason_e* track_change_state_reason, size_t* offset_tracks,
                             const size_t n_tracks, BB_t** BB_array, size_t frame, double theta, double tx, double ty,
                             size_t r_extrapol, float angle_max, int track_all, size_t fra_meteor_max) {
    for (size_t i = *offset_tracks; i < n_tracks; i++) {
        int next_id = track_end->next_id[i];
        if (!next_id) {
            *offset_tracks = i;
            break;
        }
    }
    for (size_t i = *offset_tracks; i < n_tracks; i++) {
        if (track_id[i] && track_state[i] != TRACK_FINISHED) {
            if (track_state[i] == TRACK_EXTRAPOLATED) {
                for (size_t j = 0; j < n_ROI0; j++) {
                    if ((ROI0_x[j] > track_extrapol_x[i] - r_extrapol) &&
                        (ROI0_x[j] < track_extrapol_x[i] + r_extrapol) &&
                        (ROI0_y[j] < track_extrapol_y[i] + r_extrapol) &&
                        (ROI0_y[j] > track_extrapol_y[i] - r_extrapol)) {
                        _light_copy_elmt_ROI_array(ROI0_id, ROI0_frame, ROI0_xmin, ROI0_xmax, ROI0_ymin, ROI0_ymax,
                                                   ROI0_x, ROI0_y, ROI0_prev_id, ROI0_next_id, j, track_end->id,
                                                   track_end->frame, track_end->xmin, track_end->xmax, track_end->ymin,
                                                   track_end->ymax, track_end->x, track_end->y, track_end->prev_id,
                                                   track_end->next_id, i);
                        track_state[i] = TRACK_UPDATED;
                        // update_bounding_box(BB_array, track_id[i], ROI_array0, j, frame - 1);
                        _update_bounding_box(BB_array, track_id[i], ROI0_xmin, ROI0_xmax, ROI0_ymin, ROI0_ymax, j,
                                             frame - 1);
                    }
                }
            }
            if (track_state[i] == TRACK_LOST) {
                for (size_t j = 0; j < n_ROI1; j++) {
                    if (!ROI1_prev_id[j]) {
                        if ((ROI1_x[j] > track_extrapol_x[i] - r_extrapol) &&
                            (ROI1_x[j] < track_extrapol_x[i] + r_extrapol) &&
                            (ROI1_y[j] < track_extrapol_y[i] + r_extrapol) &&
                            (ROI1_y[j] > track_extrapol_y[i] - r_extrapol)) {
                            track_state[i] = TRACK_EXTRAPOLATED;
                            ROI1_is_extrapolated[j] = 1;
                        }
                    }
                }
                if (track_state[i] != TRACK_EXTRAPOLATED)
                    track_state[i] = TRACK_FINISHED;
            }
            if (track_state[i] == TRACK_UPDATED || track_state[i] == TRACK_NEW) {
                int next_id = ROI0_next_id[track_end->id[i] - 1];
                if (next_id) {
                    if (track_obj_type[i] == METEOR) {
                        if (ROI0_prev_id[track_end->id[i] - 1]) {
                            int k = ROI0_prev_id[track_end->id[i] - 1] - 1;
                            float u_x = ROI0_x[track_end->id[i] - 1] - ROI_hist[0]->x[k];
                            float u_y = ROI0_y[track_end->id[i] - 1] - ROI_hist[0]->y[k];
                            float v_x = ROI1_x[next_id - 1] - ROI_hist[0]->x[k];
                            float v_y = ROI1_y[next_id - 1] - ROI_hist[0]->y[k];
                            float scalar_prod_uv = u_x * v_x + u_y * v_y;
                            float norm_u = sqrtf(u_x * u_x + u_y * u_y);
                            float norm_v = sqrtf(v_x * v_x + v_y * v_y);
                            float cos_uv = scalar_prod_uv / (norm_u * norm_v);
                            float angle_rad = acosf(cos_uv >= 1 ? 0.99999f : cos_uv);
                            float angle_degree = angle_rad * (180.f / (float)M_PI);
                            // angle_degree = fmodf(angle_degree, 360.f);
                            if (angle_degree >= angle_max || norm_u > norm_v) {
                                track_change_state_reason[i] = (angle_degree >= angle_max) ?
                                                               REASON_TOO_BIG_ANGLE : REASON_WRONG_DIRECTION;
                                track_obj_type[i] = NOISE;
                                if (!track_all) {
                                    _tracking_clear_index_track_array(track_id, i);
                                    continue;
                                }
                            }
                        }
                    }
                    track_extrapol_x[i] = track_end->x[i];
                    track_extrapol_y[i] = track_end->y[i];
                    _light_copy_elmt_ROI_array(ROI1_id, ROI1_frame, ROI1_xmin, ROI1_xmax, ROI1_ymin, ROI1_ymax, ROI1_x,
                                               ROI1_y, ROI1_prev_id, ROI1_next_id, next_id - 1, track_end->id,
                                               track_end->frame, track_end->xmin, track_end->xmax, track_end->ymin,
                                               track_end->ymax, track_end->x, track_end->y, track_end->prev_id,
                                               track_end->next_id, i);
                    if (track_state[i] == TRACK_NEW) // because the right time has been set in 'insert_new_track'
                        track_state[i] = TRACK_UPDATED;
                    // update_bounding_box(BB_array, track_id[i], ROI_array1, next_id - 1, frame + 1);
                    _update_bounding_box(BB_array, track_id[i], ROI1_xmin, ROI1_xmax, ROI1_ymin, ROI1_ymax, next_id - 1,
                                         frame + 1);
                } else {
                    // on extrapole si pas finished
                    _track_extrapolate(track_end, track_extrapol_x, track_extrapol_y, i, theta, tx,
                                       ty);
                    track_state[i] = TRACK_LOST;
                }
            }
            if (track_obj_type[i] == METEOR && _tracking_get_track_time(track_begin, track_end, i) >= fra_meteor_max) {
                track_obj_type[i] = NOISE;
                track_change_state_reason[i] = REASON_TOO_LONG_DURATION;
                if (!track_all) {
                    _tracking_clear_index_track_array(track_id, i);
                    continue;
                }
            }
        }
    }
}

void update_existing_tracks(const ROI_t** ROI_hist, const ROI_t* ROI_array0, ROI_t* ROI_array1, track_t* track_array,
                            BB_t** BB_array, size_t frame, double theta, double tx, double ty, size_t r_extrapol,
                            float angle_max, int track_all, size_t fra_meteor_max) {
    _update_existing_tracks(ROI_hist, ROI_array0->id, ROI_array0->frame, ROI_array0->xmin, ROI_array0->xmax,
                            ROI_array0->ymin, ROI_array0->ymax, ROI_array0->x, ROI_array0->y, ROI_array0->prev_id,
                            ROI_array0->next_id, ROI_array0->_size, ROI_array1->id, ROI_array1->frame, ROI_array1->xmin,
                            ROI_array1->xmax, ROI_array1->ymin, ROI_array1->ymax, ROI_array1->x, ROI_array1->y,
                            ROI_array1->prev_id, ROI_array1->next_id, ROI_array1->is_extrapolated, ROI_array1->_size,
                            track_array->id, track_array->begin, track_array->end, track_array->extrapol_x,
                            track_array->extrapol_y, track_array->state, track_array->obj_type,
                            track_array->change_state_reason, &track_array->_offset, track_array->_size, BB_array,
                            frame, theta, tx, ty, r_extrapol, angle_max, track_all, fra_meteor_max);
}

void _insert_new_track(const ROI_t* ROI_list, unsigned n_ROI, uint16_t* track_id, ROI_t* track_begin, ROI_t* track_end,
                       enum state_e* track_state, enum obj_e* track_obj_type, size_t* n_tracks, BB_t** BB_array,
                       int frame, enum obj_e type) {
    assert(n_ROI >= 1);
    size_t cur_track = *n_tracks;
    track_id[cur_track] = cur_track + 1;
    light_copy_elmt_ROI_array(ROI_list, track_begin, n_ROI - 1, cur_track);
    light_copy_elmt_ROI_array(ROI_list, track_end, 0, cur_track);
    track_state[cur_track] = TRACK_NEW;
    track_obj_type[cur_track] = type;
    for (unsigned n = 0; n < n_ROI; n++)
        update_bounding_box(BB_array, track_id[cur_track], ROI_list, n, frame - n);
    (*n_tracks)++;
}

void insert_new_track(const ROI_t* ROI_list, unsigned n_ROI, track_t* track_array, BB_t** BB_array,
                      int frame, enum obj_e type) {
    assert(track_array->_size < track_array->_max_size);
    _insert_new_track(ROI_list, n_ROI, track_array->id, track_array->begin, track_array->end, track_array->state,
                      track_array->obj_type,  &track_array->_size, BB_array, frame, type);
}

void _fill_ROI_list(const ROI_t** ROI_hist, ROI_t* ROI_list, const uint16_t* ROI_id, const size_t* ROI_frame,
                    const uint16_t* ROI_xmin, const uint16_t* ROI_xmax, const uint16_t* ROI_ymin,
                    const uint16_t* ROI_ymax, const float* ROI_x, const float* ROI_y, const int32_t* ROI_prev_id,
                    const int32_t* ROI_next_id, const size_t r) {
    _light_copy_elmt_ROI_array(ROI_id, ROI_frame, ROI_xmin, ROI_xmax, ROI_ymin,
                               ROI_ymax, ROI_x, ROI_y, ROI_prev_id, ROI_next_id, r, ROI_list->id,
                               ROI_list->frame, ROI_list->xmin, ROI_list->xmax, ROI_list->ymin, ROI_list->ymax,
                               ROI_list->x, ROI_list->y, ROI_list->prev_id, ROI_list->next_id, 0);
    for (size_t i = 1; i < ROI_list->_size; i++)
        light_copy_elmt_ROI_array(ROI_hist[i - 1], ROI_list, ROI_list->prev_id[i - 1] - 1, i);
}

void fill_ROI_list(const ROI_t** ROI_hist, ROI_t* ROI_list, const ROI_t* ROI_array, const size_t r) {
    _fill_ROI_list(ROI_hist, ROI_list, ROI_array->id, ROI_array->frame, ROI_array->xmin, ROI_array->xmax,
                   ROI_array->ymin, ROI_array->ymax, ROI_array->x, ROI_array->y, ROI_array->prev_id, ROI_array->next_id,
                   r);
}

void _create_new_tracks(const ROI_t** ROI_hist, ROI_t* ROI_list, const uint16_t* ROI0_id, const size_t* ROI0_frame,
                        const uint16_t* ROI0_xmin, const uint16_t* ROI0_xmax, const uint16_t* ROI0_ymin,
                        const uint16_t* ROI0_ymax, const float* ROI0_x, const float* ROI0_y, const float* ROI0_error,
                        const int32_t* ROI0_prev_id, const int32_t* ROI0_next_id, const int32_t* ROI0_time,
                        const int32_t* ROI0_time_motion, const uint8_t* ROI0_is_extrapolated, const size_t n_ROI0,
                        int32_t* ROI1_time, int32_t* ROI1_time_motion, uint16_t* track_id, ROI_t* track_begin,
                        ROI_t* track_end, enum state_e* track_state, enum obj_e* track_obj_type,
                        const size_t offset_tracks, size_t* n_tracks, BB_t** BB_array, size_t frame, double mean_error,
                        double std_deviation, float diff_dev, int track_all, size_t fra_star_min, size_t fra_meteor_min)
{
    for (size_t i = 0; i < n_ROI0; i++) {
        float e = ROI0_error[i];
        int asso = ROI0_next_id[i];
        if (asso) {
            int is_new_meteor = 0;
            // if motion detected
            if (fabs(e - mean_error) > diff_dev * std_deviation) {
                if (ROI0_is_extrapolated[i])
                    continue; // Extrapolated
                is_new_meteor = 1;
            } // else it is a new star

            int fra_min;
            int time;
            if (is_new_meteor) {
                time = ROI0_time_motion[i] + 1;
                ROI1_time_motion[asso - 1] = time;
                fra_min = fra_meteor_min;
            }
            else if (track_all) {
                time = ROI0_time[i] + 1;
                ROI1_time[asso - 1] = time;
                fra_min = fra_star_min;
            }
            if (is_new_meteor || track_all) {
                if (time == fra_min - 1) {
                    // this loop prevent adding duplicated tracks
                    size_t j = offset_tracks;
                    while (j < *n_tracks && (track_end->id[j] != ROI0_id[i] ||
                           track_end->x[j] != ROI0_x[i] || track_end->y[j] != ROI0_y[i]))
                        j++;

                    if (j == *n_tracks || *n_tracks == 0) {
                        ROI_list->_size = fra_min - 1;
                        _fill_ROI_list(ROI_hist, ROI_list, ROI0_id, ROI0_frame, ROI0_xmin, ROI0_xmax, ROI0_ymin,
                                       ROI0_ymax, ROI0_x, ROI0_y, ROI0_prev_id, ROI0_next_id, i);
                        _insert_new_track(ROI_list, fra_min - 1, track_id, track_begin, track_end, track_state,
                                          track_obj_type, n_tracks, BB_array, frame, is_new_meteor ? METEOR : STAR);
                    }
                }
            }
        }
    }
}

void create_new_tracks(const ROI_t** ROI_hist, ROI_t* ROI_list, const ROI_t* ROI_array0, ROI_t* ROI_array1,
                       track_t* track_array, BB_t** BB_array, size_t frame, double mean_error, double std_deviation,
                       float diff_dev, int track_all, size_t fra_star_min, size_t fra_meteor_min) {
    _create_new_tracks(ROI_hist, ROI_list, ROI_array0->id, ROI_array0->frame, ROI_array0->xmin, ROI_array0->xmax,
                       ROI_array0->ymin, ROI_array0->ymax, ROI_array0->x, ROI_array0->y, ROI_array0->error,
                       ROI_array0->prev_id, ROI_array0->next_id, ROI_array0->time, ROI_array0->time_motion,
                       ROI_array0->is_extrapolated, ROI_array0->_size, ROI_array1->time, ROI_array1->time_motion,
                       track_array->id, track_array->begin, track_array->end, track_array->state, track_array->obj_type,
                       track_array->_offset, &track_array->_size, BB_array, frame, mean_error, std_deviation, diff_dev,
                       track_all, fra_star_min, fra_meteor_min);
}

void _light_copy_ROI_array(const uint16_t* ROI_src_id, const size_t* ROI_src_frame, const uint16_t* ROI_src_xmin,
                           const uint16_t* ROI_src_xmax, const uint16_t* ROI_src_ymin, const uint16_t* ROI_src_ymax,
                           const float* ROI_src_x, const float* ROI_src_y, const float* ROI_src_error,
                           const int32_t* ROI_src_time, const int32_t* ROI_src_time_motion,
                           const int32_t* ROI_src_prev_id, const int32_t* ROI_src_next_id,
                           const uint8_t* ROI_src_is_extrapolated, const size_t n_ROI_src, uint16_t* ROI_dest_id,
                           size_t* ROI_dest_frame, uint16_t* ROI_dest_xmin, uint16_t* ROI_dest_xmax,
                           uint16_t* ROI_dest_ymin, uint16_t* ROI_dest_ymax, float* ROI_dest_x, float* ROI_dest_y,
                           float* ROI_dest_error, int32_t* ROI_dest_time, int32_t* ROI_dest_time_motion,
                           int32_t* ROI_dest_prev_id, int32_t* ROI_dest_next_id, uint8_t* ROI_dest_is_extrapolated,
                           size_t* n_ROI_dest) {
    *n_ROI_dest = n_ROI_src;
    memcpy(ROI_dest_id, ROI_src_id, *n_ROI_dest * sizeof(uint16_t));
    memcpy(ROI_dest_frame, ROI_src_frame, *n_ROI_dest * sizeof(size_t));
    memcpy(ROI_dest_xmin, ROI_src_xmin, *n_ROI_dest * sizeof(uint16_t));
    memcpy(ROI_dest_xmax, ROI_src_xmax, *n_ROI_dest * sizeof(uint16_t));
    memcpy(ROI_dest_ymin, ROI_src_ymin, *n_ROI_dest * sizeof(uint16_t));
    memcpy(ROI_dest_ymax, ROI_src_ymax, *n_ROI_dest * sizeof(uint16_t));
    memcpy(ROI_dest_x, ROI_src_x, *n_ROI_dest * sizeof(float));
    memcpy(ROI_dest_y, ROI_src_y, *n_ROI_dest * sizeof(float));
    memcpy(ROI_dest_error, ROI_src_error, *n_ROI_dest * sizeof(float));
    memcpy(ROI_dest_time, ROI_src_time, *n_ROI_dest * sizeof(int32_t));
    memcpy(ROI_dest_time_motion, ROI_src_time_motion, *n_ROI_dest * sizeof(int32_t));
    memcpy(ROI_dest_prev_id, ROI_src_prev_id, *n_ROI_dest * sizeof(int32_t));
    memcpy(ROI_dest_next_id, ROI_src_next_id, *n_ROI_dest * sizeof(int32_t));
    memcpy(ROI_dest_is_extrapolated, ROI_src_is_extrapolated, *n_ROI_dest * sizeof(uint8_t));
}

void light_copy_ROI_array(const ROI_t* ROI_array_src, ROI_t* ROI_array_dest) {
    _light_copy_ROI_array(ROI_array_src->id, ROI_array_src->frame, ROI_array_src->xmin, ROI_array_src->xmax,
                          ROI_array_src->ymin, ROI_array_src->ymax, ROI_array_src->x, ROI_array_src->y,
                          ROI_array_src->error, ROI_array_src->time, ROI_array_src->time_motion, ROI_array_src->prev_id,
                          ROI_array_src->next_id, ROI_array_src->is_extrapolated, ROI_array_src->_size,
                          ROI_array_dest->id, ROI_array_dest->frame, ROI_array_dest->xmin, ROI_array_dest->xmax,
                          ROI_array_dest->ymin, ROI_array_dest->ymax, ROI_array_dest->x, ROI_array_dest->y,
                          ROI_array_dest->error, ROI_array_dest->time, ROI_array_dest->time_motion,
                          ROI_array_dest->prev_id, ROI_array_dest->next_id, ROI_array_dest->is_extrapolated,
                          &ROI_array_dest->_size);
}

void _tracking_perform(tracking_data_t* tracking_data, const uint16_t* ROI0_id, const size_t* ROI0_frame,
                       const uint16_t* ROI0_xmin, const uint16_t* ROI0_xmax, const uint16_t* ROI0_ymin,
                       const uint16_t* ROI0_ymax, const float* ROI0_x, const float* ROI0_y, const float* ROI0_error,
                       const int32_t* ROI0_time, const int32_t* ROI0_time_motion, const int32_t* ROI0_prev_id,
                       const int32_t* ROI0_next_id, uint8_t* const ROI0_is_extrapolated, const size_t n_ROI0,
                       const uint16_t* ROI1_id, const size_t* ROI1_frame, const uint16_t* ROI1_xmin,
                       const uint16_t* ROI1_xmax, const uint16_t* ROI1_ymin, const uint16_t* ROI1_ymax,
                       const float* ROI1_x, const float* ROI1_y, const float* ROI1_error, int32_t* ROI1_time,
                       int32_t* ROI1_time_motion, const int32_t* ROI1_prev_id, const int32_t* ROI1_next_id,
                       uint8_t* ROI1_is_extrapolated, const size_t n_ROI1, uint16_t* track_id, ROI_t* track_begin,
                       ROI_t* track_end, float* track_extrapol_x, float* track_extrapol_y, enum state_e* track_state,
                       enum obj_e* track_obj_type, enum change_state_reason_e* track_change_state_reason,
                       size_t* offset_tracks, size_t* n_tracks, BB_t** BB_array, size_t frame, double theta, double tx,
                       double ty, double mean_error, double std_deviation, size_t r_extrapol, float angle_max,
                       float diff_dev, int track_all, size_t fra_star_min, size_t fra_meteor_min, size_t fra_meteor_max)
{
    _light_copy_ROI_array(ROI1_id, ROI1_frame, ROI1_xmin, ROI1_xmax, ROI1_ymin, ROI1_ymax, ROI1_x, ROI1_y, ROI1_error,
                          ROI1_time, ROI1_time_motion, ROI1_prev_id, ROI1_next_id, ROI1_is_extrapolated, n_ROI1,
                          tracking_data->ROI_history->array[0]->id, tracking_data->ROI_history->array[0]->frame,
                          tracking_data->ROI_history->array[0]->xmin, tracking_data->ROI_history->array[0]->xmax,
                          tracking_data->ROI_history->array[0]->ymin, tracking_data->ROI_history->array[0]->ymax,
                          tracking_data->ROI_history->array[0]->x, tracking_data->ROI_history->array[0]->y,
                          tracking_data->ROI_history->array[0]->error, tracking_data->ROI_history->array[0]->time,
                          tracking_data->ROI_history->array[0]->time_motion,
                          tracking_data->ROI_history->array[0]->prev_id, tracking_data->ROI_history->array[0]->next_id,
                          tracking_data->ROI_history->array[0]->is_extrapolated,
                          &tracking_data->ROI_history->array[0]->_size);
    if (tracking_data->ROI_history->_size < tracking_data->ROI_history->_max_size)
        tracking_data->ROI_history->_size++;
    _create_new_tracks((const ROI_t**)&tracking_data->ROI_history->array[2], tracking_data->ROI_list, ROI0_id,
                       ROI0_frame, ROI0_xmin, ROI0_xmax, ROI0_ymin, ROI0_ymax, ROI0_x, ROI0_y, ROI0_error, ROI0_prev_id,
                       ROI0_next_id, ROI0_time, ROI0_time_motion, ROI0_is_extrapolated, n_ROI0, ROI1_time,
                       ROI1_time_motion, track_id, track_begin, track_end, track_state, track_obj_type, *offset_tracks,
                       n_tracks, BB_array, frame, mean_error, std_deviation, diff_dev, track_all, fra_star_min,
                       fra_meteor_min);
    _update_existing_tracks((const ROI_t**)&tracking_data->ROI_history->array[2], ROI0_id, ROI0_frame, ROI0_xmin,
                            ROI0_xmax, ROI0_ymin, ROI0_ymax, ROI0_x, ROI0_y, ROI0_prev_id, ROI0_next_id, n_ROI0,
                            ROI1_id, ROI1_frame, ROI1_xmin, ROI1_xmax, ROI1_ymin, ROI1_ymax, ROI1_x, ROI1_y,
                            ROI1_prev_id, ROI1_next_id, ROI1_is_extrapolated, n_ROI1, track_id, track_begin, track_end,
                            track_extrapol_x, track_extrapol_y, track_state, track_obj_type, track_change_state_reason,
                            offset_tracks, *n_tracks, BB_array, frame, theta, tx, ty, r_extrapol, angle_max, track_all,
                            fra_meteor_max);
    rotate_ROI_history(tracking_data->ROI_history);
}

void tracking_perform(tracking_data_t* tracking_data, const ROI_t* ROI_array0, ROI_t* ROI_array1, track_t* track_array,
                      BB_t** BB_array, size_t frame, double theta, double tx, double ty, double mean_error,
                      double std_deviation, size_t r_extrapol, float angle_max, float diff_dev, int track_all,
                      size_t fra_star_min, size_t fra_meteor_min, size_t fra_meteor_max) {
    _tracking_perform(tracking_data, ROI_array0->id, ROI_array0->frame, ROI_array0->xmin, ROI_array0->xmax,
                      ROI_array0->ymin, ROI_array0->ymax, ROI_array0->x, ROI_array0->y,  ROI_array0->error,
                      ROI_array0->time, ROI_array0->time_motion, ROI_array0->prev_id, ROI_array0->next_id,
                      ROI_array0->is_extrapolated, ROI_array0->_size, ROI_array1->id, ROI_array1->frame,
                      ROI_array1->xmin, ROI_array1->xmax, ROI_array1->ymin, ROI_array1->ymax, ROI_array1->x,
                      ROI_array1->y, ROI_array1->error, ROI_array1->time, ROI_array1->time_motion, ROI_array1->prev_id,
                      ROI_array1->next_id, ROI_array1->is_extrapolated, ROI_array1->_size, track_array->id,
                      track_array->begin, track_array->end, track_array->extrapol_x, track_array->extrapol_y,
                      track_array->state, track_array->obj_type, track_array->change_state_reason,
                      &track_array->_offset, &track_array->_size, BB_array, frame, theta, tx, ty, mean_error,
                      std_deviation, r_extrapol, angle_max, diff_dev, track_all, fra_star_min, fra_meteor_min,
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

void tracking_print_track_array(FILE* f, const track_t* track_array) {
    fprintf(f, "# -------||---------------------------||---------------------------||---------\n");
    fprintf(f, "#  Track ||           Begin           ||            End            ||  Object \n");
    fprintf(f, "# -------||---------------------------||---------------------------||---------\n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------\n");
    fprintf(f, "#     Id || Frame # |      x |      y || Frame # |      x |      y ||    Type \n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------\n");

    for (size_t i = 0; i < track_array->_size; i++)
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
