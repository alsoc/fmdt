#include "Validation.h"
#include "Tracking.h"
#include "DebugUtil.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOLERANCE_DIRECTION 5
#define TOLERANCE_DISTANCEMIN 20 //8


static unsigned inputs_nb = 0;
static struct input* inputs = NULL;

static int positiveTrue = 0;
static int positiveFalse = 0;

int Validation_init(char* _inputs_file)
{
    assert(_inputs_file != NULL);

    FILE* file = fopen(_inputs_file, "r");
    if(!file) {
        return -1;
    } 

    if(fscanf(file, "%u\n", &inputs_nb) < 1 || inputs_nb<1)
    {
        VERBOSE (printf("[Validation] aucun meteore a suivre dans le fichier input donne !\n"); );
        return 0;
    }
    else
    { 
        VERBOSE (printf("[Validation] %4hu entrees dans le fichier d'input\n", (unsigned short)inputs_nb); );
    }
    
    inputs = (struct input*)malloc(inputs_nb * sizeof(struct input));
    
    int i=0;
    idisp(inputs_nb);
    while(i<inputs_nb && !feof(file))
    {
        if(fscanf(file, "%hu \t %f \t %f \t %hu \t %f \t %f \n", &inputs[i].t0, &inputs[i].x0, &inputs[i].y0, &inputs[i].t1, &inputs[i].x1, &inputs[i].y1))
        {
            inputs[i].t0_min = inputs[i].t0 - 5;
            inputs[i].t1_max = inputs[i].t1 + 5;

            inputs[i].a = (float)(inputs[i].y1-inputs[i].y0)/(float)(inputs[i].x1-inputs[i].x0);
            inputs[i].b = inputs[i].y1 - inputs[i].a * inputs[i].x1;
            
            VERBOSE (printf("[Validation] Input %-2d : t0=%-4d x0=%6.1f y0=%6.1f t1=%-4d x1=%6.1f y1=%6.1f\tf(x)=%-3.3f*x+%-3.3f\n", i, inputs[i].t0, inputs[i].x0, inputs[i].y0, inputs[i].t1, inputs[i].x1, inputs[i].y1, inputs[i].a, inputs[i].b); ); 

            inputs[i].track = NULL;
            inputs[i].xt = inputs[i].x0;
            inputs[i].yt = inputs[i].y0;

            inputs[i].nb_tracks     = 0;
            inputs[i].hits          = 0;
            inputs[i].hits          = 0; //tmp
            // inputs[i].is_valid      = 0;
            // inputs[i].is_valid_last = -1;

            inputs[i].dirX = inputs[i].x1 > inputs[i].x0; // vers la droite 
            inputs[i].dirY = inputs[i].y0 < inputs[i].y1; // vers le bas
            // idisp(inputs[i].dirY);
            // idisp(inputs[i].dirX);

            if(inputs[i].dirX){
                    if(inputs[i].dirY){
                        inputs[i].bb_y0   = inputs[i].y0 - TOLERANCE_DISTANCEMIN;   inputs[i].bb_x0   = inputs[i].x0 - TOLERANCE_DISTANCEMIN;
                        inputs[i].bb_y1   = inputs[i].y1 + TOLERANCE_DISTANCEMIN;   inputs[i].bb_x1   = inputs[i].x1 + TOLERANCE_DISTANCEMIN;
                    }else {
                        inputs[i].bb_y0   = inputs[i].y1 - TOLERANCE_DISTANCEMIN;   inputs[i].bb_x0   = inputs[i].x0 - TOLERANCE_DISTANCEMIN;
                        inputs[i].bb_y1   = inputs[i].y0 + TOLERANCE_DISTANCEMIN;   inputs[i].bb_x1   = inputs[i].x1 + TOLERANCE_DISTANCEMIN;
                    }
                    
            } else {
                    if(inputs[i].dirY) {
                        inputs[i].bb_y0   = inputs[i].y0 - TOLERANCE_DISTANCEMIN;   inputs[i].bb_x0   = inputs[i].x1 - TOLERANCE_DISTANCEMIN;
                        inputs[i].bb_y1   = inputs[i].y1 + TOLERANCE_DISTANCEMIN;   inputs[i].bb_x1   = inputs[i].x0 + TOLERANCE_DISTANCEMIN;
                    }else{
                        inputs[i].bb_y0   = inputs[i].y1 - TOLERANCE_DISTANCEMIN;   inputs[i].bb_x0   = inputs[i].x1 - TOLERANCE_DISTANCEMIN;
                        inputs[i].bb_y1   = inputs[i].y0 + TOLERANCE_DISTANCEMIN;   inputs[i].bb_x1   = inputs[i].x0 + TOLERANCE_DISTANCEMIN;
                    }


            }
            i++;
        }
    }
    inputs_nb = i; // petit risque de memoire pas utilisee
    
    fclose(file);
    
    return inputs_nb;
}

