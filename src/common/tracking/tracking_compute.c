#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fmdt/tools.h"
#include "fmdt/macros.h"
#include "vec.h"

#include "fmdt/tracking/tracking_compute.h"

void _tracking_clear_index_track_array(vec_track_t* track_array, const size_t t) {
    if ((*track_array)[t].magnitude != NULL)
        vector_free((*track_array)[t].magnitude);
    vector_remove(track_array, t);
}

RoI_history_t* alloc_RoI_history(const size_t max_history_size, const size_t max_RoI_size) {
    RoI_history_t* RoI_hist = (RoI_history_t*)malloc(sizeof(RoI_history_t));
    RoI_hist->_max_size = max_history_size;
    RoI_hist->array = (RoI_track_t**)malloc(RoI_hist->_max_size * sizeof(RoI_track_t*));
    RoI_hist->n_RoI = (uint32_t*)malloc(RoI_hist->_max_size * sizeof(uint32_t));
    RoI_hist->motion = (motion_t*)malloc(RoI_hist->_max_size * sizeof(motion_t));
    RoI_hist->_max_n_RoI = max_RoI_size;
    RoI_hist->_size = 0;
    for (size_t i = 0; i < RoI_hist->_max_size; i++) {
        RoI_hist->array[i] = (RoI_track_t*)malloc(max_RoI_size * sizeof(RoI_track_t));
        RoI_hist->n_RoI[i] = 0;
        memset(&RoI_hist->motion[i], 0, sizeof(motion_t));
        for (size_t j = 0; j < max_RoI_size; j++) {
            memset(&RoI_hist->array[i][j], 0, sizeof(RoI_track_t));
            RoI_hist->array[i][j].x = NAN;
            RoI_hist->array[i][j].y = NAN;
        }
    }
    return RoI_hist;
}

void free_RoI_history(RoI_history_t* RoI_hist) {
    for (size_t i = 0; i < RoI_hist->_max_size; i++)
        free(RoI_hist->array[i]);
    free(RoI_hist->array);
    free(RoI_hist->n_RoI);
    free(RoI_hist->motion);
    free(RoI_hist);
}

void rotate_RoI_history(RoI_history_t* RoI_hist) {
    RoI_track_t* last_RoI_tmp = RoI_hist->array[RoI_hist->_max_size -1];
    uint32_t last_n_RoI_tmp = RoI_hist->n_RoI[RoI_hist->_max_size -1];
    motion_t last_motion_tmp = RoI_hist->motion[RoI_hist->_max_size -1];
    for (int i = (int)(RoI_hist->_max_size -2); i >= 0; i--) {
        RoI_hist->array[i + 1] = RoI_hist->array[i];
        RoI_hist->n_RoI[i + 1] = RoI_hist->n_RoI[i];
        RoI_hist->motion[i + 1] = RoI_hist->motion[i];
    }
    RoI_hist->array[0] = last_RoI_tmp;
    RoI_hist->n_RoI[0] = last_n_RoI_tmp;
    RoI_hist->motion[0] = last_motion_tmp;
}


tracking_data_t* tracking_alloc_data(const size_t max_history_size, const size_t max_RoI_size) {
    tracking_data_t* tracking_data = (tracking_data_t*)malloc(sizeof(tracking_data_t));
    tracking_data->tracks = (vec_track_t)vector_create();
    tracking_data->RoI_history = alloc_RoI_history(max_history_size, max_RoI_size);
    tracking_data->RoI_list = (RoI_track_t*)malloc(max_history_size * sizeof(RoI_track_t));
    return tracking_data;
}

void tracking_init_data(tracking_data_t* tracking_data) {
    memset(tracking_data->RoI_list, 0, tracking_data->RoI_history->_max_size * sizeof(RoI_track_t));
    for (size_t i = 0; i < tracking_data->RoI_history->_max_size; i++) {
        memset(tracking_data->RoI_history->array[i], 0, tracking_data->RoI_history->_max_n_RoI * sizeof(RoI_track_t));
        tracking_data->RoI_history->n_RoI[i] = 0;
    }
    tracking_data->RoI_history->_size = 0;
}

