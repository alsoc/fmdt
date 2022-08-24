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
#define FRA_METEOR_MAX 150

enum color_e g_obj_to_color[N_OBJECTS];
char g_obj_to_string[N_OBJECTS][64];
char g_obj_to_string_with_spaces[N_OBJECTS][64];

ROI_buffer_t* tracking_alloc_ROI_buffer(const unsigned size) {
    ROI_buffer_t* ROI_buff = (ROI_buffer_t*) malloc(sizeof(ROI_buffer_t));
    ROI_buff->size = size;
    ROI_buff->data = (ROI_t**)malloc(size * sizeof(ROI_t*));
    for (int i = 0; i < ROI_buff->size; i++)
        ROI_buff->data[i] = (ROI_t*)malloc(MAX_ROI_SIZE * sizeof(ROI_t));
    return ROI_buff;
}

void tracking_free_ROI_buffer(ROI_buffer_t* ROI_buff) {
    for (int i = 0; i < ROI_buff->size; i++)
        free(ROI_buff->data[i]);
    free(ROI_buff->data);
    free(ROI_buff);
}

void tracking_rotate_ROI_buffer(ROI_buffer_t* ROI_buff) {
    ROI_t* last_ROI_tmp = ROI_buff->data[ROI_buff->size -1];
    for (int i = ROI_buff->size -2; i >= 0; i--)
        ROI_buff->data[i + 1] = ROI_buff->data[i];
    ROI_buff->data[0] = last_ROI_tmp;
}

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

void clear_index_tracks(track_t* tracks, int i) { memset(&tracks[i], 0, sizeof(track_t)); }

void tracking_init_tracks(track_t* tracks, int n) {
    for (int i = 0; i < n; i++)
        clear_index_tracks(tracks, i);
}

void track_extrapolate(track_t* t, int theta, int tx, int ty) {
    // compensation du mouvement + calcul vitesse entre t-1 et t
    float u = t->end.x - t->end.dx - t->x;
    float v = t->end.y - t->end.dy - t->y;

    float x = tx + t->end.x * cos(theta) - t->end.y * sin(theta);
    float y = ty + t->end.x * sin(theta) + t->end.y * cos(theta);

    t->x = x + u;
    t->y = y + v;
}

void update_bounding_box(BB_t** BB_array, track_t* track, ROI_t stats, int frame) {
    assert(stats.xmin || stats.xmax || stats.ymin || stats.ymax);

    uint16_t bb_x = (uint16_t)ceil((double)((stats.xmin + stats.xmax)) / 2);
    uint16_t bb_y = (uint16_t)ceil((double)((stats.ymin + stats.ymax)) / 2);
    uint16_t rx = (bb_x - stats.xmin);
    uint16_t ry = (bb_y - stats.ymin);

    track->bb_x = bb_x;
    track->bb_y = bb_y;
    track->rx = rx;
    track->ry = ry;

    add_to_BB_array(BB_array, rx, ry, bb_x, bb_y, track->id, frame - 1);
}

