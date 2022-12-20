#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmdt/tools.h"
#include "fmdt/macros.h"
#include "fmdt/tracking.h"
#include "vec.h"

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

size_t _tracking_get_track_time(const ROI_light_t track_begin, const ROI_light_t track_end) {
    return track_end.frame - track_begin.frame;
}

size_t tracking_get_track_time(const vec_track_t track_array, const size_t t) {
    return _tracking_get_track_time(track_array[t].begin, track_array[t].end);
}

size_t _tracking_count_objects(const vec_track_t track_array, unsigned* n_stars, unsigned* n_meteors,
                               unsigned* n_noise) {
    size_t n_tracks = vector_size(track_array);
    (*n_stars) = (*n_meteors) = (*n_noise) = 0;
    for (size_t i = 0; i < n_tracks; i++)
        if (track_array[i].id)
            switch (track_array[i].obj_type) {
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
                fprintf(stderr, "(EE) This should never happen ('track_array[i].obj_type = %d', 'i = %lu')\n",
                        track_array[i].obj_type, (unsigned long)i);
                exit(1);
            }
    return (*n_stars) + (*n_meteors) + (*n_noise);
}

size_t tracking_count_objects(const vec_track_t track_array, unsigned* n_stars, unsigned* n_meteors,
                              unsigned* n_noise) {
    return _tracking_count_objects(track_array, n_stars, n_meteors, n_noise);
}

void _tracking_clear_index_track_array(vec_track_t* track_array, const size_t t) {
    if ((*track_array)[t].magnitude != NULL)
        vector_free((*track_array)[t].magnitude);
    vector_remove(track_array, t);
}

ROI_history_t* alloc_ROI_history(const size_t max_history_size, const size_t max_ROI_size) {
    ROI_history_t* ROI_hist = (ROI_history_t*)malloc(sizeof(ROI_history_t));
    ROI_hist->_max_size = max_history_size;
    ROI_hist->array = (ROI_light_t**)malloc(ROI_hist->_max_size * sizeof(ROI_light_t*));
    ROI_hist->n_ROI = (uint32_t*)malloc(ROI_hist->_max_size * sizeof(uint32_t));
    ROI_hist->motion = (motion_t*)malloc(ROI_hist->_max_size * sizeof(motion_t));
    ROI_hist->_max_n_ROI = max_ROI_size;
    ROI_hist->_size = 0;
    for (size_t i = 0; i < ROI_hist->_max_size; i++) {
        ROI_hist->array[i] = (ROI_light_t*)malloc(max_ROI_size * sizeof(ROI_light_t));
        ROI_hist->n_ROI[i] = 0;
        memset(&ROI_hist->motion[i], 0, sizeof(motion_t));
        for (size_t j = 0; j < max_ROI_size; j++) {
            memset(&ROI_hist->array[i][j], 0, sizeof(ROI_light_t));
            ROI_hist->array[i][j].x = NAN;
            ROI_hist->array[i][j].y = NAN;
        }
    }
    return ROI_hist;
}

void free_ROI_history(ROI_history_t* ROI_hist) {
    for (size_t i = 0; i < ROI_hist->_max_size; i++)
        free(ROI_hist->array[i]);
    free(ROI_hist->array);
    free(ROI_hist->n_ROI);
    free(ROI_hist->motion);
    free(ROI_hist);
}

void rotate_ROI_history(ROI_history_t* ROI_hist) {
    ROI_light_t* last_ROI_tmp = ROI_hist->array[ROI_hist->_max_size -1];
    uint32_t last_n_ROI_tmp = ROI_hist->n_ROI[ROI_hist->_max_size -1];
    motion_t last_motion_tmp = ROI_hist->motion[ROI_hist->_max_size -1];
    for (int i = (int)(ROI_hist->_max_size -2); i >= 0; i--) {
        ROI_hist->array[i + 1] = ROI_hist->array[i];
        ROI_hist->n_ROI[i + 1] = ROI_hist->n_ROI[i];
        ROI_hist->motion[i + 1] = ROI_hist->motion[i];
    }
    ROI_hist->array[0] = last_ROI_tmp;
    ROI_hist->n_ROI[0] = last_n_ROI_tmp;
    ROI_hist->motion[0] = last_motion_tmp;
}


