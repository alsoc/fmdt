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
#define MAX_MAGNITUDE_SIZE 100000

enum color_e g_obj_to_color[N_OBJECTS];
char g_obj_to_string[N_OBJECTS][64];
char g_obj_to_string_with_spaces[N_OBJECTS][64];
char g_change_state_to_string[N_REASONS][64];
char g_change_state_to_string_with_spaces[N_REASONS][64];

void tracking_init_global_data() {
    g_obj_to_color[UNKNOWN] = UNKNOWN_COLOR;
    g_obj_to_color[STAR] = STAR_COLOR;
    g_obj_to_color[METEOR] = METEOR_COLOR;
    g_obj_to_color[NOISE] = NOISE_COLOR;

    char str_unknown[64] = UNKNOWN_STR;
    snprintf(g_obj_to_string[UNKNOWN], sizeof(g_obj_to_string[UNKNOWN]), "%s", str_unknown);
    char str_star[64] = STAR_STR;
    snprintf(g_obj_to_string[STAR], sizeof(g_obj_to_string[STAR]), "%s", str_star);
    char str_meteor[64] = METEOR_STR;
    snprintf(g_obj_to_string[METEOR], sizeof(g_obj_to_string[METEOR]), "%s", str_meteor);
    char str_noise[64] = NOISE_STR;
    snprintf(g_obj_to_string[NOISE], sizeof(g_obj_to_string[NOISE]), "%s", str_noise);

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

    snprintf(g_change_state_to_string[UNKNOWN], sizeof(g_change_state_to_string[UNKNOWN]), "%s", str_unknown);
    char str_too_big_angle[64] = TOO_BIG_ANGLE_STR;
    snprintf(g_change_state_to_string[REASON_TOO_BIG_ANGLE], sizeof(g_change_state_to_string[REASON_TOO_BIG_ANGLE]), "%s", str_too_big_angle);
    char str_wrong_direction[64] = WRONG_DIRECTION_STR;
    snprintf(g_change_state_to_string[REASON_WRONG_DIRECTION], sizeof(g_change_state_to_string[REASON_WRONG_DIRECTION]), "%s", str_wrong_direction);
    char str_too_long_duration[64] = TOO_LONG_DURATION_STR;
    snprintf(g_change_state_to_string[REASON_TOO_LONG_DURATION], sizeof(g_change_state_to_string[REASON_TOO_LONG_DURATION]), "%s", str_too_long_duration);

    max = 0;
    for (int i = 0; i < N_REASONS; i++)
        if (strlen(g_change_state_to_string[i]) > max)
            max = strlen(g_change_state_to_string[i]);

    for (int i = 0; i < N_REASONS; i++) {
        int len = strlen(g_change_state_to_string[i]);
        int diff = max - len;
        for (int c = len; c >= 0; c--)
            g_change_state_to_string_with_spaces[i][diff + c] = g_change_state_to_string[i][c];
        for (int c = 0; c < diff; c++)
            g_change_state_to_string_with_spaces[i][c] = ' ';
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

size_t _tracking_get_track_time(const ROI_light_t* track_begin, const ROI_light_t* track_end, const size_t t) {
    return track_end[t].frame - track_begin[t].frame;
}

size_t tracking_get_track_time(const track_t* track_array, const size_t t) {
    return _tracking_get_track_time(track_array->begin, track_array->end, t);
}

size_t _tracking_count_objects(const uint16_t* track_id, const enum obj_e* track_obj_type, unsigned* n_stars,
                               unsigned* n_meteors, unsigned* n_noise, const size_t n_tracks) {
    (*n_stars) = (*n_meteors) = (*n_noise) = 0;
    for (size_t i = 0; i < n_tracks; i++)
        if (track_id[i])
            switch (track_obj_type[i]) {
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
                fprintf(stderr, "(EE) This should never happen ('track_obj_type[i] = %d', 'i = %lu')\n",
                        track_obj_type[i], (unsigned long)i);
                exit(1);
            }
    return (*n_stars) + (*n_meteors) + (*n_noise);
}

size_t tracking_count_objects(const track_t* track_array, unsigned* n_stars, unsigned* n_meteors,
                              unsigned* n_noise) {
    return _tracking_count_objects(track_array->id, track_array->obj_type, n_stars, n_meteors, n_noise,
                                   track_array->_size);
}

track_t* tracking_alloc_track_array(const size_t max_size) {
    track_t* track_array = (track_t*)malloc(sizeof(track_t));
    track_array->id = (uint16_t*)malloc(max_size * sizeof(uint16_t));
    track_array->begin = (ROI_light_t*)malloc(max_size * sizeof(ROI_light_t));
    track_array->end = (ROI_light_t*)malloc(max_size * sizeof(ROI_light_t));
    track_array->extrapol_x = (float*)malloc(max_size * sizeof(float));
    track_array->extrapol_y = (float*)malloc(max_size * sizeof(float));
    track_array->state = (enum state_e*)malloc(max_size * sizeof(enum state_e));
    track_array->obj_type = (enum obj_e*)malloc(max_size * sizeof(enum obj_e));
    track_array->change_state_reason = (enum change_state_reason_e*)malloc(max_size * sizeof(enum change_state_reason_e));

    track_array->magnitude = (int64_t**)malloc(max_size * sizeof(int64_t*));
    for (size_t i = 0; i < max_size; i++)
        track_array->magnitude[i] = (int64_t*)malloc(MAX_MAGNITUDE_SIZE * sizeof(int64_t));

    track_array->_max_size = max_size;
    return track_array;
}

void tracking_init_track_array(track_t* track_array) {
    memset(track_array->id, 0, track_array->_max_size * sizeof(uint16_t));
    memset(track_array->begin, 0, track_array->_max_size * sizeof(ROI_light_t));
    memset(track_array->end, 0, track_array->_max_size * sizeof(ROI_light_t));
    memset(track_array->extrapol_x, 0, track_array->_max_size * sizeof(float));
    memset(track_array->extrapol_y, 0, track_array->_max_size * sizeof(float));
    memset(track_array->state, 0, track_array->_max_size * sizeof(enum state_e));
    memset(track_array->obj_type, 0, track_array->_max_size * sizeof(enum obj_e));
    memset(track_array->change_state_reason, 0, track_array->_max_size * sizeof(enum change_state_reason_e));

    for (size_t i = 0; i < track_array->_max_size; i++) {
        for (size_t j = 0; j < MAX_MAGNITUDE_SIZE - 1; j++)
            track_array->magnitude[i][j] = -1;
        track_array->magnitude[i][MAX_MAGNITUDE_SIZE - 1] = -2;
    }

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
    free(track_array->begin);
    free(track_array->end);
    free(track_array->extrapol_x);
    free(track_array->extrapol_y);
    free(track_array->state);
    free(track_array->obj_type);
    free(track_array->change_state_reason);

    for (size_t i = 0; i < track_array->_max_size; i++)
        free(track_array->magnitude[i]);
    free(track_array->magnitude);

    free(track_array);
}

void tracking_init_BB_array(BB_t** BB_array) {
    for (int i = 0; i < MAX_BB_LIST_SIZE; i++)
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
                     int frame, int is_extrapolated) {
    assert(frame < MAX_BB_LIST_SIZE);
    BB_t* newE = (BB_t*)malloc(sizeof(BB_t));
    newE->rx = rx;
    newE->ry = ry;
    newE->bb_x = bb_x;
    newE->bb_y = bb_y;
    newE->track_id = track_id;
    newE->is_extrapolated = is_extrapolated;
    newE->next = BB_array[frame];
    BB_array[frame] = newE;
}

ROI_history_t* alloc_ROI_history(const size_t max_history_size, const size_t max_ROI_size) {
    ROI_history_t* ROI_hist = (ROI_history_t*)malloc(sizeof(ROI_history_t));
    ROI_hist->_max_size = max_history_size;
    ROI_hist->array = (ROI_light_t**)malloc(ROI_hist->_max_size * sizeof(ROI_light_t*));
    ROI_hist->n_ROI = (uint32_t*)malloc(ROI_hist->_max_size * sizeof(uint32_t));
    ROI_hist->_max_n_ROI = max_ROI_size;
    for (size_t i = 0; i < ROI_hist->_max_size; i++)
        ROI_hist->array[i] = (ROI_light_t*)malloc(max_ROI_size * sizeof(ROI_light_t));
    return ROI_hist;
}

void free_ROI_history(ROI_history_t* ROI_hist) {
    for (size_t i = 0; i < ROI_hist->_max_size; i++)
        free(ROI_hist->array[i]);
    free(ROI_hist->array);
    free(ROI_hist->n_ROI);
    free(ROI_hist);
}

void rotate_ROI_history(ROI_history_t* ROI_hist) {
    ROI_light_t* last_ROI_tmp = ROI_hist->array[ROI_hist->_max_size -1];
    uint32_t last_n_ROI_tmp = ROI_hist->n_ROI[ROI_hist->_max_size -1];
    for (int i = (int)(ROI_hist->_max_size -2); i >= 0; i--) {
        ROI_hist->array[i + 1] = ROI_hist->array[i];
        ROI_hist->n_ROI[i + 1] = ROI_hist->n_ROI[i];
    }
    ROI_hist->array[0] = last_ROI_tmp;
    ROI_hist->n_ROI[0] = last_n_ROI_tmp;
}


tracking_data_t* tracking_alloc_data(const size_t max_history_size, const size_t max_ROI_size) {
    tracking_data_t* tracking_data = (tracking_data_t*)malloc(sizeof(tracking_data_t));
    tracking_data->ROI_history = alloc_ROI_history(max_history_size, max_ROI_size);
    // tracking_data->ROI_list = features_alloc_ROI_array(max_history_size);
    tracking_data->ROI_list = (ROI_light_t*)malloc(max_history_size * sizeof(ROI_light_t));
    return tracking_data;
}

void tracking_init_data(tracking_data_t* tracking_data) {
    memset(tracking_data->ROI_list, 0, tracking_data->ROI_history->_max_size * sizeof(ROI_light_t));
    for (size_t i = 0; i < tracking_data->ROI_history->_max_size; i++) {
        memset(tracking_data->ROI_history->array[i], 0, tracking_data->ROI_history->_max_n_ROI * sizeof(ROI_light_t));
        tracking_data->ROI_history->n_ROI[i] = 0;
    }
    tracking_data->ROI_history->_size = 0;
}

void tracking_free_data(tracking_data_t* tracking_data) {
    free_ROI_history(tracking_data->ROI_history);
    // features_free_ROI_array(tracking_data->ROI_list);
    free(tracking_data->ROI_list);
    free(tracking_data);
}

void _track_extrapolate(const ROI_light_t* track_end, float* track_extrapol_x, float* track_extrapol_y, double theta,
                        double tx, double ty) {
    // compensation du mouvement + calcul vitesse entre t-1 et t
    float u = track_end->x - track_end->dx - *track_extrapol_x;
    float v = track_end->y - track_end->dy - *track_extrapol_y;

    float x = (float)tx + track_end->x * (float)cos(theta) - track_end->y * (float)sin(theta);
    float y = (float)ty + track_end->x * (float)sin(theta) + track_end->y * (float)cos(theta);

    *track_extrapol_x = x + u;
    *track_extrapol_y = y + v;
}

void track_extrapolate(track_t* track_array, const size_t t, double theta, double tx, double ty) {
    _track_extrapolate(&track_array->end[t], &track_array->extrapol_x[t], &track_array->extrapol_y[t], theta, tx, ty);
}

void _update_bounding_box(BB_t** BB_array, const int track_id, const uint16_t ROI_xmin, const uint16_t ROI_xmax,
                          const uint16_t ROI_ymin, const uint16_t ROI_ymax, int frame, int is_extrapolated) {
    assert(ROI_xmin || ROI_xmax || ROI_ymin || ROI_ymax);
    assert(BB_array != NULL);

    uint16_t bb_x = (uint16_t)ceil((double)((ROI_xmin + ROI_xmax)) / 2);
    uint16_t bb_y = (uint16_t)ceil((double)((ROI_ymin + ROI_ymax)) / 2);
    uint16_t rx = (bb_x - ROI_xmin);
    uint16_t ry = (bb_y - ROI_ymin);

    add_to_BB_array(BB_array, rx, ry, bb_x, bb_y, track_id, frame, is_extrapolated);
}

void update_bounding_box(BB_t** BB_array, const int track_id, const ROI_t* ROI_array, const size_t r, int frame,
                         int is_extrapolated) {
    _update_bounding_box(BB_array, track_id, ROI_array->xmin[r], ROI_array->xmax[r], ROI_array->ymin[r],
                         ROI_array->ymax[r], frame, is_extrapolated);
}

void _update_existing_tracks(ROI_history_t* ROI_history, uint16_t* track_id, const ROI_light_t* track_begin,
                             ROI_light_t* track_end, float* track_extrapol_x, float* track_extrapol_y,
                             enum state_e* track_state, enum obj_e* track_obj_type,
                             enum change_state_reason_e* track_change_state_reason, int64_t** track_magnitude,
                             size_t* offset_tracks, const size_t n_tracks, BB_t** BB_array, size_t frame, double theta,
                             double tx, double ty, size_t r_extrapol, float angle_max, int track_all,
                             size_t fra_meteor_max) {
    for (size_t i = *offset_tracks; i < n_tracks; i++) {
        int next_id = track_end[i].next_id;
        if (!next_id) {
            *offset_tracks = i;
            break;
        }
    }
    for (size_t i = *offset_tracks; i < n_tracks; i++) {
        if (track_id[i] && track_state[i] != TRACK_FINISHED) {
            if (track_state[i] == TRACK_LOST) {
                for (size_t j = 0; j < ROI_history->n_ROI[0]; j++) {
                    if (!ROI_history->array[0][j].prev_id) {
                        if ((ROI_history->array[0][j].x > track_extrapol_x[i] - r_extrapol) &&
                            (ROI_history->array[0][j].x < track_extrapol_x[i] + r_extrapol) &&
                            (ROI_history->array[0][j].y < track_extrapol_y[i] + r_extrapol) &&
                            (ROI_history->array[0][j].y > track_extrapol_y[i] - r_extrapol)) {

                            track_state[i] = TRACK_UPDATED;
                            ROI_history->array[0][j].is_extrapolated = 1;
                            memcpy(&track_end[i], &ROI_history->array[0][j], sizeof(ROI_light_t));

                            assert(track_magnitude[i][frame - track_begin[i].frame] == -1);
                            track_magnitude[i][frame - track_begin[i].frame] = ROI_history->array[0][j].magnitude;

                            int m = (frame - 1) - track_begin[i].frame;
                            while (m >= 0 && track_magnitude[i][m] == -1) {
                                track_magnitude[i][m] = 0;
                                m--;
                            }

                            if (BB_array != NULL)
                                _update_bounding_box(BB_array, track_id[i], ROI_history->array[0][j].xmin,
                                                     ROI_history->array[0][j].xmax, ROI_history->array[0][j].ymin,
                                                     ROI_history->array[0][j].ymax, frame, /* is_extrapolated = */ 1);

                            // in the current implementation, the first ROI that matches is used for extrapolation
                            break;
                        }
                    }
                }
                if (track_state[i] != TRACK_UPDATED)
                    track_state[i] = TRACK_FINISHED;
            }
            else if (track_state[i] == TRACK_UPDATED || track_state[i] == TRACK_NEW) {
                int next_id = ROI_history->array[1][track_end[i].id - 1].next_id;
                if (next_id) {
                    if (track_obj_type[i] == METEOR) {
                        if (ROI_history->array[1][track_end[i].id - 1].prev_id) {
                            int k = ROI_history->array[1][track_end[i].id - 1].prev_id - 1;
                            float u_x = ROI_history->array[1][track_end[i].id - 1].x - ROI_history->array[2][k].x;
                            float u_y = ROI_history->array[1][track_end[i].id - 1].y - ROI_history->array[2][k].y;
                            float v_x = ROI_history->array[0][next_id - 1].x - ROI_history->array[2][k].x;
                            float v_y = ROI_history->array[0][next_id - 1].y - ROI_history->array[2][k].y;
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
                    track_extrapol_x[i] = track_end[i].x;
                    track_extrapol_y[i] = track_end[i].y;

                    memcpy(&track_end[i], &ROI_history->array[0][next_id - 1], sizeof(ROI_light_t));

                    if (track_state[i] == TRACK_NEW) // because the right time has been set in 'insert_new_track'
                        track_state[i] = TRACK_UPDATED;

                    assert(track_magnitude[i][frame - track_begin[i].frame] == -1);
                    assert(track_magnitude[i][(frame - 1) - track_begin[i].frame] != -1);
                    track_magnitude[i][frame - track_begin[i].frame] = ROI_history->array[0][next_id - 1].magnitude;
                    if (BB_array != NULL)
                        _update_bounding_box(BB_array, track_id[i], ROI_history->array[0][next_id - 1].xmin,
                                             ROI_history->array[0][next_id - 1].xmax,
                                             ROI_history->array[0][next_id - 1].ymin,
                                             ROI_history->array[0][next_id - 1].ymax, frame, /* is_extrapolated = */ 0);
                } else {
                    // on extrapole si pas finished
                    _track_extrapolate(&track_end[i], &track_extrapol_x[i], &track_extrapol_y[i], theta, tx, ty);
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

void update_existing_tracks(ROI_history_t* ROI_history, track_t* track_array, BB_t** BB_array, size_t frame,
                            double theta, double tx, double ty, size_t r_extrapol, float angle_max, int track_all,
                            size_t fra_meteor_max) {
    _update_existing_tracks(ROI_history, track_array->id, track_array->begin, track_array->end, track_array->extrapol_x,
                            track_array->extrapol_y, track_array->state, track_array->obj_type,
                            track_array->change_state_reason, track_array->magnitude, &track_array->_offset,
                            track_array->_size, BB_array, frame, theta, tx, ty, r_extrapol, angle_max, track_all,
                            fra_meteor_max);
}

void _insert_new_track(const ROI_light_t* ROI_list, unsigned n_ROI, uint16_t* track_id, ROI_light_t* track_begin,
                       ROI_light_t* track_end, enum state_e* track_state, enum obj_e* track_obj_type,
                       int64_t** track_magnitude, size_t* n_tracks, BB_t** BB_array, int frame, enum obj_e type) {
    assert(n_ROI >= 1);
    assert((*n_tracks) + 1 < MAX_TRACKS_SIZE);
    size_t cur_track = *n_tracks;
    track_id[cur_track] = cur_track + 1;
    memcpy(&track_begin[cur_track], &ROI_list[n_ROI - 1], sizeof(ROI_light_t));
    memcpy(&track_end[cur_track], &ROI_list[0], sizeof(ROI_light_t));
    track_state[cur_track] = TRACK_NEW;
    track_obj_type[cur_track] = type;
    for (unsigned n = 0; n < n_ROI; n++) {
        assert(track_magnitude[cur_track][(n_ROI - 1) - n] == -1);
        track_magnitude[cur_track][(n_ROI - 1) - n] = ROI_list[n].magnitude;
    }
    if (BB_array != NULL)
        for (unsigned n = 0; n < n_ROI; n++)
            _update_bounding_box(BB_array, track_id[cur_track], ROI_list[n].xmin, ROI_list[n].xmax, ROI_list[n].ymin,
                                 ROI_list[n].ymax, (frame - n) - 1, /* is_extrapolated = */ 0);
    (*n_tracks)++;
}

void insert_new_track(const ROI_light_t* ROI_list, unsigned n_ROI, track_t* track_array, BB_t** BB_array,
                      int frame, enum obj_e type) {
    assert(track_array->_size < track_array->_max_size);
    _insert_new_track(ROI_list, n_ROI, track_array->id, track_array->begin, track_array->end, track_array->state,
                      track_array->obj_type, track_array->magnitude, &track_array->_size, BB_array, frame, type);
}

void _create_new_tracks(ROI_history_t* ROI_history, ROI_light_t* ROI_list, uint16_t* track_id, ROI_light_t* track_begin,
                        ROI_light_t* track_end, enum state_e* track_state, enum obj_e* track_obj_type,
                        int64_t** track_magnitude, const size_t offset_tracks, size_t* n_tracks, BB_t** BB_array,
                        size_t frame, double mean_error, double std_deviation, float diff_dev, int track_all,
                        size_t fra_star_min, size_t fra_meteor_min)
{
    for (size_t i = 0; i < ROI_history->n_ROI[1]; i++) {
        float e = ROI_history->array[1][i].error;
        int asso = ROI_history->array[1][i].next_id;
        if (asso) {
            int is_new_meteor = 0;
            // if motion detected
            if (fabs(e - mean_error) > diff_dev * std_deviation) {
                if (ROI_history->array[1][i].is_extrapolated)
                    continue; // Extrapolated
                is_new_meteor = 1;
            } // else it is a new star

            int fra_min;
            int time;
            if (is_new_meteor) {
                time = ROI_history->array[1][i].time_motion + 1;
                ROI_history->array[0][asso - 1].time_motion = time;
                fra_min = fra_meteor_min;
            }
            else if (track_all) {
                time = ROI_history->array[1][i].time + 1;
                ROI_history->array[0][asso - 1].time = time;
                fra_min = fra_star_min;
            }
            if (is_new_meteor || track_all) {
                if (time == fra_min - 1) {
                    // this loop prevent adding duplicated tracks
                    size_t j = offset_tracks;
                    while (j < *n_tracks && (track_end[j].id != ROI_history->array[1][i].id ||
                           track_end[j].x != ROI_history->array[1][i].x ||
                           track_end[j].y != ROI_history->array[1][i].y))
                        j++;

                    if (j == *n_tracks || *n_tracks == 0) {
                        memcpy(&ROI_list[0], &ROI_history->array[1][i], sizeof(ROI_light_t));
                        for (size_t ii = 1; ii < fra_min - 1; ii++)
                            memcpy(&ROI_list[ii], &ROI_history->array[ii + 1][ROI_list[ii - 1].prev_id - 1],
                                   sizeof(ROI_light_t));
                        _insert_new_track(ROI_list, fra_min - 1, track_id, track_begin, track_end, track_state,
                                          track_obj_type, track_magnitude, n_tracks, BB_array, frame,
                                          is_new_meteor ? METEOR : STAR);
                    }
                }
            }
        }
    }
}

void create_new_tracks(ROI_history_t* ROI_history, ROI_light_t* ROI_list, track_t* track_array, BB_t** BB_array,
                       size_t frame, double mean_error, double std_deviation, float diff_dev, int track_all,
                       size_t fra_star_min, size_t fra_meteor_min) {
    _create_new_tracks(ROI_history, ROI_list, track_array->id, track_array->begin, track_array->end, track_array->state,
                       track_array->obj_type, track_array->magnitude, track_array->_offset, &track_array->_size,
                       BB_array, frame, mean_error, std_deviation, diff_dev, track_all, fra_star_min, fra_meteor_min);
}

void _light_copy_ROI_array(const uint16_t* ROI_src_id, const uint32_t ROI_src_frame, const uint16_t* ROI_src_xmin,
                           const uint16_t* ROI_src_xmax, const uint16_t* ROI_src_ymin, const uint16_t* ROI_src_ymax,
                           const float* ROI_src_x, const float* ROI_src_y, const int32_t* ROI_src_prev_id,
                           const uint32_t* ROI_magnitude, const size_t n_ROI_src, ROI_light_t* ROI_dest) {
    for (size_t i = 0; i < n_ROI_src; i++) {
        ROI_dest[i].id = ROI_src_id[i];
        ROI_dest[i].frame = ROI_src_frame;
        ROI_dest[i].xmin = ROI_src_xmin[i];
        ROI_dest[i].xmax = ROI_src_xmax[i];
        ROI_dest[i].ymin = ROI_src_ymin[i];
        ROI_dest[i].ymax = ROI_src_ymax[i];
        ROI_dest[i].x = ROI_src_x[i];
        ROI_dest[i].y = ROI_src_y[i];
        ROI_dest[i].time = 0;
        ROI_dest[i].time_motion = 0;
        ROI_dest[i].prev_id = ROI_src_prev_id[i];
        ROI_dest[i].is_extrapolated = 0;
        ROI_dest[i].magnitude = ROI_magnitude[i];
    }
}

void light_copy_ROI_array(const ROI_t* ROI_array_src, const uint32_t ROI_src_frame, ROI_light_t* ROI_array_dest) {
    _light_copy_ROI_array(ROI_array_src->id, ROI_src_frame, ROI_array_src->xmin, ROI_array_src->xmax, 
                          ROI_array_src->ymin, ROI_array_src->ymax, ROI_array_src->x, ROI_array_src->y, 
                          ROI_array_src->prev_id, ROI_array_src->magnitude, ROI_array_src->_size, ROI_array_dest);
}

void _update_ROI_array(const float* ROI_src_error, const int32_t* ROI_src_next_id, ROI_light_t* ROI_dest,
                       const size_t n_ROI_dest) {
    for (size_t i = 0; i < n_ROI_dest; i++) {
        ROI_dest[i].next_id = ROI_src_next_id[i];
        ROI_dest[i].error = ROI_src_error[i];
    }
}

void _tracking_perform(tracking_data_t* tracking_data, const float* ROI0_error, const int32_t* ROI0_next_id,
                       const uint16_t* ROI1_id, const uint16_t* ROI1_xmin, const uint16_t* ROI1_xmax,
                       const uint16_t* ROI1_ymin, const uint16_t* ROI1_ymax, const float* ROI1_x, const float* ROI1_y,
                       const int32_t* ROI1_prev_id, const uint32_t* ROI1_magnitude, const size_t n_ROI1,
                       uint16_t* track_id, ROI_light_t* track_begin, ROI_light_t* track_end, float* track_extrapol_x,
                       float* track_extrapol_y, enum state_e* track_state, enum obj_e* track_obj_type,
                       enum change_state_reason_e* track_change_state_reason, int64_t** track_magnitude,
                       size_t* offset_tracks, size_t* n_tracks, BB_t** BB_array, size_t frame, double theta, double tx,
                       double ty, double mean_error, double std_deviation, size_t r_extrapol, float angle_max,
                       float diff_dev, int track_all, size_t fra_star_min, size_t fra_meteor_min,
                       size_t fra_meteor_max) {
    tracking_data->ROI_history->n_ROI[0] = n_ROI1;
    _light_copy_ROI_array(ROI1_id, frame, ROI1_xmin, ROI1_xmax, ROI1_ymin, ROI1_ymax, ROI1_x, ROI1_y, ROI1_prev_id, 
                          ROI1_magnitude, n_ROI1, tracking_data->ROI_history->array[0]);
    if (tracking_data->ROI_history->_size > 0)
        _update_ROI_array(ROI0_error, ROI0_next_id, tracking_data->ROI_history->array[1],
                          tracking_data->ROI_history->n_ROI[1]);
    if (tracking_data->ROI_history->_size < tracking_data->ROI_history->_max_size)
        tracking_data->ROI_history->_size++;
    _create_new_tracks(tracking_data->ROI_history, tracking_data->ROI_list, track_id, track_begin, track_end,
                       track_state, track_obj_type, track_magnitude, *offset_tracks, n_tracks, BB_array, frame,
                       mean_error, std_deviation, diff_dev, track_all, fra_star_min, fra_meteor_min);
    _update_existing_tracks(tracking_data->ROI_history, track_id, track_begin, track_end, track_extrapol_x,
                            track_extrapol_y, track_state, track_obj_type, track_change_state_reason, track_magnitude,
                            offset_tracks, *n_tracks, BB_array, frame, theta, tx, ty, r_extrapol, angle_max, track_all,
                            fra_meteor_max);
    rotate_ROI_history(tracking_data->ROI_history);
    memset(tracking_data->ROI_history->array[0], 0, tracking_data->ROI_history->n_ROI[0] * sizeof(ROI_light_t));
    tracking_data->ROI_history->n_ROI[0] = 0;
}

void tracking_perform(tracking_data_t* tracking_data, const ROI_t* ROI_array0, ROI_t* ROI_array1, track_t* track_array,
                      BB_t** BB_array, size_t frame, double theta, double tx, double ty, double mean_error,
                      double std_deviation, size_t r_extrapol, float angle_max, float diff_dev, int track_all,
                      size_t fra_star_min, size_t fra_meteor_min, size_t fra_meteor_max) {
    _tracking_perform(tracking_data, ROI_array0->error, ROI_array0->next_id, ROI_array1->id, ROI_array1->xmin,
                      ROI_array1->xmax, ROI_array1->ymin, ROI_array1->ymax, ROI_array1->x, ROI_array1->y,
                      ROI_array1->prev_id, ROI_array1->magnitude, ROI_array1->_size, track_array->id,
                      track_array->begin, track_array->end, track_array->extrapol_x, track_array->extrapol_y,
                      track_array->state, track_array->obj_type, track_array->change_state_reason,
                      track_array->magnitude, &track_array->_offset, &track_array->_size, BB_array, frame, theta, tx,
                      ty, mean_error, std_deviation, r_extrapol, angle_max, diff_dev, track_all, fra_star_min,
                      fra_meteor_min, fra_meteor_max);
}

void tracking_print_array_BB(BB_t** BB_array, int n) {
    for (int i = 0; i < n; i++) {
        if (BB_array[i] != NULL) {
            for (BB_t* current = BB_array[i]; current != NULL; current = current->next) {
                printf("%d %d %d %d %d %d %d \n", i, current->rx, current->ry, current->bb_x, current->bb_y,
                       current->track_id, current->is_extrapolated);
            }
        }
    }
}

void _tracking_track_array_write(FILE* f, const uint16_t* track_id, const ROI_light_t* track_begin,
                                 const ROI_light_t* track_end, const enum obj_e* track_obj_type,
                                 const size_t n_tracks) {
    size_t real_n_tracks = 0;
    for (size_t i = 0; i < n_tracks; i++)
        if (track_id[i])
            real_n_tracks++;

    fprintf(f, "# Tracks [%lu]:\n", (unsigned long)real_n_tracks);
    fprintf(f, "# -------||---------------------------||---------------------------||---------\n");
    fprintf(f, "#  Track ||           Begin           ||            End            ||  Object \n");
    fprintf(f, "# -------||---------------------------||---------------------------||---------\n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------\n");
    fprintf(f, "#     Id || Frame # |      x |      y || Frame # |      x |      y ||    Type \n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------\n");

    for (size_t i = 0; i < n_tracks; i++)
        if (track_id[i]) {
            fprintf(f, "   %5d || %7u | %6.1f | %6.1f || %7u | %6.1f | %6.1f || %s \n", track_id[i],
                    track_begin[i].frame, track_begin[i].x, track_begin[i].y, track_end[i].frame, track_end[i].x,
                    track_end[i].y, g_obj_to_string_with_spaces[track_obj_type[i]]);
        }
}

void tracking_track_array_write(FILE* f, const track_t* track_array) {
    _tracking_track_array_write(f, track_array->id, track_array->begin, track_array->end, track_array->obj_type,
                                track_array->_size);
}

void _tracking_track_array_write_full(FILE* f, const uint16_t* track_id, const ROI_light_t* track_begin,
                                      const ROI_light_t* track_end, const enum obj_e* track_obj_type,
                                      const enum change_state_reason_e* track_change_state_reason,
                                      const size_t n_tracks) {
    size_t real_n_tracks = 0;
    for (size_t i = 0; i < n_tracks; i++)
        if (track_id[i])
            real_n_tracks++;
    fprintf(f, "# Tracks [%lu]:\n", (unsigned long)real_n_tracks);
    fprintf(f, "# -------||---------------------------||---------------------------||---------||-------------------\n");
    fprintf(f, "#  Track ||           Begin           ||            End            ||  Object || Reason of changed \n");
    fprintf(f, "# -------||---------------------------||---------------------------||---------||    state (from    \n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------||  meteor to noise  \n");
    fprintf(f, "#     Id || Frame # |      x |      y || Frame # |      x |      y ||    Type ||    object only)   \n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------||-------------------\n");

    for (size_t i = 0; i < n_tracks; i++)
        if (track_id[i]) {
            char reason[64] = "               --";
            if (track_obj_type[i] == NOISE)
                snprintf(reason, sizeof(reason), "%s",
                    g_change_state_to_string_with_spaces[track_change_state_reason[i]]);
            fprintf(f, "   %5d || %7u | %6.1f | %6.1f || %7u | %6.1f | %6.1f || %s || %s \n", track_id[i],
                    track_begin[i].frame, track_begin[i].x, track_begin[i].y, track_end[i].frame, track_end[i].x,
                    track_end[i].y, g_obj_to_string_with_spaces[track_obj_type[i]], reason);
        }
}

void tracking_track_array_write_full(FILE* f, const track_t* track_array) {
    _tracking_track_array_write_full(f, track_array->id, track_array->begin, track_array->end, track_array->obj_type,
                                     track_array->change_state_reason, track_array->_size);
}

void _tracking_track_array_magnitude_write(FILE* f, const uint16_t* track_id, const enum obj_e* track_obj_type,
                                           const int64_t** track_magnitude, const size_t n_tracks) {
    size_t real_n_tracks = 0;
    for (size_t i = 0; i < n_tracks; i++)
        if (track_id[i])
            real_n_tracks++;

    for (size_t i = 0; i < n_tracks; i++)
        if (track_id[i]) {
            fprintf(f, " %5d %s ", track_id[i], g_obj_to_string_with_spaces[track_obj_type[i]]);
            uint32_t j = 0;
            while (track_magnitude[i][j] >= 0)
                fprintf(f, " %5lld ", track_magnitude[i][j++]);
            fprintf(f, "\n");
        }
}

void tracking_track_array_magnitude_write(FILE* f, const track_t* track_array) {
    _tracking_track_array_magnitude_write(f, track_array->id, track_array->obj_type,
                                          (const int64_t**)track_array->magnitude, track_array->_size);
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
            track_array->begin[track_array->_size].frame = t0;
            track_array->end[track_array->_size].frame = t1;
            track_array->state[track_array->_size] = TRACK_FINISHED;
            track_array->begin[track_array->_size].x = x0;
            track_array->begin[track_array->_size].y = y0;
            track_array->end[track_array->_size].x = x1;
            track_array->end[track_array->_size].y = y1;
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
                    fprintf(f, "%d %d %d %d %d %d %d \n", i, current->rx, current->ry, current->bb_x, current->bb_y,
                            current->track_id, current->is_extrapolated);
            }
        }
    }

    fclose(f);
}