void Validation_print()
{
    unsigned int nb_tracks = 0;
    //float tracking_rate;
    if(inputs)
    {
        printf("# -----||--------------||---------------||--------\n");
        printf("#  Obj ||     Hits     ||   GT frames   || Tracks \n");
        printf("# -----||--------------||---------------||--------\n");
        printf("# -----||--------|-----||-------|-------||--------\n");
        printf("#   Id || Detect |  GT || Start |  Stop ||      # \n");
        printf("# -----||--------|-----||-------|-------||--------\n");
        for(int i=0;i<inputs_nb;i++)
        {
            int expected_hits = inputs[i].t1-inputs[i].t0+1;
            nb_tracks += inputs[i].nb_tracks;

            // tmp
            if (inputs[i].hits== 1) inputs[i].hits = 0;
            // VERBOSE (printf("[Validation] Input %-2d : hits = %d/%d \t nb_tracks = %3d \t %4d \t %4d\n", i, inputs[i].hits, expected_hits, inputs[i].nb_tracks, inputs[i].t0, inputs[i].t1 ); );
            VERBOSE (printf("[Validation] Input %-2d : hits = %d/%d \t nb_tracks = %3d \t %4d \t %4d \t %4d \t %4d \t %6.1f \t %6.1f \t %6.1f \t %6.1f\n", i, inputs[i].hits, expected_hits, inputs[i].nb_tracks, inputs[i].t0, inputs[i].t1, inputs[i].track_t0, inputs[i].track_t1, inputs[i].track_x0, inputs[i].track_y0 ,inputs[i].track_x1, inputs[i].track_y1 ); );

            printf("   %3d ||    %3d | %3d || %5d | %5d ||  %5d  \n", i, inputs[i].hits, expected_hits, inputs[i].t0, inputs[i].t1, inputs[i].nb_tracks);
        }
        free(inputs);
    } else {
        fprintf(stderr, "(WW) no objects\n");
    }

    printf("Statistics: \n");
    printf("  - Number of obj (GT) = %3d\n", inputs_nb);
    printf("  - Number of tracks   = %3d\n", nb_tracks);
    printf("  - True positives     = %3d\n", positiveTrue);
    printf("  - False positives    = %3d\n", positiveFalse);
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
        
        if (track->timestamp == 0) continue;

        ValidationInput* input = NULL;
        for(int i = 0; i < inputs_nb; i++) {
            if(inputs[i].t0_min <= track->timestamp && track->timestamp+track->time <= inputs[i].t1_max &&
               inputs[i].bb_x0 <= track->begin.x && track->end.x <= inputs[i].bb_x1 &&
               inputs[i].bb_y0 <= track->begin.y && track->end.y <= inputs[i].bb_y1) {
                input = &inputs[i];

                inputs[i].track_t0 = track->timestamp;
                inputs[i].track_t1 = track->timestamp+track->time;
                inputs[i].track_x0 = track->begin.x;
                inputs[i].track_y0 = track->begin.y;
                inputs[i].track_x1 = track->end.x;
                inputs[i].track_y1 = track->end.y;

                if (inputs[i].nb_tracks == 0)
                    break; // maybe
            }
        }

        // Piste matche avec un input
        if(input) {
            input->nb_tracks++;
            input->hits = track->time + input->hits + 1;
            track->is_valid = 1;
            positiveTrue++;
        } else { // Piste ne matche pas avec input
            positiveFalse++;
        }

    }

}