void tracking_free_data(tracking_data_t* tracking_data) {
    int vs = vector_size(tracking_data->tracks);
    for (int t = 0; t < vs; t++)
        if (tracking_data->tracks[t].magnitude != NULL)
            vector_free(tracking_data->tracks[t].magnitude);
    vector_free(tracking_data->tracks);
    free_RoI_history(tracking_data->RoI_history);
    free(tracking_data->RoI_list);
    free(tracking_data);
}

void _update_bounding_box(vec_BB_t* BB_array, const int track_id, const uint32_t RoI_xmin, const uint32_t RoI_xmax,
                          const uint32_t RoI_ymin, const uint32_t RoI_ymax, int frame, int is_extrapolated) {
    assert(RoI_xmin || RoI_xmax || RoI_ymin || RoI_ymax);

    uint32_t bb_x = (uint32_t)ceilf((float)(RoI_xmin + RoI_xmax) / 2.f);
    uint32_t bb_y = (uint32_t)ceilf((float)(RoI_ymin + RoI_ymax) / 2.f);
    uint32_t rx = (bb_x - RoI_xmin);
    uint32_t ry = (bb_y - RoI_ymin);

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

void _compute_angle_and_norms(const RoI_history_t* RoI_history, const track_t* cur_track, float *angle_degree,
                              float *norm_u, float *norm_v) {
    int next_id = RoI_history->array[1][cur_track->end.id - 1].next_id;
    int k = RoI_history->array[1][cur_track->end.id - 1].prev_id - 1;

    float x0_0 = RoI_history->array[0][next_id - 1].x;
    float y0_0 = RoI_history->array[0][next_id - 1].y;

    float x1_1 = RoI_history->array[1][cur_track->end.id - 1].x;
    float y1_1 = RoI_history->array[1][cur_track->end.id - 1].y;

    float x2_2 = RoI_history->array[2][k].x;
    float y2_2 = RoI_history->array[2][k].y;

    float theta0 = RoI_history->motion[0].theta;
    float tx0 = RoI_history->motion[0].tx;
    float ty0 = RoI_history->motion[0].ty;

    float theta1 = RoI_history->motion[1].theta;
    float tx1 = RoI_history->motion[1].tx;
    float ty1 = RoI_history->motion[1].ty;

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

void _track_extrapolate(const RoI_track_t* track_end, float* track_extrapol_x, float* track_extrapol_y,
                        float* track_extrapol_u, float* track_extrapol_v, const float theta, const float tx,
                        const float ty, uint8_t extrapol_order) {
    assert(extrapol_order > 0);

    if (extrapol_order == 1) {
        // track_end->x/y @ t -1 --- track_extrapol_x/y @ t -2
        // (u, v) is the motion vector between t - 2 and t - 1
        *track_extrapol_u = track_end->x - track_end->dx - *track_extrapol_x;
        *track_extrapol_v = track_end->y - track_end->dy - *track_extrapol_y;
    }

    // motion compensation from t - 1 to t
    float x = tx + track_end->x * cosf(theta) - track_end->y * sinf(theta);
    float y = ty + track_end->x * sinf(theta) + track_end->y * cosf(theta);

    // extrapolate x and y @ t
    *track_extrapol_x = x + (float)extrapol_order * *track_extrapol_u;
    *track_extrapol_y = y + (float)extrapol_order * *track_extrapol_v;
}

void _update_existing_tracks(RoI_history_t* RoI_history, vec_track_t track_array, vec_BB_t* BB_array,
                             const size_t frame, const size_t r_extrapol, const float angle_max, const int track_all,
                             const size_t fra_meteor_max, const uint8_t extrapol_order_max,
                             const float min_extrapol_ratio_S) {
    size_t n_tracks = vector_size(track_array);
    for (size_t i = 0; i < n_tracks; i++) {
        track_t* cur_track = &track_array[i];
        if (cur_track->id && cur_track->state != TRACK_FINISHED) {
            if (cur_track->state == TRACK_LOST) {
                for (size_t j = 0; j < RoI_history->n_RoI[0]; j++) {
                    if (!RoI_history->array[0][j].prev_id) {
                        // test NaN
                        assert(RoI_history->array[0][j].x == RoI_history->array[0][j].x);
                        assert(RoI_history->array[0][j].y == RoI_history->array[0][j].y);
                        assert(cur_track->extrapol_x == cur_track->extrapol_x);
                        assert(cur_track->extrapol_y == cur_track->extrapol_y);

                        float theta = RoI_history->motion[0].theta;
                        float tx = RoI_history->motion[0].tx;
                        float ty = RoI_history->motion[0].ty;

                        float x_0 = RoI_history->array[0][j].x;
                        float y_0 = RoI_history->array[0][j].y;
                        float x_1 = cosf(theta) * (x_0 - tx) + sinf(theta) * (y_0 - ty);
                        float y_1 = cosf(theta) * (y_0 - ty) - sinf(theta) * (x_0 - tx);

                        float x_diff = cur_track->extrapol_x - x_1;
                        float y_diff = cur_track->extrapol_y - y_1;

                        float dist = sqrtf(x_diff * x_diff + y_diff * y_diff);

                        float ratio_S_ij = cur_track->end.S < RoI_history->array[0][j].S ?
                                           (float)cur_track->end.S / (float)RoI_history->array[0][j].S :
                                           (float)RoI_history->array[0][j].S / (float)cur_track->end.S;

                        if (dist < r_extrapol && ratio_S_ij >= min_extrapol_ratio_S) {
                            // save extrapolated bounding boxes
                            if (BB_array != NULL) {
                                size_t motion_id = 1;
                                for (uint8_t e = cur_track->extrapol_order; e >= 1; e--) {
                                    float theta = RoI_history->motion[motion_id + 1].theta;
                                    float tx = RoI_history->motion[motion_id + 1].tx;
                                    float ty = RoI_history->motion[motion_id + 1].ty;

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

                            RoI_history->array[0][j].is_extrapolated = 1;
                            memcpy(&cur_track->end, &RoI_history->array[0][j], sizeof(RoI_track_t));

                            if (cur_track->magnitude != NULL) {
                                // because we don't know the magnitude when RoI has been extrapolated
                                for (uint8_t e = cur_track->extrapol_order; e >= 1; e--)
                                    vector_add(&cur_track->magnitude, (uint32_t)0);
                                vector_add(&cur_track->magnitude, RoI_history->array[0][j].magnitude);
                            }

                            cur_track->extrapol_order = 0;

                            if (BB_array != NULL) {
                                size_t vs = vector_size(BB_array);
                                _update_bounding_box(&BB_array[vs - 1], cur_track->id, RoI_history->array[0][j].xmin,
                                                     RoI_history->array[0][j].xmax, RoI_history->array[0][j].ymin,
                                                     RoI_history->array[0][j].ymax, frame, /* is_extrapolated = */ 0);
                            }

                            // in the current implementation, the first RoI that matches is used for extrapolation
                            break;
                        }
                    }
                }
            }
            else if (cur_track->state == TRACK_UPDATED) {
                int next_id = RoI_history->array[1][cur_track->end.id - 1].next_id;
                if (next_id) {
                    if (cur_track->obj_type == METEOR) {
                        if (RoI_history->array[1][cur_track->end.id - 1].prev_id) {
                            float norm_u, norm_v, angle_degree;
                            _compute_angle_and_norms(RoI_history, cur_track, &angle_degree, &norm_u, &norm_v);
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

                    memcpy(&cur_track->end, &RoI_history->array[0][next_id - 1], sizeof(RoI_track_t));

                    if (cur_track->magnitude != NULL)
                        vector_add(&cur_track->magnitude, RoI_history->array[0][next_id - 1].magnitude);

                    if (BB_array != NULL) {
                        size_t vs = vector_size(BB_array);
                        _update_bounding_box(&BB_array[vs - 1], cur_track->id,
                                             RoI_history->array[0][next_id - 1].xmin,
                                             RoI_history->array[0][next_id - 1].xmax,
                                             RoI_history->array[0][next_id - 1].ymin,
                                             RoI_history->array[0][next_id - 1].ymax, frame, /* is_extrapolated = */ 0);
                    }
                } else {
                    cur_track->state = TRACK_LOST;
                }
            }
            if (cur_track->state == TRACK_LOST) {
                cur_track->extrapol_order++;
                if (cur_track->extrapol_order > extrapol_order_max) {
                    cur_track->state = TRACK_FINISHED;
                } else {
                    // on extrapole si pas finished
                    _track_extrapolate(&cur_track->end, &cur_track->extrapol_x, &cur_track->extrapol_y,
                                       &cur_track->extrapol_u, &cur_track->extrapol_v, RoI_history->motion[0].theta,
                                       RoI_history->motion[0].tx, RoI_history->motion[0].ty, cur_track->extrapol_order);
                }
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

void _insert_new_track(const RoI_track_t* RoI_list, const unsigned n_RoI, vec_track_t* track_array, vec_BB_t* BB_array,
                       const int frame, const enum obj_e type, const int magnitude)
{
    assert(n_RoI >= 1);

    size_t track_id = vector_size(*track_array) + 1;
    track_t* tmp_track = vector_add_asg(track_array);
    tmp_track->id = track_id;
    memcpy(&tmp_track->begin, &RoI_list[n_RoI - 1], sizeof(RoI_track_t));
    memcpy(&tmp_track->end, &RoI_list[0], sizeof(RoI_track_t));
    tmp_track->state = TRACK_UPDATED;
    tmp_track->obj_type = type;
    tmp_track->magnitude = NULL;
    tmp_track->extrapol_x = NAN;
    tmp_track->extrapol_y = NAN;
    tmp_track->extrapol_order = 0;
    if (magnitude) {
        tmp_track->magnitude = (vec_uint32_t)vector_create();
        for (unsigned n = 0; n < n_RoI; n++)
            vector_add(&tmp_track->magnitude, RoI_list[(n_RoI - 1) - n].magnitude);
    }
    tmp_track = NULL; // stop using temp now that the element is initialized

    if (BB_array != NULL) {
        size_t vs = vector_size(BB_array);
        for (unsigned n = 0; n < n_RoI; n++)
            _update_bounding_box(&BB_array[(vs - 1) - n - 1], track_id, RoI_list[n].xmin, RoI_list[n].xmax,
                                 RoI_list[n].ymin, RoI_list[n].ymax, (frame - n) - 1, /* is_extrapolated = */ 0);
    }
}

void _create_new_tracks(RoI_history_t* RoI_history, RoI_track_t* RoI_list, vec_track_t* track_array, vec_BB_t* BB_array,
                        const size_t frame, const float diff_dev, const int track_all, const size_t fra_star_min,
                        const size_t fra_meteor_min, const int magnitude)
{
    for (size_t i = 0; i < RoI_history->n_RoI[1]; i++) {
        int asso = RoI_history->array[1][i].next_id;
        float e = RoI_history->array[0][asso - 1].error;
        if (asso) {
            int is_new_meteor = 0;
            // if motion detected
            if (fabs(e - RoI_history->motion[0].mean_error) > diff_dev * RoI_history->motion[0].std_deviation) {
                if (RoI_history->array[1][i].is_extrapolated)
                    continue; // Extrapolated
                is_new_meteor = 1;
            } // else it is a new star

            int fra_min;
            int time;
            if (is_new_meteor) {
                time = RoI_history->array[1][i].time_motion + 1;
                RoI_history->array[0][asso - 1].time_motion = time;
                fra_min = fra_meteor_min;
            }
            else if (track_all) {
                time = RoI_history->array[1][i].time + 1;
                RoI_history->array[0][asso - 1].time = time;
                fra_min = fra_star_min;
            }
            if (is_new_meteor || track_all) {
                if (time == fra_min - 1) {
                    // this loop prevent adding duplicated tracks
                    size_t n_tracks = vector_size(*track_array);
                    size_t j = 0;
                    while (j < n_tracks && ((*track_array)[j].end.id != RoI_history->array[1][i].id ||
                           (*track_array)[j].end.x != RoI_history->array[1][i].x ||
                           (*track_array)[j].end.y != RoI_history->array[1][i].y))
                        j++;

                    if (j == n_tracks || n_tracks == 0) {
                        memcpy(&RoI_list[0], &RoI_history->array[1][i], sizeof(RoI_track_t));
                        for (int ii = 1; ii < fra_min - 1; ii++)
                            memcpy(&RoI_list[ii], &RoI_history->array[ii + 1][RoI_list[ii - 1].prev_id - 1],
                                   sizeof(RoI_track_t));
                        _insert_new_track(RoI_list, fra_min - 1, track_array, BB_array, frame,
                                          is_new_meteor ? METEOR : STAR, magnitude);
                    }
                }
            }
        }
    }
}

void _light_copy_RoI_array(const uint32_t* RoI_src_id, const uint32_t RoI_src_frame, const uint32_t* RoI_src_xmin,
                           const uint32_t* RoI_src_xmax, const uint32_t* RoI_src_ymin, const uint32_t* RoI_src_ymax,
                           const uint32_t* RoI_src_S, const float* RoI_src_x, const float* RoI_src_y,
                           const float* RoI_src_error, const uint32_t* RoI_src_prev_id, const uint32_t* RoI_magnitude,
                           const size_t n_RoI_src, RoI_track_t* RoI_dest) {
    for (size_t i = 0; i < n_RoI_src; i++) {
        RoI_dest[i].id = RoI_src_id[i];
        RoI_dest[i].frame = RoI_src_frame;
        RoI_dest[i].xmin = RoI_src_xmin[i];
        RoI_dest[i].xmax = RoI_src_xmax[i];
        RoI_dest[i].ymin = RoI_src_ymin[i];
        RoI_dest[i].ymax = RoI_src_ymax[i];
        RoI_dest[i].S = RoI_src_S[i];
        RoI_dest[i].x = RoI_src_x[i];
        RoI_dest[i].y = RoI_src_y[i];
        RoI_dest[i].error = RoI_src_error[i];
        RoI_dest[i].time = 0;
        RoI_dest[i].time_motion = 0;
        RoI_dest[i].prev_id = RoI_src_prev_id[i];
        RoI_dest[i].is_extrapolated = 0;
        RoI_dest[i].magnitude = RoI_magnitude[i];
    }
}

void light_copy_RoI_array(const RoI_t* RoI_array_src, const uint32_t RoI_src_frame, RoI_track_t* RoI_array_dest) {
    _light_copy_RoI_array(RoI_array_src->id, RoI_src_frame, RoI_array_src->basic->xmin, RoI_array_src->basic->xmax,
                          RoI_array_src->basic->ymin, RoI_array_src->basic->ymax, RoI_array_src->basic->S,
                          RoI_array_src->basic->x, RoI_array_src->basic->y, RoI_array_src->motion->error,
                          RoI_array_src->asso->prev_id, RoI_array_src->misc->magnitude, RoI_array_src->_size,
                          RoI_array_dest);
}

void _update_RoI_array_next_id(const uint32_t* RoI_prev_id, RoI_track_t* RoI_dest, const size_t n_RoI) {
    for (size_t i = 0; i < n_RoI; i++)
        if (RoI_prev_id[i])
            RoI_dest[RoI_prev_id[i] - 1].next_id = i + 1;
}

void _tracking_perform(tracking_data_t* tracking_data, const uint32_t* RoI_id, const uint32_t* RoI_xmin,
                       const uint32_t* RoI_xmax, const uint32_t* RoI_ymin, const uint32_t* RoI_ymax,
                       const uint32_t* RoI_S, const float* RoI_x, const float* RoI_y, const float* RoI_error,
                       const uint32_t* RoI_prev_id, const uint32_t* RoI_magnitude, const size_t n_RoI1,
                       vec_BB_t** BB_array, const size_t frame, const motion_t* motion_est, const size_t r_extrapol,
                       const float angle_max, const float diff_dev, const int track_all, const size_t fra_star_min,
                       const size_t fra_meteor_min, const size_t fra_meteor_max, const int magnitude,
                       const uint8_t extrapol_order_max, const float min_extrapol_ratio_S) {
    assert(extrapol_order_max < tracking_data->RoI_history->_max_size);
    assert(min_extrapol_ratio_S >= 0.f && min_extrapol_ratio_S <= 1.f);

    if (*BB_array != NULL) {
        vec_BB_t new_BB = (vec_BB_t)vector_create();
        vector_add(BB_array, new_BB);
    }

    tracking_data->RoI_history->n_RoI[0] = n_RoI1;
    _light_copy_RoI_array(RoI_id, frame, RoI_xmin, RoI_xmax, RoI_ymin, RoI_ymax, RoI_S, RoI_x, RoI_y, RoI_error,
                          RoI_prev_id, RoI_magnitude, n_RoI1, tracking_data->RoI_history->array[0]);
    tracking_data->RoI_history->motion[0] = *motion_est;
    if (tracking_data->RoI_history->_size > 0)
        _update_RoI_array_next_id(RoI_prev_id, tracking_data->RoI_history->array[1], n_RoI1);
    if (tracking_data->RoI_history->_size < tracking_data->RoI_history->_max_size)
        tracking_data->RoI_history->_size++;

    if (tracking_data->RoI_history->_size >= 2) {
        _create_new_tracks(tracking_data->RoI_history, tracking_data->RoI_list, &tracking_data->tracks, *BB_array,
                           frame, diff_dev, track_all, fra_star_min, fra_meteor_min, magnitude);
        _update_existing_tracks(tracking_data->RoI_history, tracking_data->tracks, *BB_array, frame, r_extrapol,
                                angle_max, track_all, fra_meteor_max, extrapol_order_max, min_extrapol_ratio_S);
    }

    rotate_RoI_history(tracking_data->RoI_history);
    memset(tracking_data->RoI_history->array[0], 0, tracking_data->RoI_history->n_RoI[0] * sizeof(RoI_track_t));
    tracking_data->RoI_history->n_RoI[0] = 0;
}

void tracking_perform(tracking_data_t* tracking_data, const RoI_t* RoI_array, vec_BB_t** BB_array, const size_t frame,
                      const motion_t* motion_est, const size_t r_extrapol, const float angle_max, const float diff_dev,
                      const int track_all, const size_t fra_star_min, const size_t fra_meteor_min,
                      const size_t fra_meteor_max, const int magnitude, const uint8_t extrapol_order_max,
                      const float min_extrapol_ratio_S) {
    _tracking_perform(tracking_data, RoI_array->id, RoI_array->basic->xmin, RoI_array->basic->xmax, RoI_array->basic->ymin,
                      RoI_array->basic->ymax, RoI_array->basic->S, RoI_array->basic->x, RoI_array->basic->y,
                      RoI_array->motion->error, RoI_array->asso->prev_id, RoI_array->misc->magnitude, RoI_array->_size,
                      BB_array, frame, motion_est, r_extrapol, angle_max, diff_dev, track_all, fra_star_min,
                      fra_meteor_min, fra_meteor_max, magnitude, extrapol_order_max, min_extrapol_ratio_S);
}
