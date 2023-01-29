#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "fmdt/macros.h"
#include "vec.h"

#include "fmdt/tracking/tracking_global.h"
#include "fmdt/validation/validation_struct.h"
#include "fmdt/validation/validation_global.h"
#include "fmdt/validation/validation_io.h"

unsigned validation_count_objects(const validation_obj_t* val_objects, const unsigned n_val_objects, unsigned* n_stars,
                                  unsigned* n_meteors, unsigned* n_noise) {
    (*n_stars) = (*n_meteors) = (*n_noise) = 0;
    for (unsigned i = 0; i < n_val_objects; i++)
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

void validation_print(const vec_track_t track_array) {
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
        for (unsigned i = 0; i < g_n_val_objects; i++) {
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

    size_t n_tracks = vector_size(track_array);

    printf("Statistics: \n");
    printf("  - Number of GT objs = ['meteor': %4d, 'star': %4d, 'noise': %4d, 'all': %4d]\n", n_val_meteors,
           n_val_stars, n_gt_noise, n_val_objects);
    printf("  - Number of tracks  = ['meteor': %4d, 'star': %4d, 'noise': %4d, 'all': %4lu]\n", n_track_meteors,
           n_track_stars, n_track_noise, (unsigned long)n_tracks);
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
