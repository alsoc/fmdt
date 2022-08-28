#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "macros.h"
#include "validation.h"

#define TOLERANCE_DISTANCEMIN 20 // 8

static unsigned g_n_val_objects = 0;
static validation_obj_t* g_val_objects = NULL;

static int g_true_positive[N_OBJECTS] = {0};
static int g_false_positive[N_OBJECTS] = {0};
static int g_true_negative[N_OBJECTS] = {0};
static int g_false_negative[N_OBJECTS] = {0};
uint8_t g_is_valid_track[MAX_TRACKS_SIZE] = {0};

int validation_init(const char* val_objects_file) {
    assert(val_objects_file != NULL);

    FILE* file = fopen(val_objects_file, "r");
    if (!file) {
        fprintf(stderr, "(EE) Impossible to open '%s'\n", val_objects_file);
        exit(1);
    }

    // count the number of lines
    char tmp_obj_type[1024];
    int16_t tmp_t0;
    float tmp_x0;
    float tmp_y0;
    int16_t tmp_t1;
    float tmp_x1;
    float tmp_y1;
    g_n_val_objects = 0;
    while (!feof(file) && fscanf(file, "%s %hu \t %f \t %f \t %hu \t %f \t %f \n", tmp_obj_type, &tmp_t0, &tmp_x0,
                                 &tmp_y0, &tmp_t1, &tmp_x1, &tmp_y1))
        g_n_val_objects++;
    fseek(file, 0, SEEK_SET);

    if (g_n_val_objects < 1) {
        VERBOSE(fprintf(stderr, "(DBG) [Validation] aucun meteore a suivre dans le fichier input donne !\n"););
        return 0;
    } else {
        VERBOSE(fprintf(stderr, "(DBG) [Validation] %4hu entrees dans le fichier d'input\n",
                        (unsigned short)g_n_val_objects););
    }

    g_val_objects = (validation_obj_t*)malloc(g_n_val_objects * sizeof(validation_obj_t));

    int i = 0;
    while (i < g_n_val_objects && !feof(file)) {
        if (fscanf(file, "%s %hu \t %f \t %f \t %hu \t %f \t %f \n", tmp_obj_type, &g_val_objects[i].t0,
                   &g_val_objects[i].x0, &g_val_objects[i].y0, &g_val_objects[i].t1, &g_val_objects[i].x1,
                   &g_val_objects[i].y1)) {
            g_val_objects[i].t0_min = g_val_objects[i].t0 - 5;
            g_val_objects[i].t1_max = g_val_objects[i].t1 + 5;

            g_val_objects[i].a =
                (float)(g_val_objects[i].y1 - g_val_objects[i].y0) / (float)(g_val_objects[i].x1 - g_val_objects[i].x0);
            g_val_objects[i].b = g_val_objects[i].y1 - g_val_objects[i].a * g_val_objects[i].x1;

            VERBOSE(fprintf(stderr,
                            "(DBG) [Validation] Input %-2d : t0=%-4d x0=%6.1f y0=%6.1f t1=%-4d x1=%6.1f "
                            "y1=%6.1f\tf(x)=%-3.3f*x+%-3.3f\n",
                            i, g_val_objects[i].t0, g_val_objects[i].x0, g_val_objects[i].y0, g_val_objects[i].t1,
                            g_val_objects[i].x1, g_val_objects[i].y1, g_val_objects[i].a, g_val_objects[i].b););

            g_val_objects[i].track = NULL;
            g_val_objects[i].xt = g_val_objects[i].x0;
            g_val_objects[i].yt = g_val_objects[i].y0;

            g_val_objects[i].nb_tracks = 0;
            g_val_objects[i].hits = 0;
            g_val_objects[i].hits = 0; // tmp

            g_val_objects[i].dirX = g_val_objects[i].x1 > g_val_objects[i].x0; // vers la droite
            g_val_objects[i].dirY = g_val_objects[i].y0 < g_val_objects[i].y1; // vers le bas

            if (g_val_objects[i].dirX) {
                if (g_val_objects[i].dirY) {
                    g_val_objects[i].bb_y0 = g_val_objects[i].y0 - TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_x0 = g_val_objects[i].x0 - TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_y1 = g_val_objects[i].y1 + TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_x1 = g_val_objects[i].x1 + TOLERANCE_DISTANCEMIN;
                } else {
                    g_val_objects[i].bb_y0 = g_val_objects[i].y1 - TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_x0 = g_val_objects[i].x0 - TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_y1 = g_val_objects[i].y0 + TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_x1 = g_val_objects[i].x1 + TOLERANCE_DISTANCEMIN;
                }
            } else {
                if (g_val_objects[i].dirY) {
                    g_val_objects[i].bb_y0 = g_val_objects[i].y0 - TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_x0 = g_val_objects[i].x1 - TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_y1 = g_val_objects[i].y1 + TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_x1 = g_val_objects[i].x0 + TOLERANCE_DISTANCEMIN;
                } else {
                    g_val_objects[i].bb_y0 = g_val_objects[i].y1 - TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_x0 = g_val_objects[i].x1 - TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_y1 = g_val_objects[i].y0 + TOLERANCE_DISTANCEMIN;
                    g_val_objects[i].bb_x1 = g_val_objects[i].x0 + TOLERANCE_DISTANCEMIN;
                }
            }

            if (!strcmp(tmp_obj_type, "noise"))
                g_val_objects[i].obj_type = NOISE;
            else if (!strcmp(tmp_obj_type, "meteor"))
                g_val_objects[i].obj_type = METEOR;
            else if (!strcmp(tmp_obj_type, "star"))
                g_val_objects[i].obj_type = STAR;
            else
                g_val_objects[i].obj_type = UNKNOWN;
            i++;
        }
    }
    g_n_val_objects = i; // petit risque de memoire pas utilisee

    fclose(file);

    return g_n_val_objects;
}