tracking_data_t* tracking_alloc_data(const size_t max_history_size, const size_t max_ROI_size) {
    tracking_data_t* tracking_data = (tracking_data_t*)malloc(sizeof(tracking_data_t));
    tracking_data->tracks = (vec_track_t)vector_create();
    tracking_data->ROI_history = alloc_ROI_history(max_history_size, max_ROI_size);
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
    int vs = vector_size(tracking_data->tracks);
    for (int t = 0; t < vs; t++)
        if (tracking_data->tracks[t].magnitude != NULL)
            vector_free(tracking_data->tracks[t].magnitude);
    vector_free(tracking_data->tracks);
    free_ROI_history(tracking_data->ROI_history);
    free(tracking_data->ROI_list);
    free(tracking_data);
}

void _update_bounding_box(vec_BB_t* BB_array, const int track_id, const uint32_t ROI_xmin, const uint32_t ROI_xmax,
                          const uint32_t ROI_ymin, const uint32_t ROI_ymax, int frame, int is_extrapolated) {
    assert(ROI_xmin || ROI_xmax || ROI_ymin || ROI_ymax);

    uint32_t bb_x = (uint32_t)ceilf((float)(ROI_xmin + ROI_xmax) / 2.f);
    uint32_t bb_y = (uint32_t)ceilf((float)(ROI_ymin + ROI_ymax) / 2.f);
    uint32_t rx = (bb_x - ROI_xmin);
    uint32_t ry = (bb_y - ROI_ymin);

    BB_t* tmp_bb = vector_add_asg(BB_array);
    tmp_bb->frame_id = frame;
    tmp_bb->track_id = track_id;
    tmp_bb->rx = rx;
    tmp_bb->ry = ry;
    tmp_bb->bb_x = bb_x;
    tmp_bb->bb_y = bb_y;
    tmp_bb->is_extrapolated = is_extrapolated;
    tmp_bb = NULL; // stop using temp now that the element is initialized
}

void _compute_angle_and_norms(const ROI_history_t* ROI_history, const track_t* cur_track, float *angle_degree,
                              float *norm_u, float *norm_v) {
    int next_id = ROI_history->array[1][cur_track->end.id - 1].next_id;
    int k = ROI_history->array[1][cur_track->end.id - 1].prev_id - 1;

    float x0_0 = ROI_history->array[0][next_id - 1].x;
    float y0_0 = ROI_history->array[0][next_id - 1].y;

    float x1_1 = ROI_history->array[1][cur_track->end.id - 1].x;
    float y1_1 = ROI_history->array[1][cur_track->end.id - 1].y;

    float x2_2 = ROI_history->array[2][k].x;
    float y2_2 = ROI_history->array[2][k].y;

    float theta0 = ROI_history->motion[0].theta;
    float tx0 = ROI_history->motion[0].tx;
    float ty0 = ROI_history->motion[0].ty;

    float x0_1 = cosf(theta0) * (x0_0 - tx0) + sinf(theta0) * (y0_0 - ty0);
    float y0_1 = cosf(theta0) * (y0_0 - ty0) - sinf(theta0) * (x0_0 - tx0);

    float theta1 = ROI_history->motion[1].theta;
    float tx1 = ROI_history->motion[1].tx;
    float ty1 = ROI_history->motion[1].ty;

    float x0_2 = cosf(theta1) * (x0_1 - tx1) + sinf(theta1) * (y0_1 - ty1);
    float y0_2 = cosf(theta1) * (y0_1 - ty1) - sinf(theta1) * (x0_1 - tx1);

    float x1_2 = cosf(theta1) * (x1_1 - tx1) + sinf(theta1) * (y1_1 - ty1);
    float y1_2 = cosf(theta1) * (y1_1 - ty1) - sinf(theta1) * (x1_1 - tx1);

    float u_x = x1_2 - x2_2;
    float u_y = y1_2 - y2_2;
    float v_x = x0_2 - x2_2;
    float v_y = y0_2 - y2_2;

    float scalar_prod_uv = u_x * v_x + u_y * v_y;

    *norm_u = sqrtf(u_x * u_x + u_y * u_y);
    *norm_v = sqrtf(v_x * v_x + v_y * v_y);

    float cos_uv = scalar_prod_uv / (*norm_u * *norm_v);

    float angle_rad = acosf(cos_uv >= 1 ? 0.99999f : cos_uv);
    *angle_degree = angle_rad * (180.f / (float)M_PI);
    // *angle_degree = fmodf(angle_degree, 360.f);
}

