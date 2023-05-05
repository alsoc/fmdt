#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "fmdt/tools.h"
#include "fmdt/macros.h"
#include "vec.h"

#include "fmdt/tracking/tracking_compute.h"

void _tracking_clear_index_track_array(vec_track_t* track_array, const size_t t) {
    if ((*track_array)[t].RoIs_id != NULL)
        vector_free((*track_array)[t].RoIs_id);
    vector_remove(track_array, t);
}

RoIs_history_t* alloc_RoIs_history(const size_t max_history_size, const size_t max_RoIs_size) {
    RoIs_history_t* RoIs_hist = (RoIs_history_t*)malloc(sizeof(RoIs_history_t));
    RoIs_hist->_max_size = max_history_size;
    RoIs_hist->array = (RoI_t**)malloc(RoIs_hist->_max_size * sizeof(RoI_t*));
    RoIs_hist->n_RoIs = (uint32_t*)malloc(RoIs_hist->_max_size * sizeof(uint32_t));
    RoIs_hist->motion = (motion_t*)malloc(RoIs_hist->_max_size * sizeof(motion_t));
    RoIs_hist->_max_n_RoIs = max_RoIs_size;
    RoIs_hist->_size = 0;
    for (size_t i = 0; i < RoIs_hist->_max_size; i++) {
        RoIs_hist->array[i] = (RoI_t*)malloc(max_RoIs_size * sizeof(RoI_t));
        RoIs_hist->n_RoIs[i] = 0;
        memset(&RoIs_hist->motion[i], 0, sizeof(motion_t));
        for (size_t j = 0; j < max_RoIs_size; j++) {
            memset(&RoIs_hist->array[i][j], 0, sizeof(RoI_t));
            RoIs_hist->array[i][j].x = NAN;
            RoIs_hist->array[i][j].y = NAN;
        }
    }
    return RoIs_hist;
}

void free_RoIs_history(RoIs_history_t* RoIs_hist) {
    for (size_t i = 0; i < RoIs_hist->_max_size; i++)
        free(RoIs_hist->array[i]);
    free(RoIs_hist->array);
    free(RoIs_hist->n_RoIs);
    free(RoIs_hist->motion);
    free(RoIs_hist);
}

void rotate_RoIs_history(RoIs_history_t* RoIs_hist) {
    RoI_t* last_RoIs_tmp = RoIs_hist->array[RoIs_hist->_max_size -1];
    uint32_t last_n_RoIs_tmp = RoIs_hist->n_RoIs[RoIs_hist->_max_size -1];
    motion_t last_motion_tmp = RoIs_hist->motion[RoIs_hist->_max_size -1];
    for (int i = (int)(RoIs_hist->_max_size -2); i >= 0; i--) {
        RoIs_hist->array[i + 1] = RoIs_hist->array[i];
        RoIs_hist->n_RoIs[i + 1] = RoIs_hist->n_RoIs[i];
        RoIs_hist->motion[i + 1] = RoIs_hist->motion[i];
    }
    RoIs_hist->array[0] = last_RoIs_tmp;
    RoIs_hist->n_RoIs[0] = last_n_RoIs_tmp;
    RoIs_hist->motion[0] = last_motion_tmp;
}


tracking_data_t* tracking_alloc_data(const size_t max_history_size, const size_t max_RoIs_size) {
    tracking_data_t* tracking_data = (tracking_data_t*)malloc(sizeof(tracking_data_t));
    tracking_data->tracks = (vec_track_t)vector_create();
    tracking_data->RoIs_history = alloc_RoIs_history(max_history_size, max_RoIs_size);
    tracking_data->RoIs_list = (RoI_t*)malloc(max_history_size * sizeof(RoI_t));
    return tracking_data;
}

void tracking_init_data(tracking_data_t* tracking_data) {
    memset(tracking_data->RoIs_list, 0, tracking_data->RoIs_history->_max_size * sizeof(RoI_t));
    for (size_t i = 0; i < tracking_data->RoIs_history->_max_size; i++) {
        memset(tracking_data->RoIs_history->array[i], 0, tracking_data->RoIs_history->_max_n_RoIs *
               sizeof(RoI_t));
        tracking_data->RoIs_history->n_RoIs[i] = 0;
    }
    tracking_data->RoIs_history->_size = 0;
}