void validation_process(const track_t* track_array) {
    for (int t = 0; t < track_array->_size; t++) {
        validation_obj_t* val_obj = NULL;
        for (int i = 0; i < g_n_val_objects; i++) {
            if (g_val_objects[i].t0_min <= track_array->begin->frame[t] &&
                track_array->begin->frame[t] + tracking_get_track_time(track_array, t) <= g_val_objects[i].t1_max &&
                g_val_objects[i].bb_x0 <= track_array->begin->x[t] && track_array->end->x[t] <= g_val_objects[i].bb_x1 &&
                g_val_objects[i].bb_y0 <= track_array->begin->y[t] && track_array->end->y[t] <= g_val_objects[i].bb_y1 &&
                track_array->obj_type[t] == g_val_objects[i].obj_type) {
#ifdef ENABLE_DEBUG
                g_val_objects[i].track_array_t0 = track_array->begin->frame[t];
                g_val_objects[i].track_array_t1 = track_array->end->frame[t];
                g_val_objects[i].track_array_x0 = track_array->begin->x[t];
                g_val_objects[i].track_array_y0 = track_array->begin->y[t];
                g_val_objects[i].track_array_x1 = track_array->end->x[t];
                g_val_objects[i].track_array_y1 = track_array->end->y[t];
#endif
                val_obj = &g_val_objects[i];
                if (g_val_objects[i].nb_tracks == 0)
                    break; // maybe
            }
        }

        // Piste matche avec un input
        if (val_obj) {
            val_obj->nb_tracks++;
            val_obj->hits = tracking_get_track_time(track_array, t) + val_obj->hits + 1;
            g_true_positive[track_array->obj_type[t]]++;
            if (track_array->obj_type[t] == METEOR)
                g_is_valid_track[t] = 1;
        } else { // Piste ne matche pas avec input
            g_false_positive[track_array->obj_type[t]]++;
            if (track_array->obj_type[t] == METEOR)
                g_is_valid_track[t] = 2;
        }
    }

    for (int i = 0; i < g_n_val_objects; i++)
        if (!g_val_objects[i].nb_tracks)
            g_false_negative[g_val_objects[i].obj_type]++;

    for (int t = 0; t < track_array->_size; t++)
        for (int ot = 1; ot < N_OBJECTS; ot++)
            if (ot != track_array->obj_type[t])
                g_true_negative[ot]++;
}