void _track_extrapolate(const ROI_history_t* ROI_history, const ROI_light_t* track_end, float* track_extrapol_x,
                        float* track_extrapol_y, float* track_extrapol_u, float* track_extrapol_v, const float theta,
                        const float tx, const float ty, uint8_t extrapol_order) {
    assert(extrapol_order > 0);

    if (extrapol_order == 1) {
        // compensation du mouvement + calcul vitesse entre t-1 et t
        *track_extrapol_u = track_end->x - track_end->dx - *track_extrapol_x;
        *track_extrapol_v = track_end->y - track_end->dy - *track_extrapol_y;
    }

    float x = tx + track_end->x * cosf(theta) - track_end->y * sinf(theta);
    float y = ty + track_end->x * sinf(theta) + track_end->y * cosf(theta);

    *track_extrapol_x = x + (float)extrapol_order * *track_extrapol_u;
    *track_extrapol_y = y + (float)extrapol_order * *track_extrapol_v;
}

void _update_existing_tracks(ROI_history_t* ROI_history, vec_track_t track_array, vec_BB_t* BB_array,
                             const size_t frame, const size_t r_extrapol, const float angle_max, const int track_all,
                             const size_t fra_meteor_max, const uint8_t extrapol_order_max) {
    size_t n_tracks = vector_size(track_array);
    for (size_t i = 0; i < n_tracks; i++) {
        track_t* cur_track = &track_array[i];
        if (cur_track->id && cur_track->state != TRACK_FINISHED) {
            if (cur_track->state == TRACK_LOST) {
                for (size_t j = 0; j < ROI_history->n_ROI[0]; j++) {
                    if (!ROI_history->array[0][j].prev_id) {
                        // test NaN
                        assert(ROI_history->array[0][j].x == ROI_history->array[0][j].x);
                        assert(ROI_history->array[0][j].y == ROI_history->array[0][j].y);
                        assert(cur_track->extrapol_x == cur_track->extrapol_x);
                        assert(cur_track->extrapol_y == cur_track->extrapol_y);

                        float theta = ROI_history->motion[0].theta;
                        float tx = ROI_history->motion[0].tx;
                        float ty = ROI_history->motion[0].ty;

                        float x_0 = ROI_history->array[0][j].x;
                        float y_0 = ROI_history->array[0][j].y;
                        float x_1 = cosf(theta) * (x_0 - tx) + sinf(theta) * (y_0 - ty);
                        float y_1 = cosf(theta) * (y_0 - ty) - sinf(theta) * (x_0 - tx);

                        float x_diff = cur_track->extrapol_x - x_1;
                        float y_diff = cur_track->extrapol_y - y_1;

                        float dist = sqrtf(x_diff * x_diff + y_diff * y_diff);
                        if (dist < r_extrapol) {
                            // save extrapolated bounding boxes
                            if (BB_array != NULL) {
                                size_t motion_id = 1;
                                for (uint8_t e = cur_track->extrapol_order; e >= 1; e--) {
                                    float theta = ROI_history->motion[motion_id + 1].theta;
                                    float tx = ROI_history->motion[motion_id + 1].tx;
                                    float ty = ROI_history->motion[motion_id + 1].ty;

                                    float x = tx + cur_track->end.x * cosf(theta) - cur_track->end.y * sinf(theta);
                                    float y = ty + cur_track->end.x * sinf(theta) + cur_track->end.y * cosf(theta);

                                    float track_extrapol_x = x + (float)e * cur_track->extrapol_u;
                                    float track_extrapol_y = y + (float)e * cur_track->extrapol_v;

                                    uint32_t height_extrapol = cur_track->end.ymax - cur_track->end.ymin;
                                    uint32_t width_extrapol = cur_track->end.xmax - cur_track->end.xmin;
                                    uint32_t xmin_extrapol = track_extrapol_x - (width_extrapol / 2);
                                    uint32_t xmax_extrapol = track_extrapol_x + (width_extrapol / 2);
                                    uint32_t ymin_extrapol = track_extrapol_y - (height_extrapol / 2);
                                    uint32_t ymax_extrapol = track_extrapol_y + (height_extrapol / 2);
                                    size_t vs = vector_size(BB_array);
                                    _update_bounding_box(&BB_array[vs - (motion_id + 1)], cur_track->id, xmin_extrapol,
                                                         xmax_extrapol, ymin_extrapol, ymax_extrapol,
                                                         frame - (motion_id), /* is_extrapolated = */ 1);
                                    motion_id++;
                                }
                            }

                            cur_track->state = TRACK_UPDATED;
                            cur_track->extrapol_order = 0;
                            ROI_history->array[0][j].is_extrapolated = 1;
                            memcpy(&cur_track->end, &ROI_history->array[0][j], sizeof(ROI_light_t));

                            if (cur_track->magnitude != NULL) {
                                // because we don't know the magnitude when ROI has been extrapolated
                                vector_add(&cur_track->magnitude, (uint32_t)0);
                                vector_add(&cur_track->magnitude, ROI_history->array[0][j].magnitude);
                            }

                            if (BB_array != NULL) {
                                size_t vs = vector_size(BB_array);
                                _update_bounding_box(&BB_array[vs - 1], cur_track->id, ROI_history->array[0][j].xmin,
                                                     ROI_history->array[0][j].xmax, ROI_history->array[0][j].ymin,
                                                     ROI_history->array[0][j].ymax, frame, /* is_extrapolated = */ 0);
                            }

                            // in the current implementation, the first ROI that matches is used for extrapolation
                            break;
                        }
                    }
                }
            }
            else if (cur_track->state == TRACK_UPDATED || cur_track->state == TRACK_NEW) {
                int next_id = ROI_history->array[1][cur_track->end.id - 1].next_id;
                if (next_id) {
                    if (cur_track->obj_type == METEOR) {
                        if (ROI_history->array[1][cur_track->end.id - 1].prev_id) {
                            float norm_u, norm_v, angle_degree;
                            _compute_angle_and_norms(ROI_history, cur_track, &angle_degree, &norm_u, &norm_v);
                            if (angle_degree >= angle_max || norm_u > norm_v) {
                                cur_track->change_state_reason = (angle_degree >= angle_max) ?
                                    REASON_TOO_BIG_ANGLE : REASON_WRONG_DIRECTION;
                                cur_track->obj_type = NOISE;
                                if (!track_all) {
                                    cur_track->id = 0; // clear_index_track_array
                                    continue;
                                }
                            }
                        }
                    }
                    cur_track->extrapol_x = cur_track->end.x;
                    cur_track->extrapol_y = cur_track->end.y;

                    memcpy(&cur_track->end, &ROI_history->array[0][next_id - 1], sizeof(ROI_light_t));

                    if (cur_track->state == TRACK_NEW) // because the right time has been set in 'insert_new_track'
                        cur_track->state = TRACK_UPDATED;

                    if (cur_track->magnitude != NULL)
                        vector_add(&cur_track->magnitude, ROI_history->array[0][next_id - 1].magnitude);

                    if (BB_array != NULL) {
                        size_t vs = vector_size(BB_array);
                        _update_bounding_box(&BB_array[vs - 1], cur_track->id,
                                             ROI_history->array[0][next_id - 1].xmin,
                                             ROI_history->array[0][next_id - 1].xmax,
                                             ROI_history->array[0][next_id - 1].ymin,
                                             ROI_history->array[0][next_id - 1].ymax, frame, /* is_extrapolated = */ 0);
                    }
                } else {
                    cur_track->state = TRACK_LOST;
                }
            }
            if (cur_track->state == TRACK_LOST) {
                cur_track->extrapol_order++;
                if (cur_track->extrapol_order > extrapol_order_max)
                    cur_track->state = TRACK_FINISHED;
                else
                    // on extrapole si pas finished
                    _track_extrapolate(ROI_history, &cur_track->end, &cur_track->extrapol_x, &cur_track->extrapol_y,
                                       &cur_track->extrapol_u, &cur_track->extrapol_v, ROI_history->motion[0].theta,
                                       ROI_history->motion[0].tx, ROI_history->motion[0].ty, cur_track->extrapol_order);
            }
            if (cur_track->obj_type == METEOR &&
                _tracking_get_track_time(cur_track->begin, cur_track->end) >= fra_meteor_max) {
                cur_track->obj_type = NOISE;
                cur_track->change_state_reason = REASON_TOO_LONG_DURATION;
                if (!track_all) {
                    cur_track->id = 0; // clear_index_track_array
                    continue;
                }
            }
        }
    }
}

