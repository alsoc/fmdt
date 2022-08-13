#include "Validation.h"
#include "Tracking.h"
#include "DebugUtil.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOLERANCE_DIRECTION 5
#define TOLERANCE_DISTANCEMIN 20 //8


static unsigned g_inputs_nb = 0;
static struct input* g_inputs = NULL;

static int g_positiveTrue [N_OBJ_TYPES] = {0};
static int g_positiveFalse[N_OBJ_TYPES] = {0};
static int g_negativeTrue [N_OBJ_TYPES] = {0};
static int g_negativeFalse[N_OBJ_TYPES] = {0};

int Validation_init(char* _inputs_file)
{
    assert(_inputs_file != NULL);

    FILE* file = fopen(_inputs_file, "r");
    if(!file) {
        return -1;
    } 

    // count the number of lines
    char tmp_obj_type[1024];
    sint16 tmp_t0;
    float32 tmp_x0;
    float32 tmp_y0;
    sint16 tmp_t1;
    float32 tmp_x1;
    float32 tmp_y1;
    g_inputs_nb = 0;
    while(!feof(file) && fscanf(file, "%s %hu \t %f \t %f \t %hu \t %f \t %f \n", tmp_obj_type, &tmp_t0, &tmp_x0, &tmp_y0, &tmp_t1, &tmp_x1, &tmp_y1))
        g_inputs_nb++;
    fseek(file, 0, SEEK_SET);

    if(g_inputs_nb < 1)
    {
        VERBOSE (fprintf(stderr, "(DBG) [Validation] aucun meteore a suivre dans le fichier input donne !\n"); );
        return 0;
    }
    else
    {
        VERBOSE (fprintf(stderr, "(DBG) [Validation] %4hu entrees dans le fichier d'input\n", (unsigned short)g_inputs_nb); );
    }

    g_inputs = (struct input*)malloc(g_inputs_nb * sizeof(struct input));
    
    int i=0;
    idisp(g_inputs_nb);
    while(i<g_inputs_nb && !feof(file))
    {
        if(fscanf(file, "%s %hu \t %f \t %f \t %hu \t %f \t %f \n", tmp_obj_type, &g_inputs[i].t0, &g_inputs[i].x0, &g_inputs[i].y0, &g_inputs[i].t1, &g_inputs[i].x1, &g_inputs[i].y1))
        {
            g_inputs[i].t0_min = g_inputs[i].t0 - 5;
            g_inputs[i].t1_max = g_inputs[i].t1 + 5;

            g_inputs[i].a = (float)(g_inputs[i].y1-g_inputs[i].y0)/(float)(g_inputs[i].x1-g_inputs[i].x0);
            g_inputs[i].b = g_inputs[i].y1 - g_inputs[i].a * g_inputs[i].x1;
            
            VERBOSE (fprintf(stderr, "(DBG) [Validation] Input %-2d : t0=%-4d x0=%6.1f y0=%6.1f t1=%-4d x1=%6.1f y1=%6.1f\tf(x)=%-3.3f*x+%-3.3f\n", i, g_inputs[i].t0, g_inputs[i].x0, g_inputs[i].y0, g_inputs[i].t1, g_inputs[i].x1, g_inputs[i].y1, g_inputs[i].a, g_inputs[i].b); );

            g_inputs[i].track = NULL;
            g_inputs[i].xt = g_inputs[i].x0;
            g_inputs[i].yt = g_inputs[i].y0;

            g_inputs[i].nb_tracks     = 0;
            g_inputs[i].hits          = 0;
            g_inputs[i].hits          = 0; //tmp
            // g_inputs[i].is_valid      = 0;
            // g_inputs[i].is_valid_last = -1;

            g_inputs[i].dirX = g_inputs[i].x1 > g_inputs[i].x0; // vers la droite
            g_inputs[i].dirY = g_inputs[i].y0 < g_inputs[i].y1; // vers le bas
            // idisp(g_inputs[i].dirY);
            // idisp(g_inputs[i].dirX);

            if(g_inputs[i].dirX){
                if(g_inputs[i].dirY){
                    g_inputs[i].bb_y0 = g_inputs[i].y0 - TOLERANCE_DISTANCEMIN; g_inputs[i].bb_x0 = g_inputs[i].x0 - TOLERANCE_DISTANCEMIN;
                    g_inputs[i].bb_y1 = g_inputs[i].y1 + TOLERANCE_DISTANCEMIN; g_inputs[i].bb_x1 = g_inputs[i].x1 + TOLERANCE_DISTANCEMIN;
                }else {
                    g_inputs[i].bb_y0 = g_inputs[i].y1 - TOLERANCE_DISTANCEMIN; g_inputs[i].bb_x0 = g_inputs[i].x0 - TOLERANCE_DISTANCEMIN;
                    g_inputs[i].bb_y1 = g_inputs[i].y0 + TOLERANCE_DISTANCEMIN; g_inputs[i].bb_x1 = g_inputs[i].x1 + TOLERANCE_DISTANCEMIN;
                }
            } else {
                if(g_inputs[i].dirY) {
                    g_inputs[i].bb_y0 = g_inputs[i].y0 - TOLERANCE_DISTANCEMIN; g_inputs[i].bb_x0 = g_inputs[i].x1 - TOLERANCE_DISTANCEMIN;
                    g_inputs[i].bb_y1 = g_inputs[i].y1 + TOLERANCE_DISTANCEMIN; g_inputs[i].bb_x1 = g_inputs[i].x0 + TOLERANCE_DISTANCEMIN;
                }else{
                    g_inputs[i].bb_y0 = g_inputs[i].y1 - TOLERANCE_DISTANCEMIN; g_inputs[i].bb_x0 = g_inputs[i].x1 - TOLERANCE_DISTANCEMIN;
                    g_inputs[i].bb_y1 = g_inputs[i].y0 + TOLERANCE_DISTANCEMIN; g_inputs[i].bb_x1 = g_inputs[i].x0 + TOLERANCE_DISTANCEMIN;
                }
            }

            if(!strcmp(tmp_obj_type, "noise"))
                g_inputs[i].obj_type = NOISE;
            else if(!strcmp(tmp_obj_type, "meteor"))
                g_inputs[i].obj_type = METEOR;
            else if(!strcmp(tmp_obj_type, "star"))
                g_inputs[i].obj_type = STAR;
            else
                g_inputs[i].obj_type = UNKNOWN;
            i++;
        }
    }
    g_inputs_nb = i; // petit risque de memoire pas utilisee
    
    fclose(file);
    
    return g_inputs_nb;
}