void update_existing_tracks(ROI_buffer_t* ROI_buff, track_t* tracks, BB_t** BB_array, int nc1, int frame, int* offset,
                            int* tracks_cnt, int theta, int tx, int ty, int r_extrapol, float angle_max,
                            int track_all) {
    ROI_t* stats0 = ROI_buff->data[1];
    ROI_t* stats1 = ROI_buff->data[0];

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
        if (tracks[i].time && tracks[i].state != TRACK_FINISHED) {
            if (tracks[i].state == TRACK_EXTRAPOLATED) {
                PUTS("TRACK_EXTRAPOLATED");
                for (int j = 1; j <= nc1; j++) {
                    if ((stats0[j].x > tracks[i].x - r_extrapol) && (stats0[j].x < tracks[i].x + r_extrapol) &&
                        (stats0[j].y < tracks[i].y + r_extrapol) && (stats0[j].y > tracks[i].y - r_extrapol)) {
                        tracks[i].end = stats0[j];
                        stats0[j].track_id = tracks[i].id;
                        tracks[i].state = TRACK_UPDATED;
                        update_bounding_box(BB_array, tracks + i, stats0[j], frame - 1);
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
                next = stats0[tracks[i].end.id].next;
                if (next) {
                    if (tracks[i].obj_type == METEOR) {
                        float angle_degree = 0.f;
                        float norm_u = 0.f;
                        float norm_v = 0.f;
                        if (stats0[tracks[i].end.id].prev) {
                            int k = stats0[tracks[i].end.id].prev;
                            float u_x = (stats0[tracks[i].end.id].x - ROI_buff->data[2][k].x);
                            float u_y = (stats0[tracks[i].end.id].y - ROI_buff->data[2][k].y);
                            float v_x = (stats1[next].x - ROI_buff->data[2][k].x);
                            float v_y = (stats1[next].y - ROI_buff->data[2][k].y);

                            float scalar_prod_uv = u_x * v_x + u_y * v_y;
                            norm_u = sqrtf(u_x * u_x + u_y * u_y);
                            norm_v = sqrtf(v_x * v_x + v_y * v_y);
                            float cos_uv = scalar_prod_uv / (norm_u * norm_v);
                            float angle_rad = acosf(cos_uv >= 1 ? 0.99999f : cos_uv);
                            angle_degree = angle_rad * (180.f / (float)M_PI);
                            // float angle_degree_mod = fmodf(angle_degree, 360.f);
                        }
                        if (angle_degree >= angle_max || norm_u >= norm_v) {
                            tracks[i].obj_type = NOISE;
                            if (!track_all) {
                                clear_index_tracks(tracks, i);
                                continue;
                            }
                        }
                    }
                    // tracks[i].vitesse[(tracks[i].cur)++] = stats0[tracks[i].end.id].error;
                    tracks[i].x = tracks[i].end.x;
                    tracks[i].y = tracks[i].end.y;
                    tracks[i].end = stats1[next];
                    tracks[i].time++;
                    stats1[next].track_id = tracks[i].id;
                    update_bounding_box(BB_array, tracks + i, stats1[next], frame + 1);
                } else {
                    // on extrapole si pas finished
                    track_extrapolate(&tracks[i], theta, tx, ty);
                    tracks[i].state = TRACK_LOST;
                }
            }
            if (tracks[i].time > FRA_METEOR_MAX) {
                if (tracks[i].obj_type == METEOR)
                    tracks[i].obj_type = NOISE;
                if (!track_all) {
                    clear_index_tracks(tracks, i);
                    continue;
                }
            }
        }
    }
}

void insert_new_track(ROI_t* ROI_list[256], unsigned n_ROI, track_t* tracks, int tracks_cnt, BB_t** BB_array, int frame,
                      enum obj_e type) {
    assert(tracks_cnt < MAX_TRACKS_SIZE);
    assert(n_ROI >= 2);

    ROI_t* first_ROI = ROI_list[n_ROI - 1];
    ROI_t* last_ROI = ROI_list[0];

    tracks[tracks_cnt].id = tracks_cnt + 1;
    tracks[tracks_cnt].begin = *first_ROI;
    tracks[tracks_cnt].end = *last_ROI;
    tracks[tracks_cnt].bb_x = (uint16_t)ceil((double)((first_ROI->xmin + first_ROI->xmax)) / 2);
    tracks[tracks_cnt].bb_y = (uint16_t)ceil((double)((first_ROI->ymin + first_ROI->ymax)) / 2);
    tracks[tracks_cnt].time = n_ROI - 1; // TODO: -1 is wrong, fix this where 'track_t.time' is used later in the code
    tracks[tracks_cnt].timestamp = frame - (n_ROI);
    tracks[tracks_cnt].state = TRACK_NEW;
    tracks[tracks_cnt].obj_type = type;

    for (unsigned n = 0; n < n_ROI; n++) {
        ROI_list[n]->track_id = tracks[tracks_cnt].id;
        update_bounding_box(BB_array, &tracks[tracks_cnt], *ROI_list[n], frame - n);
    }
}

void fill_ROI_list(const ROI_t** ROI_data, ROI_t* ROI_list[256], const unsigned n_ROI, ROI_t* last_ROI,
                   const unsigned frame) {
    assert(n_ROI < 256);
    ROI_list[0] = last_ROI;
    for (int i = 1; i < n_ROI; i++)
        ROI_list[i] = (ROI_t*)&ROI_data[i+1][ROI_list[i - 1]->prev];
}

void create_new_tracks(ROI_buffer_t* ROI_buff, track_t* tracks, BB_t** BB_array, int nc0, int frame, int* tracks_cnt,
                       int* offset, float diff_dev, int track_all, int fra_star_min) {
    ROI_t* ROI_list[256];
    ROI_t* stats0 = ROI_buff->data[1];
    ROI_t* stats1 = ROI_buff->data[0];

    double errMoy = features_error_moy(stats0, nc0);
    double eType = features_ecart_type(stats0, nc0, errMoy);

    for (int i = 1; i <= nc0; i++) {
        float e = stats0[i].error;
        int asso = stats0[i].next;
        if (asso) {
            // si mouvement detecté
            if (fabs(e - errMoy) > diff_dev * eType) {
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
                        if (tracks[j].end.id == stats0[i].id && tracks[j].end.x == stats0[i].x)
                            break;
                    if (j == *tracks_cnt + 1 || *tracks_cnt == -1) {
                        fill_ROI_list((const ROI_t**)ROI_buff->data, ROI_list, 3 - 1, &stats0[i], frame);
                        insert_new_track(ROI_list, 3 - 1, tracks, ++(*tracks_cnt), BB_array, frame, METEOR);
                    }
                }
            } else if (track_all) {
                stats0[i].time++;
                stats1[stats0[i].next].time = stats0[i].time;
                if (stats0[i].time == fra_star_min - 1) {
                    fill_ROI_list((const ROI_t**)ROI_buff->data, ROI_list, fra_star_min - 1, &stats0[i], frame);
                    insert_new_track(ROI_list, fra_star_min - 1, tracks, ++(*tracks_cnt), BB_array, frame, STAR);
                }
            }
        }
    }
}