void tracking_free_data(tracking_data_t* tracking_data) {
    int vs = vector_size(tracking_data->tracks);
    for (int t = 0; t < vs; t++)
        if (tracking_data->tracks[t].RoIs_id != NULL)
            vector_free(tracking_data->tracks[t].RoIs_id);
    vector_free(tracking_data->tracks);
    free_RoIs_history(tracking_data->RoIs_history);
    free(tracking_data->RoIs_list);
    free(tracking_data);
}

void _compute_angle_and_norms(const RoIs_history_t* RoIs_history, const track_t* cur_track, float *angle_degree,
                              float *norm_u, float *norm_v) {
    int next_id = RoIs_history->array[1][cur_track->end.id - 1].next_id;

    float x0_0 = RoIs_history->array[0][next_id - 1].x;
    float y0_0 = RoIs_history->array[0][next_id - 1].y;

    int k = RoIs_history->array[1][cur_track->end.id - 1].prev_id - 1;

    float x1_1, y1_1;
    if (k == -1) {
        assert(cur_track->extrapol_x1 == cur_track->end.x);
        assert(cur_track->extrapol_y1 == cur_track->end.y);

        x1_1 = cur_track->extrapol_x1;
        y1_1 = cur_track->extrapol_y1;
    } else {
        assert(RoIs_history->array[1][cur_track->end.id - 1].x == cur_track->end.x);
        assert(RoIs_history->array[1][cur_track->end.id - 1].y == cur_track->end.y);

        x1_1 = RoIs_history->array[1][cur_track->end.id - 1].x;
        y1_1 = RoIs_history->array[1][cur_track->end.id - 1].y;
    }

    float x2_2, y2_2;
    if (k == -1) {
        x2_2 = cur_track->extrapol_x2;
        y2_2 = cur_track->extrapol_y2;
    } else {
        x2_2 = RoIs_history->array[2][k].x;
        y2_2 = RoIs_history->array[2][k].y;
    }

    float theta0 = RoIs_history->motion[0].theta;
    float tx0 = RoIs_history->motion[0].tx;
    float ty0 = RoIs_history->motion[0].ty;

    float theta1 = RoIs_history->motion[1].theta;
    float tx1 = RoIs_history->motion[1].tx;
    float ty1 = RoIs_history->motion[1].ty;

    float x0_2 = cosf(theta0 + theta1) * (x0_0 - (tx1 + tx0)) + sinf(theta0 + theta1) * (y0_0 - (ty1 + ty0));
    float y0_2 = cosf(theta0 + theta1) * (y0_0 - (ty1 + ty0)) - sinf(theta0 + theta1) * (x0_0 - (tx1 + tx0));

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

// Sometimes when the camera moves very fast, there are no associations being made, so the motion cannot be estimated
// and its value become a NaN...
// To avoid this problem, the `search_motion` function will select an older motion (without NaN...)
// Note that this is a "hot fix" and it could be improved later.
size_t _search_valid_motion_id(const RoIs_history_t* RoIs_history, const size_t start_index) {
    assert(start_index < RoIs_history->_size);
    for (size_t i = start_index; i < RoIs_history->_size; i++) {
        if (!isnan(RoIs_history->motion[i].tx) && !isnan(RoIs_history->motion[i].ty))
            return i;
    }
    // if we arrive here we have a problem...
    return 0;
}

// Returns 0 if no RoI matches or returns the RoI id found (RoI id >= 1)
size_t _find_and_match_next_RoI(RoIs_history_t* RoIs_history, track_t* cur_track, const size_t r_extrapol,
                                const float min_extrapol_ratio_S) {
    for (size_t j = 0; j < RoIs_history->n_RoIs[0]; j++) {
        if (!RoIs_history->array[0][j].prev_id && !RoIs_history->array[0][j].is_extrapolated) {
            size_t motion_id = _search_valid_motion_id(RoIs_history, 0);

            float theta = RoIs_history->motion[motion_id].theta;
            float tx = RoIs_history->motion[motion_id].tx;
            float ty = RoIs_history->motion[motion_id].ty;

            float x0_0 = RoIs_history->array[0][j].x;
            float y0_0 = RoIs_history->array[0][j].y;

            // motion compensation from t - 1 to t
            float x1_0 = tx + cur_track->extrapol_x1 * cosf(theta) - cur_track->extrapol_y1 * sinf(theta);
            float y1_0 = ty + cur_track->extrapol_x1 * sinf(theta) + cur_track->extrapol_y1 * cosf(theta);

            float x_diff = x0_0 - (x1_0 + cur_track->extrapol_dx);
            float y_diff = y0_0 - (y1_0 + cur_track->extrapol_dy);
            float dist = sqrtf(x_diff * x_diff + y_diff * y_diff);

            float ratio_S_ij = cur_track->end.S < RoIs_history->array[0][j].S ?
                               (float)cur_track->end.S / (float)RoIs_history->array[0][j].S :
                               (float)RoIs_history->array[0][j].S / (float)cur_track->end.S;

            if (dist < r_extrapol && ratio_S_ij >= min_extrapol_ratio_S) {
                RoIs_history->array[0][j].is_extrapolated = 1;
                memcpy(&cur_track->end, &RoIs_history->array[0][j], sizeof(RoI_t));

                // in the current implementation, the first RoI that matches is used for extrapolation
                // TODO: this behavior is dangerous, we should associate the closest RoI
                return j + 1;
            }
        }
    }
    return 0;
}

void _track_extrapolate(const RoIs_history_t* RoIs_history, track_t* cur_track) {
    size_t motion_id = _search_valid_motion_id(RoIs_history, 0);

    float theta = RoIs_history->motion[motion_id].theta;
    float tx = RoIs_history->motion[motion_id].tx;
    float ty = RoIs_history->motion[motion_id].ty;

    float x1_1 = cur_track->extrapol_x1;
    float y1_1 = cur_track->extrapol_y1;

    // motion compensation from t - 1 to t
    float x1_0 = tx + x1_1 * cosf(theta) - y1_1 * sinf(theta);
    float y1_0 = ty + x1_1 * sinf(theta) + y1_1 * cosf(theta);

    cur_track->extrapol_x2 = x1_1;
    cur_track->extrapol_y2 = y1_1;

    // extrapolate x0 and y0 @ t
    cur_track->extrapol_x1 = x1_0 + cur_track->extrapol_dx;
    cur_track->extrapol_y1 = y1_0 + cur_track->extrapol_dy;
}

void _update_extrapol_vars(const RoIs_history_t* RoIs_history, track_t* cur_track) {
    cur_track->extrapol_x2 = cur_track->extrapol_x1;
    cur_track->extrapol_y2 = cur_track->extrapol_y1;
    cur_track->extrapol_x1 = cur_track->end.x;
    cur_track->extrapol_y1 = cur_track->end.y;

    size_t motion_id = _search_valid_motion_id(RoIs_history, 0);

    float theta = RoIs_history->motion[motion_id].theta;
    float tx = RoIs_history->motion[motion_id].tx;
    float ty = RoIs_history->motion[motion_id].ty;

    float x2_2 = cur_track->extrapol_x2;
    float y2_2 = cur_track->extrapol_y2;

    // motion compensation from t - 2 to t - 1
    float x2_1 = tx + x2_2 * cosf(theta) - y2_2 * sinf(theta);
    float y2_1 = ty + x2_2 * sinf(theta) + y2_2 * cosf(theta);

    float x1_1 = cur_track->extrapol_x1;
    float y1_1 = cur_track->extrapol_y1;

    cur_track->extrapol_dx = x1_1 - x2_1;
    cur_track->extrapol_dy = y1_1 - y2_1;
}

void _update_existing_tracks(RoIs_history_t* RoIs_history, vec_track_t track_array, const size_t frame,
                             const size_t r_extrapol, const float angle_max, const int track_all,
                             const size_t fra_meteor_max, const uint8_t extrapol_order_max,
                             const float min_extrapol_ratio_S) {
    size_t n_tracks = vector_size(track_array);
    for (size_t i = 0; i < n_tracks; i++) {
        track_t* cur_track = &track_array[i];
        if (cur_track->id && cur_track->state != STATE_FINISHED) {
            if (cur_track->state == STATE_LOST) {
                if (size_t j = _find_and_match_next_RoI(RoIs_history, cur_track, r_extrapol, min_extrapol_ratio_S)) {
                    cur_track->state = STATE_UPDATED;
                    if (cur_track->RoIs_id != NULL) {
                        // no RoI id when the RoI has been extrapolated
                        for (uint8_t e = cur_track->extrapol_order; e >= 1; e--)
                            vector_add(&cur_track->RoIs_id, (uint32_t)0);
                        vector_add(&cur_track->RoIs_id, RoIs_history->array[0][j - 1].id);
                    }
                    cur_track->extrapol_order = 0;
                    _update_extrapol_vars(RoIs_history, cur_track);
                }
            }
            else if (cur_track->state == STATE_UPDATED) {
                int next_id = RoIs_history->array[1][cur_track->end.id - 1].next_id;
                if (next_id) {
                    if (cur_track->obj_type == OBJ_METEOR) {
                        float norm_u, norm_v, angle_degree;
                        _compute_angle_and_norms(RoIs_history, cur_track, &angle_degree, &norm_u, &norm_v);
                        if (angle_degree >= angle_max || norm_u > norm_v) {
                            cur_track->change_state_reason = (angle_degree >= angle_max) ?
                                REASON_TOO_BIG_ANGLE : REASON_WRONG_DIRECTION;
                            cur_track->obj_type = OBJ_NOISE;
                            if (!track_all) {
                                cur_track->id = 0; // clear_index_track_array
                                continue;
                            }
                        }
                    }
                    memcpy(&cur_track->end, &RoIs_history->array[0][next_id - 1], sizeof(RoI_t));
                    _update_extrapol_vars(RoIs_history, cur_track);

                    if (cur_track->RoIs_id != NULL)
                        vector_add(&cur_track->RoIs_id, RoIs_history->array[0][next_id - 1].id);
                } else {
                    if (size_t j = _find_and_match_next_RoI(RoIs_history, cur_track, r_extrapol, min_extrapol_ratio_S))
                    {
                        if (cur_track->RoIs_id != NULL)
                            vector_add(&cur_track->RoIs_id, RoIs_history->array[0][j - 1].id);
                        _update_extrapol_vars(RoIs_history, cur_track);
                    } else {
                        cur_track->state = STATE_LOST;
                    }
                }
            }
            if (cur_track->state == STATE_LOST) {
                cur_track->extrapol_order++;
                if (cur_track->extrapol_order > extrapol_order_max) {
                    cur_track->state = STATE_FINISHED;
                } else { // on extrapole si pas finished
                    _track_extrapolate(RoIs_history, cur_track);
                }
            }
            if (cur_track->obj_type == OBJ_METEOR &&
                _tracking_get_track_time(cur_track->begin, cur_track->end) >= fra_meteor_max) {
                cur_track->obj_type = OBJ_NOISE;
                cur_track->change_state_reason = REASON_TOO_LONG_DURATION;
                if (!track_all) {
                    cur_track->id = 0; // clear_index_track_array
                    continue;
                }
            }
        }
    }
}

void _insert_new_track(const RoI_t* RoIs_list, const unsigned n_RoIs, vec_track_t* track_array, const int frame,
                       const enum obj_e type, const uint8_t save_RoIs_id) {
    assert(n_RoIs >= 1);

    size_t track_id = vector_size(*track_array) + 1;
    track_t* tmp_track = vector_add_asg(track_array);
    tmp_track->id = track_id;
    memcpy(&tmp_track->begin, &RoIs_list[n_RoIs - 1], sizeof(RoI_t));
    memcpy(&tmp_track->end, &RoIs_list[0], sizeof(RoI_t));
    tmp_track->state = STATE_UPDATED;
    tmp_track->obj_type = type;
    tmp_track->RoIs_id = NULL;
    tmp_track->extrapol_x2 = RoIs_list[1].x;
    tmp_track->extrapol_y2 = RoIs_list[1].y;
    tmp_track->extrapol_x1 = RoIs_list[0].x;
    tmp_track->extrapol_y1 = RoIs_list[0].y;
    tmp_track->extrapol_dx = NAN;
    tmp_track->extrapol_dy = NAN;
    tmp_track->extrapol_order = 0;
    if (save_RoIs_id) {
        tmp_track->RoIs_id = (vec_uint32_t)vector_create();
        for (unsigned n = 0; n < n_RoIs; n++)
            vector_add(&tmp_track->RoIs_id, RoIs_list[(n_RoIs - 1) - n].id);
    }
    tmp_track = NULL; // stop using temp now that the element is initialized
}

void _create_new_tracks(RoIs_history_t* RoIs_history, RoI_t* RoIs_list, vec_track_t* track_array, const size_t frame,
                        const float diff_dev, const int track_all, const size_t fra_star_min,
                        const size_t fra_meteor_min, const uint8_t save_RoIs_id) {
    for (size_t i = 0; i < RoIs_history->n_RoIs[1]; i++) {
        int asso = RoIs_history->array[1][i].next_id;
        if (asso) {
            float e = RoIs_history->array[0][asso - 1].error;
            int is_new_meteor = 0;
            // if motion detected
            if (fabs(e - RoIs_history->motion[0].mean_error) > diff_dev * RoIs_history->motion[0].std_deviation) {
                if (RoIs_history->array[1][i].is_extrapolated)
                    continue; // Extrapolated
                is_new_meteor = 1;
            } // else it is a new star

            int fra_min;
            int time;
            if (is_new_meteor) {
                time = RoIs_history->array[1][i].time_motion + 1;
                RoIs_history->array[0][asso - 1].time_motion = time;
                fra_min = fra_meteor_min;
            }
            else if (track_all) {
                time = RoIs_history->array[1][i].time + 1;
                RoIs_history->array[0][asso - 1].time = time;
                fra_min = fra_star_min;
            }
            if (is_new_meteor || track_all) {
                if (time == fra_min - 1) {
                    // this loop prevent adding duplicated tracks
                    size_t n_tracks = vector_size(*track_array);
                    size_t j = 0;
                    while (j < n_tracks && ((*track_array)[j].end.id != RoIs_history->array[1][i].id ||
                           (*track_array)[j].end.x != RoIs_history->array[1][i].x ||
                           (*track_array)[j].end.y != RoIs_history->array[1][i].y))
                        j++;

                    if (j == n_tracks || n_tracks == 0) {
                        memcpy(&RoIs_list[0], &RoIs_history->array[1][i], sizeof(RoI_t));

                        for (int ii = 1; ii < fra_min - 1; ii++)
                            memcpy(&RoIs_list[ii], &RoIs_history->array[ii + 1][RoIs_list[ii - 1].prev_id - 1],
                                   sizeof(RoI_t));
                        _insert_new_track(RoIs_list, fra_min - 1, track_array, frame,
                                          is_new_meteor ? OBJ_METEOR : OBJ_STAR, save_RoIs_id);
                    }
                }
            }
        }
    }
}

void _light_copy_RoIs(const uint32_t* RoIs_src_id, const uint32_t frame, const uint32_t* RoIs_src_xmin,
                      const uint32_t* RoIs_src_xmax, const uint32_t* RoIs_src_ymin, const uint32_t* RoIs_src_ymax,
                      const uint32_t* RoIs_src_S, const float* RoIs_src_x, const float* RoIs_src_y,
                      const float* RoIs_src_error, const uint32_t* RoIs_src_prev_id, const size_t n_RoIs_src,
                      RoI_t* RoIs_dst) {
    for (size_t i = 0; i < n_RoIs_src; i++) {
        RoIs_dst[i].id = RoIs_src_id[i];
        RoIs_dst[i].frame = frame;
        RoIs_dst[i].xmin = RoIs_src_xmin[i];
        RoIs_dst[i].xmax = RoIs_src_xmax[i];
        RoIs_dst[i].ymin = RoIs_src_ymin[i];
        RoIs_dst[i].ymax = RoIs_src_ymax[i];
        RoIs_dst[i].S = RoIs_src_S[i];
        RoIs_dst[i].x = RoIs_src_x[i];
        RoIs_dst[i].y = RoIs_src_y[i];
        RoIs_dst[i].error = RoIs_src_error[i];
        RoIs_dst[i].time = 0;
        RoIs_dst[i].time_motion = 0;
        RoIs_dst[i].prev_id = RoIs_src_prev_id[i];
        RoIs_dst[i].is_extrapolated = 0;
    }
}

void light_copy_RoIs(const RoIs_t* RoIs_src, const uint32_t frame, RoI_t* RoIs_dst) {
    _light_copy_RoIs(RoIs_src->id, frame, RoIs_src->basic->xmin, RoIs_src->basic->xmax, RoIs_src->basic->ymin,
                     RoIs_src->basic->ymax, RoIs_src->basic->S, RoIs_src->basic->x, RoIs_src->basic->y,
                     RoIs_src->motion->error, RoIs_src->asso->prev_id, RoIs_src->_size, RoIs_dst);
}

void _update_RoIs_next_id(const uint32_t* RoIs_prev_id, RoI_t* RoIs_dst, const size_t n_RoIs) {
    for (size_t i = 0; i < n_RoIs; i++)
        if (RoIs_prev_id[i])
            RoIs_dst[RoIs_prev_id[i] - 1].next_id = i + 1;
}

void _tracking_perform(tracking_data_t* tracking_data, const uint32_t* RoIs_id, const uint32_t* RoIs_xmin,
                       const uint32_t* RoIs_xmax, const uint32_t* RoIs_ymin, const uint32_t* RoIs_ymax,
                       const uint32_t* RoIs_S, const float* RoIs_x, const float* RoIs_y, const float* RoIs_error,
                       const uint32_t* RoIs_prev_id, const size_t n_RoIs, const size_t frame,
                       const motion_t* motion_est, const size_t r_extrapol, const float angle_max, const float diff_dev,
                       const int track_all, const size_t fra_star_min, const size_t fra_meteor_min,
                       const size_t fra_meteor_max, const uint8_t save_RoIs_id, const uint8_t extrapol_order_max,
                       const float min_extrapol_ratio_S) {
    assert(extrapol_order_max < tracking_data->RoIs_history->_max_size);
    assert(min_extrapol_ratio_S >= 0.f && min_extrapol_ratio_S <= 1.f);

    tracking_data->RoIs_history->n_RoIs[0] = n_RoIs;
    _light_copy_RoIs(RoIs_id, frame, RoIs_xmin, RoIs_xmax, RoIs_ymin, RoIs_ymax, RoIs_S, RoIs_x, RoIs_y, RoIs_error,
                     RoIs_prev_id, n_RoIs, tracking_data->RoIs_history->array[0]);
    tracking_data->RoIs_history->motion[0] = *motion_est;
    if (tracking_data->RoIs_history->_size > 0)
        _update_RoIs_next_id(RoIs_prev_id, tracking_data->RoIs_history->array[1], n_RoIs);
    if (tracking_data->RoIs_history->_size < tracking_data->RoIs_history->_max_size)
        tracking_data->RoIs_history->_size++;

    if (tracking_data->RoIs_history->_size >= 2) {
        _create_new_tracks(tracking_data->RoIs_history, tracking_data->RoIs_list, &tracking_data->tracks, frame,
                           diff_dev, track_all, fra_star_min, fra_meteor_min, save_RoIs_id);
        _update_existing_tracks(tracking_data->RoIs_history, tracking_data->tracks, frame, r_extrapol, angle_max,
                                track_all, fra_meteor_max, extrapol_order_max, min_extrapol_ratio_S);
    }

    rotate_RoIs_history(tracking_data->RoIs_history);
    memset(tracking_data->RoIs_history->array[0], 0, tracking_data->RoIs_history->n_RoIs[0] * sizeof(RoI_t));
    tracking_data->RoIs_history->n_RoIs[0] = 0;
}

void tracking_perform(tracking_data_t* tracking_data, const RoIs_t* RoIs, const size_t frame,
                      const motion_t* motion_est, const size_t r_extrapol, const float angle_max, const float diff_dev,
                      const int track_all, const size_t fra_star_min, const size_t fra_meteor_min,
                      const size_t fra_meteor_max, const uint8_t save_RoIs_id, const uint8_t extrapol_order_max,
                      const float min_extrapol_ratio_S) {
    _tracking_perform(tracking_data, RoIs->id, RoIs->basic->xmin, RoIs->basic->xmax, RoIs->basic->ymin,
                      RoIs->basic->ymax, RoIs->basic->S, RoIs->basic->x, RoIs->basic->y, RoIs->motion->error,
                      RoIs->asso->prev_id, RoIs->_size, frame, motion_est, r_extrapol, angle_max, diff_dev, track_all,
                      fra_star_min, fra_meteor_min, fra_meteor_max, save_RoIs_id, extrapol_order_max,
                      min_extrapol_ratio_S);
}