void Validation_print(const Track* tracks, const int tracks_nb)
{
    char str_unknown[128] = "unknown";
    char str_noise  [128] = "  noise";
    char str_meteor [128] = " meteor";
    char str_star   [128] = "   star";

    char* type_lut[N_OBJ_TYPES];
    type_lut[METEOR]  = str_meteor;
    type_lut[STAR]    = str_star;
    type_lut[NOISE]   = str_noise;
    type_lut[UNKNOWN] = str_unknown;

    float tracking_rate[N_OBJ_TYPES +1];
    unsigned total_tracked_frames[N_OBJ_TYPES +1] = {0};
    unsigned total_gt_frames[N_OBJ_TYPES +1] = {0};
    if(g_inputs)
    {
        printf("# ---------------||--------------||---------------||--------\n");
        printf("#    GT Object   ||     Hits     ||   GT Frames   || Tracks \n");
        printf("# ---------------||--------------||---------------||--------\n");
        printf("# -----|---------||--------|-----||-------|-------||--------\n");
        printf("#   Id |    Type || Detect |  GT || Start |  Stop ||      # \n");
        printf("# -----|---------||--------|-----||-------|-------||--------\n");
        for(int i=0;i<g_inputs_nb;i++)
        {
            int expected_hits = g_inputs[i].t1-g_inputs[i].t0+1;

            // tmp
            if (g_inputs[i].hits== 1) g_inputs[i].hits = 0; // TODO: what is this?!
            VERBOSE (fprintf(stderr, "(DBG) [Validation] Input %-2d : hits = %d/%d \t nb_tracks = %3d \t %4d \t %4d \t %4d \t %4d \t %6.1f \t %6.1f \t %6.1f \t %6.1f\n", i, g_inputs[i].hits, expected_hits, g_inputs[i].nb_tracks, g_inputs[i].t0, g_inputs[i].t1, g_inputs[i].track_t0, g_inputs[i].track_t1, g_inputs[i].track_x0, g_inputs[i].track_y0, g_inputs[i].track_x1, g_inputs[i].track_y1 ); );
            printf("   %3d | %s ||    %3d | %3d || %5d | %5d ||  %5d  \n", i, type_lut[g_inputs[i].obj_type], g_inputs[i].hits, expected_hits, g_inputs[i].t0, g_inputs[i].t1, g_inputs[i].nb_tracks);

            unsigned tmp = (g_inputs[i].hits <= expected_hits) ? g_inputs[i].hits : expected_hits - (g_inputs[i].hits - expected_hits);
            total_gt_frames[g_inputs[i].obj_type] += expected_hits;
            total_tracked_frames[g_inputs[i].obj_type] += tmp;
            total_gt_frames[N_OBJ_TYPES] += expected_hits;
            total_tracked_frames[N_OBJ_TYPES] += tmp;
        }
        free(g_inputs);
    } else {
        fprintf(stderr, "(WW) no objects\n");
    }

    int allPositiveFalse = 0;
    int allPositiveTrue = 0;
    int allNegativeFalse = 0;
    int allNegativeTrue = 0;
    for (int i = 0; i < N_OBJ_TYPES; i++) {
        allPositiveTrue  += g_positiveTrue[i];
        allPositiveFalse += g_positiveFalse[i];
        allNegativeFalse += g_negativeFalse[i];
        allNegativeTrue += g_negativeTrue[i];
    }

    unsigned n_tracks = 0, n_track_stars = 0, n_track_meteors = 0, n_track_noise = 0;
    n_tracks = track_count_objects(tracks, tracks_nb, &n_track_stars, &n_track_meteors, &n_track_noise);

    unsigned n_gt_objs = 0, n_gt_stars = 0, n_gt_meteors = 0, n_gt_noise = 0;
    n_gt_objs = gt_count_objects(g_inputs, g_inputs_nb, &n_gt_stars, &n_gt_meteors, &n_gt_noise);

    for (int i = 0; i < N_OBJ_TYPES +1; i++)
        tracking_rate[i] = (float)total_tracked_frames[i] / (float)total_gt_frames[i];

    printf("Statistics: \n");
    printf("  - Number of GT objs = ['meteor': %4d, 'star': %4d, 'noise': %4d, 'all': %4d]\n", n_gt_meteors,            n_gt_stars,            n_gt_noise,             n_gt_objs       );
    printf("  - Number of tracks  = ['meteor': %4d, 'star': %4d, 'noise': %4d, 'all': %4d]\n", n_track_meteors,         n_track_stars,         n_track_noise,          n_tracks        );
    printf("  - True positives    = ['meteor': %4d, 'star': %4d, 'noise': %4d, 'all': %4d]\n", g_positiveTrue [METEOR], g_positiveTrue [STAR], g_positiveTrue [NOISE], allPositiveTrue );
    printf("  - False positives   = ['meteor': %4d, 'star': %4d, 'noise': %4d, 'all': %4d]\n", g_positiveFalse[METEOR], g_positiveFalse[STAR], g_positiveFalse[NOISE], allPositiveFalse);
    printf("  - True negative     = ['meteor': %4d, 'star': %4d, 'noise': %4d, 'all': %4d]\n", g_negativeTrue [METEOR], g_negativeTrue [STAR], g_negativeTrue [NOISE], allNegativeTrue );
    printf("  - False negative    = ['meteor': %4d, 'star': %4d, 'noise': %4d, 'all': %4d]\n", g_negativeFalse[METEOR], g_negativeFalse[STAR], g_negativeFalse[NOISE], allNegativeFalse);
    printf("  - Tracking rate     = ['meteor': %4.2f, 'star': %4.2f, 'noise': %4.2f, 'all': %4.2f]\n", tracking_rate[METEOR], tracking_rate[STAR], tracking_rate[NOISE], tracking_rate[N_OBJ_TYPES]);
}