void _insert_new_track(const ROI_light_t* ROI_list, const unsigned n_ROI, vec_track_t* track_array, vec_BB_t* BB_array,
                       const int frame, const enum obj_e type, const int magnitude)
{
    assert(n_ROI >= 1);

    size_t track_id = vector_size(*track_array) + 1;
    track_t* tmp_track = vector_add_asg(track_array);
    tmp_track->id = track_id;
    memcpy(&tmp_track->begin, &ROI_list[n_ROI - 1], sizeof(ROI_light_t));
    memcpy(&tmp_track->end, &ROI_list[0], sizeof(ROI_light_t));
    tmp_track->state = TRACK_NEW;
    tmp_track->obj_type = type;
    tmp_track->magnitude = NULL;
    tmp_track->extrapol_x = NAN;
    tmp_track->extrapol_y = NAN;
    tmp_track->extrapol_order = 0;
    if (magnitude) {
        tmp_track->magnitude = (vec_uint32_t)vector_create();
        for (unsigned n = 0; n < n_ROI; n++)
            vector_add(&tmp_track->magnitude, ROI_list[(n_ROI - 1) - n].magnitude);
    }
    tmp_track = NULL; // stop using temp now that the element is initialized

    if (BB_array != NULL) {
        size_t vs = vector_size(BB_array);
        for (unsigned n = 0; n < n_ROI; n++)
            _update_bounding_box(&BB_array[(vs - 1) - n - 1], track_id, ROI_list[n].xmin, ROI_list[n].xmax,
                                 ROI_list[n].ymin, ROI_list[n].ymax, (frame - n) - 1, /* is_extrapolated = */ 0);
    }
}

