/**
 * Copyright (c) 2021-2022, Clara CIOCAN, LIP6 Sorbonne University
 * Copyright (c) 2021-2022, Mathuran KANDEEPAN, LIP6 Sorbonne University
 */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <nrutil.h>
#include <ffmpeg-io/reader.h>
#include <ffmpeg-io/writer.h>

#include "args.h"
#include "ballon.h"
#include "CCL.h"
#include "debug_utils.h"
#include "features.h"
#include "threshold.h"
#include "tracking.h"
#include "video.h"
#include "macro_debug.h"

#define SIZE_BUF 10000
#define INF 9999999

static ROIx2_t g_buffer[SIZE_BUF];
BB_t* g_tabBB[NB_FRAMES];

extern uint32** g_nearest;
extern float32** g_distances;
extern uint32* g_conflicts;
extern char g_path_bounding_box[200];

enum color_e g_obj_type_to_color[N_OBJECTS];
char g_obj_type_to_string[N_OBJECTS][64];
char g_obj_type_to_string_with_spaces[N_OBJECTS][64];

void tracking_init_global_data() {
    g_obj_type_to_color[UNKNOWN] = UNKNOWN_COLOR;
    g_obj_type_to_color[STAR] = STAR_COLOR;
    g_obj_type_to_color[METEOR] = METEOR_COLOR;
    g_obj_type_to_color[NOISE] = NOISE_COLOR;

    char str_unknown[64] = UNKNOWN_STR;
    sprintf(g_obj_type_to_string[UNKNOWN], "%s", str_unknown);
    char str_star[64] = STAR_STR;
    sprintf(g_obj_type_to_string[STAR], "%s", str_star);
    char str_meteor[64] = METEOR_STR;
    sprintf(g_obj_type_to_string[METEOR], "%s", str_meteor);
    char str_noise[64] = NOISE_STR;
    sprintf(g_obj_type_to_string[NOISE], "%s", str_noise);

    unsigned max = 0;
    for (int i = 0; i < N_OBJECTS; i++)
        if (strlen(g_obj_type_to_string[i]) > max)
            max = strlen(g_obj_type_to_string[i]);

    for (int i = 0; i < N_OBJECTS; i++) {
        int len = strlen(g_obj_type_to_string[i]);
        int diff = max - len;
        for (int c = len; c >= 0; c--)
            g_obj_type_to_string_with_spaces[i][diff + c] = g_obj_type_to_string[i][c];
        for (int c = 0; c < diff; c++)
            g_obj_type_to_string_with_spaces[i][c] = ' ';
    }
}

enum obj_e tracking_string_to_obj_type(const char* string) {
    enum obj_e obj = UNKNOWN;
    for (int i = 0; i < N_OBJECTS; i++)
        if (!strcmp(string, g_obj_type_to_string[i])) {
            obj = (enum obj_e)i;
            break;
        }
    return obj;
}