void Validation_free(void)
{
}

void Validation(Track* tracks, int tracks_nb)
{
    Track* track;
    idisp(tracks_nb);
    for(int t = 0; t < tracks_nb; t++) {
        track = &tracks[t];
        ValidationInput* input = NULL;
        for(int i = 0; i < g_inputs_nb; i++) {
            if(g_inputs[i].t0_min <= track->timestamp && track->timestamp+track->time <= g_inputs[i].t1_max &&
               g_inputs[i].bb_x0 <= track->begin.x && track->end.x <= g_inputs[i].bb_x1 &&
               g_inputs[i].bb_y0 <= track->begin.y && track->end.y <= g_inputs[i].bb_y1 &&
               track->obj_type == g_inputs[i].obj_type) {
#ifdef ENABLE_DEBUG
                g_inputs[i].track_t0 = track->timestamp;
                g_inputs[i].track_t1 = track->timestamp+track->time;
                g_inputs[i].track_x0 = track->begin.x;
                g_inputs[i].track_y0 = track->begin.y;
                g_inputs[i].track_x1 = track->end.x;
                g_inputs[i].track_y1 = track->end.y;
#endif
                input = &g_inputs[i];
                if (g_inputs[i].nb_tracks == 0)
                    break; // maybe
            }
        }

        // Piste matche avec un input
        if(input) {
            input->nb_tracks++;
            input->hits = track->time + input->hits +1;
            g_positiveTrue[track->obj_type]++;
            if (track->obj_type == METEOR)
                track->is_valid = 1;
        } else { // Piste ne matche pas avec input
            g_positiveFalse[track->obj_type]++;
            if (track->obj_type == METEOR)
                track->is_valid = 2;
        }
    }

    for(int i = 0; i < g_inputs_nb; i++)
        if (!g_inputs[i].nb_tracks)
            g_negativeFalse[g_inputs[i].obj_type]++;

    for(int t = 0; t < tracks_nb; t++)
        for (int ot = 1; ot < N_OBJ_TYPES; ot++)
            if (ot != tracks[t].obj_type)
                g_negativeTrue[ot]++;
}

// ---------------------------------------------------------------------------------------------------
unsigned gt_count_objects(const ValidationInput* gt_objs, const unsigned n_gt_objs, unsigned *n_stars, unsigned *n_meteors, unsigned *n_noise)
// ---------------------------------------------------------------------------------------------------
{
    (*n_stars) = (*n_meteors) = (*n_noise) = 0;
    for(int i = 0; i < n_gt_objs; i++)
        switch (gt_objs[i].obj_type){
            case STAR:   (*n_stars  )++; break;
            case METEOR: (*n_meteors)++; break;
            case NOISE:  (*n_noise  )++; break;
            default:
                fprintf(stderr, "(EE) This should never happen ('gt_objs[i].obj_type = %d', 'i = %d')\n", gt_objs[i].obj_type, i);
                exit(1);
        }
    return (*n_stars) + (*n_meteors) + (*n_noise);
}