void _create_new_tracks(ROI_history_t* ROI_history, ROI_light_t* ROI_list, vec_track_t* track_array, vec_BB_t* BB_array,
                        const size_t frame, const float diff_dev, const int track_all, const size_t fra_star_min,
                        const size_t fra_meteor_min, const int magnitude)
{
    for (size_t i = 0; i < ROI_history->n_ROI[1]; i++) {
        int asso = ROI_history->array[1][i].next_id;
        float e = ROI_history->array[0][asso - 1].error;
        if (asso) {
            int is_new_meteor = 0;
            // if motion detected
            if (fabs(e - ROI_history->motion[0].mean_error) > diff_dev * ROI_history->motion[0].std_deviation) {
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
                    size_t n_tracks = vector_size(*track_array);
                    size_t j = 0;
                    while (j < n_tracks && ((*track_array)[j].end.id != ROI_history->array[1][i].id ||
                           (*track_array)[j].end.x != ROI_history->array[1][i].x ||
                           (*track_array)[j].end.y != ROI_history->array[1][i].y))
                        j++;

                    if (j == n_tracks || n_tracks == 0) {
                        memcpy(&ROI_list[0], &ROI_history->array[1][i], sizeof(ROI_light_t));
                        for (int ii = 1; ii < fra_min - 1; ii++)
                            memcpy(&ROI_list[ii], &ROI_history->array[ii + 1][ROI_list[ii - 1].prev_id - 1],
                                   sizeof(ROI_light_t));
                        _insert_new_track(ROI_list, fra_min - 1, track_array, BB_array, frame,
                                          is_new_meteor ? METEOR : STAR, magnitude);
                    }
                }
            }
        }
    }
}

void _light_copy_ROI_array(const uint32_t* ROI_src_id, const uint32_t ROI_src_frame, const uint32_t* ROI_src_xmin,
                           const uint32_t* ROI_src_xmax, const uint32_t* ROI_src_ymin, const uint32_t* ROI_src_ymax,
                           const float* ROI_src_x, const float* ROI_src_y, const float* ROI_src_error,
                           const uint32_t* ROI_src_prev_id, const uint32_t* ROI_magnitude, const size_t n_ROI_src,
                           ROI_light_t* ROI_dest) {
    for (size_t i = 0; i < n_ROI_src; i++) {
        ROI_dest[i].id = ROI_src_id[i];
        ROI_dest[i].frame = ROI_src_frame;
        ROI_dest[i].xmin = ROI_src_xmin[i];
        ROI_dest[i].xmax = ROI_src_xmax[i];
        ROI_dest[i].ymin = ROI_src_ymin[i];
        ROI_dest[i].ymax = ROI_src_ymax[i];
        ROI_dest[i].x = ROI_src_x[i];
        ROI_dest[i].y = ROI_src_y[i];
        ROI_dest[i].error = ROI_src_error[i];
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
                          ROI_array_src->error, ROI_array_src->prev_id, ROI_array_src->magnitude, ROI_array_src->_size,
                          ROI_array_dest);
}