void tracking_perform(ROI_buffer_t* ROI_buff, track_t* tracks, BB_t** BB_array, int nc0, int nc1, int frame,
                      int* tracks_cnt, int* offset, int theta, int tx, int ty, int r_extrapol, float angle_max,
                      float diff_dev, int track_all, int fra_star_min) {
    create_new_tracks(ROI_buff, tracks, BB_array, nc0, frame, tracks_cnt, offset, diff_dev, track_all, fra_star_min);
    update_existing_tracks(ROI_buff,tracks, BB_array, nc1, frame, offset, tracks_cnt, theta, tx, ty, r_extrapol,
                           angle_max, track_all);
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

void tracking_print_tracks(FILE* f, track_t* tracks, int n)
{
    fprintf(f, "# -------||---------------------------||---------------------------||---------\n");
    fprintf(f, "#  Track ||           Begin           ||            End            ||  Object \n");
    fprintf(f, "# -------||---------------------------||---------------------------||---------\n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------\n");
    fprintf(f, "#     Id || Frame # |      x |      y || Frame # |      x |      y ||    Type \n");
    fprintf(f, "# -------||---------|--------|--------||---------|--------|--------||---------\n");

    unsigned track_id = 0;
    for (int i = 0; i < n; i++)
        if (tracks[i].time) {
            fprintf(f, "   %5d || %7d | %6.1f | %6.1f || %7d | %6.1f | %6.1f || %s \n", tracks[i].id,
                    tracks[i].timestamp, tracks[i].begin.x, tracks[i].begin.y, tracks[i].timestamp + tracks[i].time,
                    tracks[i].end.x, tracks[i].end.y, g_obj_to_string_with_spaces[tracks[i].obj_type]);
            track_id++;
        }
}

void tracking_parse_tracks(const char* filename, track_t* tracks, int* n)
{
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
    // int bb_x, bb_y;
    // int obj_type;
    char obj_type_str[1024];

    *n = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        // printf("Retrieved line of length %zu:\n", read);
        if (line[0] != '#') {
            sscanf(line, "%d || %d | %f | %f || %d | %f | %f || %s ", &tid, &t0, &x0, &y0, &t1, &x1, &y1, obj_type_str);

            tracks[*n].id = tid;
            tracks[*n].timestamp = t0;
            tracks[*n].time = t1 - t0;
            tracks[*n].state = 0;
            tracks[*n].begin.x = x0;
            tracks[*n].begin.y = y0;
            tracks[*n].end.x = x1;
            tracks[*n].end.y = y1;
            // tracks[*n].bb_x   = bb_x;
            // tracks[*n].bb_y   = bb_y;
            tracks[*n].obj_type = tracking_string_to_obj_type((const char*)obj_type_str);
            (*n)++;
        }
    }

    fclose(fp);
    if (line)
        free(line);
}

void tracking_save_tracks(const char* filename, track_t* tracks, int n) {
    FILE* f = fopen(filename, "w");
    if (f == NULL) {
        fprintf(stderr, "(EE) error ouverture %s \n", filename);
        exit(1);
    }

    int cpt = 0;
    for (int i = 0; i <= n; i++) {
        if (tracks[i].time)
            cpt++;
    }

    fprintf(f, "%d\n", cpt);

    if (cpt != 0) {
        for (int i = 0; i <= n; i++) {
            if (tracks[i].time) {
                fprintf(f, "%4d \t %6.1f \t %6.1f \t %4d \t %6.1f \t %6.1f \t %4d \t %4d \t %4d\n", tracks[i].timestamp,
                        tracks[i].begin.x, tracks[i].begin.y, tracks[i].timestamp + tracks[i].time, tracks[i].end.x,
                        tracks[i].end.y, tracks[i].bb_x, tracks[i].bb_y, tracks[i].obj_type);
            }
        }
    }
    fclose(f);
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