unsigned tracking_count_objects(const track_t* tracks, const int n_tracks, unsigned* n_stars, unsigned* n_meteors,
                                unsigned* n_noise) {
    (*n_stars) = (*n_meteors) = (*n_noise) = 0;
    for (int i = 0; i < n_tracks; i++)
        if (tracks[i].time)
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

void tracking_init_array_BB() {
    for (int i = 0; i < NB_FRAMES; i++)
        g_tabBB[i] = NULL;
}

void add_to_list(uint16 rx, uint16 ry, uint16 bb_x, uint16 bb_y, uint16 track_id, int frame) {
    assert(frame < NB_FRAMES);
    BB_t* newE = (BB_t*)malloc(sizeof(BB_t));
    newE->rx = rx;
    newE->ry = ry;
    newE->bb_x = bb_x;
    newE->bb_y = bb_y;
    newE->track_id = track_id;
    newE->next = g_tabBB[frame];
    g_tabBB[frame] = newE;
}

void clear_index_track_t(track_t* tracks, int i) { memset(&tracks[i], 0, sizeof(track_t)); }

void tracking_init_tracks(track_t* tracks, int n) {
    for (int i = 0; i < n; i++)
        clear_index_track_t(tracks, i);
}

void clear_index_buffer(int i) {
    memset(&g_buffer[i].stats0, 0, sizeof(ROI_t));
    memset(&g_buffer[i].stats1, 0, sizeof(ROI_t));
    g_buffer[i].frame = 0;
}

void clear_buffer_history(int frame, int history_size) {
    int diff;
    for (int i = 0; i < SIZE_BUF; i++) {
        if (g_buffer[i].frame != 0) {
            diff = frame - g_buffer[i].frame;
            if (diff >= history_size)
                clear_index_buffer(i);
        }
    }
}

void insert_buffer(ROI_t stats0, ROI_t stats1, int frame) {
    for (int i = 0; i < SIZE_BUF; i++)
        if (g_buffer[i].stats0.ID == 0) {
            memcpy(&g_buffer[i].stats0, &stats0, sizeof(ROI_t));
            memcpy(&g_buffer[i].stats1, &stats1, sizeof(ROI_t));
            g_buffer[i].frame = frame;
            return;
        }
    fprintf(stderr, "(EE) This sould never happen, out of 'g_buffer' ('SIZE_BUF' = %d)\n", SIZE_BUF);
    exit(-1);
}

int search_buf_stat(int ROI_id, int frame) {
    for (int i = 0; i < SIZE_BUF; i++)
        if (frame == g_buffer[i].frame + 1 && ROI_id == g_buffer[i].stats0.ID)
            return i;
    return -1;
}

void track_extrapolate(track_t* t, int theta, int tx, int ty) {
    float32 u, v;
    float32 x, y;
    t->state = TRACK_LOST;
    // compensation du mouvement + calcul vitesse entre t-1 et t
    u = t->end.x - t->end.dx - t->x;
    v = t->end.y - t->end.dy - t->y;

    x = tx + t->end.x * cos(theta) - t->end.y * sin(theta);
    y = ty + t->end.x * sin(theta) + t->end.y * cos(theta);

    t->x = x + u;
    t->y = y + v;
}

void update_bounding_box(track_t* track, ROI_t stats, int frame) {
    PUTS("UPDATE BB");
    uint16 rx, ry, bb_x, bb_y;

    assert(stats.xmin || stats.xmax || stats.ymin || stats.ymax);

    bb_x = (uint16)ceil((double)((stats.xmin + stats.xmax)) / 2);
    bb_y = (uint16)ceil((double)((stats.ymin + stats.ymax)) / 2);
    rx = (bb_x - stats.xmin) + 5;
    ry = (bb_y - stats.ymin) + 5;

    // juste pour debug (affichage)
    track->bb_x = bb_x;
    track->bb_y = bb_y;
    track->rx = rx;
    track->ry = ry;

    add_to_list(rx, ry, bb_x, bb_y, track->id, frame - 1);
}

void update_existing_tracks(ROI_t* stats0, ROI_t* stats1, track_t* tracks, int nc1, int frame, int* offset,
                            int* tracks_cnt, int theta, int tx, int ty, int r_extrapol, int d_line, int track_all) {
    int next;
    int i;

    for (i = *offset; i <= *tracks_cnt; i++) {
        next = tracks[i].end.next;
        if (!next) {
            *offset = i;
            break;
        }
    }
    for (i = *offset; i <= *tracks_cnt; i++) {
        if (tracks[i].time > 150 && !track_all) {
            clear_index_track_t(tracks, i);
            continue;
        }
        if (tracks[i].time && tracks[i].state != TRACK_FINISHED) {
            if (tracks[i].state == TRACK_EXTRAPOLATED) {
                PUTS("TRACK_EXTRAPOLATED");
                for (int j = 1; j <= nc1; j++) {
                    if ((stats0[j].x > tracks[i].x - r_extrapol) && (stats0[j].x < tracks[i].x + r_extrapol) &&
                        (stats0[j].y < tracks[i].y + r_extrapol) && (stats0[j].y > tracks[i].y - r_extrapol)) {
                        tracks[i].end = stats0[j];
                        stats0[j].track_id = tracks[i].id;
                        tracks[i].state = TRACK_UPDATED;
                        update_bounding_box(tracks + i, stats0[j], frame - 1);
                    }
                }
            }
            if (tracks[i].state == TRACK_LOST) {
                PUTS("TRACK_LOST");
                for (int j = 1; j <= nc1; j++) {
                    if (!stats1[j].prev) {
                        if ((stats1[j].x > tracks[i].x - r_extrapol) && (stats1[j].x < tracks[i].x + r_extrapol) &&
                            (stats1[j].y < tracks[i].y + r_extrapol) && (stats1[j].y > tracks[i].y - r_extrapol)) {
                            tracks[i].state = TRACK_EXTRAPOLATED;
                            tracks[i].time += 2;
                            stats1[j].state = 1;
                        }
                    }
                }
                if (tracks[i].state != TRACK_EXTRAPOLATED) {
                    PUTS("FINISHED");
                    tracks[i].state = TRACK_FINISHED;
                }
            }
            if (tracks[i].state == TRACK_UPDATED || tracks[i].state == TRACK_NEW) {
                next = stats0[tracks[i].end.ID].next;
                if (next) {
                    float32 a;
                    float32 dx = (stats1[next].x - stats0[tracks[i].end.ID].x);
                    float32 dy = (stats1[next].y - stats0[tracks[i].end.ID].y);

                    a = (dx == 0) ? INF : (dy / dx);

                    float32 y = tracks[i].a * stats1[next].x + tracks[i].b;

                    if ((fabs(stats1[next].y - y) < d_line) &&
                        ((dx * tracks[i].dx >= 0.0f) && (dy * tracks[i].dy >= 0.0f)) &&
                        ((a < 0 && tracks[i].a < 0) || (a > 0 && tracks[i].a > 0) ||
                         ((a == INF) && (tracks[i].a == INF)))) {
                        tracks[i].obj_type = METEOR;
                        tracks[i].a = a;
                        tracks[i].dx = dx;
                        tracks[i].dy = dy;
                        tracks[i].b = y - a * stats1[next].x;
                    } else {
                        if (tracks[i].obj_type == METEOR) {
                            if (!track_all) {
                                clear_index_track_t(tracks, i);
                                continue;
                            } else
                                tracks[i].obj_type = NOISE;
                        }
                    }

                    // tracks[i].vitesse[(tracks[i].cur)++] = stats0[tracks[i].end.ID].error;
                    tracks[i].x = tracks[i].end.x;
                    tracks[i].y = tracks[i].end.y;
                    tracks[i].end = stats1[next];
                    tracks[i].time++;
                    stats1[next].track_id = tracks[i].id;
                    update_bounding_box(tracks + i, stats1[next], frame + 1);
                } else {
                    // on extrapole si pas finished
                    track_extrapolate(&tracks[i], theta, tx, ty);
                    // tracks[i].state = TRACK_FINISHED;
                }
            }
        }
    }
}

void insert_new_track(ROI_t* ROI_list[256], unsigned n_ROI, track_t* tracks, int tracks_cnt, int frame,
                      enum obj_e type) {
    assert(tracks_cnt < SIZE_MAX_TRACKS);
    assert(n_ROI >= 2);

    ROI_t* first_ROI = ROI_list[n_ROI - 1];
    ROI_t* before_last_ROI = ROI_list[1];
    ROI_t* last_ROI = ROI_list[0];

    tracks[tracks_cnt].id = tracks_cnt + 1;
    tracks[tracks_cnt].begin = *first_ROI;
    tracks[tracks_cnt].end = *last_ROI;
    tracks[tracks_cnt].bb_x = (uint16)ceil((double)((first_ROI->xmin + first_ROI->xmax)) / 2);
    tracks[tracks_cnt].bb_y = (uint16)ceil((double)((first_ROI->ymin + first_ROI->ymax)) / 2);
    tracks[tracks_cnt].time = n_ROI - 1; // TODO: -1 is wrong, fix this where 'track_t.time' is used later in the code
    tracks[tracks_cnt].timestamp = frame - (n_ROI);
    tracks[tracks_cnt].state = TRACK_NEW;
    tracks[tracks_cnt].obj_type = type;

    if (type != STAR) {
        float dx = (last_ROI->x - before_last_ROI->x);
        float dy = (last_ROI->y - before_last_ROI->y);

        tracks[tracks_cnt].a = (dx == 0) ? INF : (dy / dx);
        tracks[tracks_cnt].b = last_ROI->y - tracks[tracks_cnt].a * last_ROI->x;
        tracks[tracks_cnt].dx = dx;
        tracks[tracks_cnt].dy = dy;
    }

    for (unsigned n = 0; n < n_ROI; n++) {
        ROI_list[n]->track_id = tracks[tracks_cnt].id;
        update_bounding_box(&tracks[tracks_cnt], *ROI_list[n], frame - n);
    }
}

void fill_ROI_list(ROI_t* ROI_list[256], const unsigned n_ROI, ROI_t* last_ROI, const unsigned frame) {
    assert(n_ROI < 256);
    unsigned cpt = 0;

    ROI_list[cpt++] = last_ROI;
    int k = search_buf_stat(last_ROI->prev, frame);
    for (int f = 1; f < n_ROI; f++) {
        if (k != -1) {
            ROI_list[cpt++] = &g_buffer[k].stats0;
            k = search_buf_stat(g_buffer[k].stats0.prev, frame - f);
        } else {
            fprintf(stderr, "(EE) This should never happen ('k' = -1, 'f' = %d.\n", f);
            exit(-1);
        }
    }
    assert(cpt == n_ROI);
}

void create_new_tracks(ROI_t* stats0, ROI_t* stats1, track_t* tracks, int nc0, int frame, int* tracks_cnt,
                       int* offset, float diff_deviation, int track_all, int min_frames_star) {
    ROI_t* ROI_list[256];
    double errMoy = features_error_moy(stats0, nc0);
    double eType = features_ecart_type(stats0, nc0, errMoy);

    for (int i = 1; i <= nc0; i++) {
        float32 e = stats0[i].error;
        int asso = stats0[i].next;
        if (asso) {
            // si mouvement detecté
            if (fabs(e - errMoy) > diff_deviation * eType) {
                if (stats0[i].state) {
                    PUTS("EXTRAPOLATED");
                    continue; // Extrapolated
                }
                stats0[i].motion = 1; // debug
                stats0[i].time_motion++;
                stats1[stats0[i].next].time_motion = stats0[i].time_motion;
                if (stats0[i].time_motion == 3 - 1) {
                    int j;
                    for (j = *offset; j <= *tracks_cnt; j++)
                        if (tracks[j].end.ID == stats0[i].ID && tracks[j].end.x == stats0[i].x)
                            break;
                    if (j == *tracks_cnt + 1 || *tracks_cnt == -1) {
                        fill_ROI_list(ROI_list, 3 - 1, &stats0[i], frame);
                        insert_new_track(ROI_list, 3 - 1, tracks, ++(*tracks_cnt), frame, METEOR);
                    }
                } else if (stats0[i].time_motion == 1)
                    insert_buffer(stats0[i], stats1[stats0[i].next], frame);
            } else if (track_all) {
                stats0[i].time++;
                stats1[stats0[i].next].time = stats0[i].time;
                if (stats0[i].time == min_frames_star - 1) {
                    fill_ROI_list(ROI_list, min_frames_star - 1, &stats0[i], frame);
                    insert_new_track(ROI_list, min_frames_star - 1, tracks, ++(*tracks_cnt), frame, STAR);
                } else
                    insert_buffer(stats0[i], stats1[stats0[i].next], frame);
            }
        }
    }
}

void tracking_perform(ROI_t* stats0, ROI_t* stats1, track_t* tracks, int nc0, int nc1, int frame, int* tracks_cnt,
                      int* offset, int theta, int tx, int ty, int r_extrapol, int d_line, float diff_deviation,
                      int track_all, int min_frames_star) {
    create_new_tracks(stats0, stats1, tracks, nc0, frame, tracks_cnt, offset, diff_deviation, track_all,
                      min_frames_star);
    update_existing_tracks(stats0, stats1, tracks, nc1, frame, offset, tracks_cnt, theta, tx, ty, r_extrapol, d_line,
                           track_all);
    clear_buffer_history(frame, min_frames_star);
}