void _update_ROI_array_next_id(const uint32_t* ROI_prev_id, ROI_light_t* ROI_dest, const size_t n_ROI) {
    for (size_t i = 0; i < n_ROI; i++)
        if (ROI_prev_id[i])
            ROI_dest[ROI_prev_id[i] - 1].next_id = i + 1;
}

void _tracking_perform(tracking_data_t* tracking_data, const uint32_t* ROI_id, const uint32_t* ROI_xmin,
                       const uint32_t* ROI_xmax, const uint32_t* ROI_ymin, const uint32_t* ROI_ymax,
                       const float* ROI_x, const float* ROI_y, const float* ROI_error, const uint32_t* ROI_prev_id,
                       const uint32_t* ROI_magnitude, const size_t n_ROI1, vec_BB_t** BB_array, const size_t frame,
                       const motion_t* motion_est, const size_t r_extrapol, const float angle_max, const float diff_dev,
                       const int track_all, const size_t fra_star_min, const size_t fra_meteor_min,
                       const size_t fra_meteor_max, const int magnitude, const uint8_t extrapol_order_max) {
    assert(extrapol_order_max < tracking_data->ROI_history->_max_size);

    if (*BB_array != NULL) {
        vec_BB_t new_BB = (vec_BB_t)vector_create();
        vector_add(BB_array, new_BB);
    }

    tracking_data->ROI_history->n_ROI[0] = n_ROI1;
    _light_copy_ROI_array(ROI_id, frame, ROI_xmin, ROI_xmax, ROI_ymin, ROI_ymax, ROI_x, ROI_y, ROI_error,
                          ROI_prev_id, ROI_magnitude, n_ROI1, tracking_data->ROI_history->array[0]);
    tracking_data->ROI_history->motion[0] = *motion_est;
    if (tracking_data->ROI_history->_size > 0)
        _update_ROI_array_next_id(ROI_prev_id, tracking_data->ROI_history->array[1], n_ROI1);
    if (tracking_data->ROI_history->_size < tracking_data->ROI_history->_max_size)
        tracking_data->ROI_history->_size++;

    if (tracking_data->ROI_history->_size >= 2) {
        _create_new_tracks(tracking_data->ROI_history, tracking_data->ROI_list, &tracking_data->tracks, *BB_array,
                           frame, diff_dev, track_all, fra_star_min, fra_meteor_min, magnitude);
        _update_existing_tracks(tracking_data->ROI_history, tracking_data->tracks, *BB_array, frame, r_extrapol,
                                angle_max, track_all, fra_meteor_max, extrapol_order_max);
    }

    rotate_ROI_history(tracking_data->ROI_history);
    memset(tracking_data->ROI_history->array[0], 0, tracking_data->ROI_history->n_ROI[0] * sizeof(ROI_light_t));
    tracking_data->ROI_history->n_ROI[0] = 0;
}

void tracking_perform(tracking_data_t* tracking_data, const ROI_t* ROI_array, vec_BB_t** BB_array, const size_t frame,
                      const motion_t* motion_est, const size_t r_extrapol, const float angle_max, const float diff_dev,
                      const int track_all, const size_t fra_star_min, const size_t fra_meteor_min,
                      const size_t fra_meteor_max, const int magnitude, const uint8_t extrapol_order_max) {
    _tracking_perform(tracking_data, ROI_array->id, ROI_array->xmin, ROI_array->xmax, ROI_array->ymin,
                      ROI_array->ymax, ROI_array->x, ROI_array->y, ROI_array->error, ROI_array->prev_id,
                      ROI_array->magnitude, ROI_array->_size, BB_array, frame, motion_est, r_extrapol, angle_max,
                      diff_dev, track_all, fra_star_min, fra_meteor_min, fra_meteor_max, magnitude, extrapol_order_max);
}

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