void validation_print(const track_t* track_array) {
    float tracking_rate[N_OBJECTS + 1];

    unsigned total_tracked_frames[N_OBJECTS + 1] = {0};
    unsigned total_gt_frames[N_OBJECTS + 1] = {0};
    if (g_val_objects) {
        printf("# ---------------||--------------||---------------||--------\n");
        printf("#    GT Object   ||     Hits     ||   GT Frames   || Tracks \n");
        printf("# ---------------||--------------||---------------||--------\n");
        printf("# -----|---------||--------|-----||-------|-------||--------\n");
        printf("#   Id |    Type || Detect |  GT || Start |  Stop ||      # \n");
        printf("# -----|---------||--------|-----||-------|-------||--------\n");
        for (int i = 0; i < g_n_val_objects; i++) {
            int expected_hits = g_val_objects[i].t1 - g_val_objects[i].t0 + 1;

            // tmp
            if (g_val_objects[i].hits == 1)
                g_val_objects[i].hits = 0; // TODO: what is this?!
            VERBOSE(fprintf(stderr,
                            "(DBG) [Validation] Input %-2d : hits = %d/%d \t nb_tracks = %3d \t %4d \t %4d \t %4d \t "
                            "%4d \t %6.1f \t %6.1f \t %6.1f \t %6.1f\n",
                            i + 1, g_val_objects[i].hits, expected_hits, g_val_objects[i].nb_tracks,
                            g_val_objects[i].t0, g_val_objects[i].t1, g_val_objects[i].track_t0,
                            g_val_objects[i].track_t1, g_val_objects[i].track_x0, g_val_objects[i].track_y0,
                            g_val_objects[i].track_x1, g_val_objects[i].track_y1););
            printf("   %3d | %s ||    %3d | %3d || %5d | %5d ||  %5d  \n", i + 1,
                   g_obj_to_string_with_spaces[g_val_objects[i].obj_type], g_val_objects[i].hits, expected_hits,
                   g_val_objects[i].t0, g_val_objects[i].t1, g_val_objects[i].nb_tracks);

            unsigned tmp = (g_val_objects[i].hits <= expected_hits)
                               ? g_val_objects[i].hits
                               : expected_hits - (g_val_objects[i].hits - expected_hits);
            total_gt_frames[g_val_objects[i].obj_type] += expected_hits;
            total_tracked_frames[g_val_objects[i].obj_type] += tmp;
            total_gt_frames[N_OBJECTS] += expected_hits;
            total_tracked_frames[N_OBJECTS] += tmp;
        }
        free(g_val_objects);
    } else {
        fprintf(stderr, "(WW) no objects\n");
    }

    int allPositiveFalse = 0, allPositiveTrue = 0, allNegativeFalse = 0, allNegativeTrue = 0;
    for (int i = 0; i < N_OBJECTS; i++) {
        allPositiveTrue += g_true_positive[i];
        allPositiveFalse += g_false_positive[i];
        allNegativeFalse += g_false_negative[i];
        allNegativeTrue += g_true_negative[i];
    }

    unsigned n_track_stars = 0, n_track_meteors = 0, n_track_noise = 0;
    tracking_count_objects(track_array, &n_track_stars, &n_track_meteors, &n_track_noise);
    unsigned n_val_objects = 0, n_val_stars = 0, n_val_meteors = 0, n_gt_noise = 0;
    n_val_objects = validation_count_objects(g_val_objects, g_n_val_objects, &n_val_stars, &n_val_meteors, &n_gt_noise);

    for (int i = 0; i < N_OBJECTS + 1; i++)
        tracking_rate[i] = (float)total_tracked_frames[i] / (float)total_gt_frames[i];

    printf("Statistics: \n");
    printf("  - Number of GT objs = ['meteor': %4d, 'star': %4d, 'noise': %4d, 'all': %4d]\n", n_val_meteors,
           n_val_stars, n_gt_noise, n_val_objects);
    printf("  - Number of tracks  = ['meteor': %4d, 'star': %4d, 'noise': %4d, 'all': %4lu]\n", n_track_meteors,
           n_track_stars, n_track_noise, track_array->_size);
    printf("  - True positives    = ['meteor': %4d, 'star': %4d, 'noise': %4d, 'all': %4d]\n", g_true_positive[METEOR],
           g_true_positive[STAR], g_true_positive[NOISE], allPositiveTrue);
    printf("  - False positives   = ['meteor': %4d, 'star': %4d, 'noise': %4d, 'all': %4d]\n", g_false_positive[METEOR],
           g_false_positive[STAR], g_false_positive[NOISE], allPositiveFalse);
    printf("  - True negative     = ['meteor': %4d, 'star': %4d, 'noise': %4d, 'all': %4d]\n", g_true_negative[METEOR],
           g_true_negative[STAR], g_true_negative[NOISE], allNegativeTrue);
    printf("  - False negative    = ['meteor': %4d, 'star': %4d, 'noise': %4d, 'all': %4d]\n", g_false_negative[METEOR],
           g_false_negative[STAR], g_false_negative[NOISE], allNegativeFalse);
    printf("  - tracking rate     = ['meteor': %4.2f, 'star': %4.2f, 'noise': %4.2f, 'all': %4.2f]\n",
           tracking_rate[METEOR], tracking_rate[STAR], tracking_rate[NOISE], tracking_rate[N_OBJECTS]);
}

void validation_free(void) {}

unsigned validation_count_objects(const validation_obj_t* val_objects, const unsigned n_val_objects, unsigned* n_stars,
                                  unsigned* n_meteors, unsigned* n_noise) {
    (*n_stars) = (*n_meteors) = (*n_noise) = 0;
    for (int i = 0; i < n_val_objects; i++)
        switch (val_objects[i].obj_type) {
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
            fprintf(stderr, "(EE) This should never happen ('val_objects[i].obj_type = %d', 'i = %d')\n",
                    val_objects[i].obj_type, i);
            exit(1);
        }
    return (*n_stars) + (*n_meteors) + (*n_noise);
}