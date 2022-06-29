/**
 * Copyright (c) 2021-2022, Clara CIOCAN, LIP6 Sorbonne University
 * Copyright (c) 2021-2022, Mathuran KANDEEPAN, LIP6 Sorbonne University
 */
#include "nrutil.h"
#include "Args.h"
#include "Video.h"
#include "CCL.h"
#include "Features.h"
#include "Threshold.h"
#include "DebugUtil.h"
#include "macro_debug.h"
#include "Tracking.h"

#include "ffmpeg-io/reader.h"
#include "ffmpeg-io/writer.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdio.h>

#define SIZE_BUF 20
#define R 10



static Buf buffer[SIZE_BUF];

extern uint32 **nearest;
extern float32 **distances;
extern uint32 *conflicts;
// ---------------------------------------------------------------------------------------------------
void init_Track(Track *tracks, int n)
// ---------------------------------------------------------------------------------------------------
{
    for (int i = 0; i < n ; i++){
        tracks[i].timestamp = 0;
        tracks[i].time      = 0;
        tracks[i].state  = 0;
        tracks[i].x = 0;
        tracks[i].y = 0;
        tracks[i].rx = 0;
        tracks[i].ry = 0;
        tracks[i].bb_x      = 0;
        tracks[i].bb_y      = 0;
    }
}

// ---------------------------------------------------------------------------------------------------
void clear_index_Track(Track *tracks, int i)
// ---------------------------------------------------------------------------------------------------
{
    tracks[i].timestamp = 0;
    tracks[i].time      = 0;
    tracks[i].state     = 0;
    tracks[i].x         = 0;
    tracks[i].y         = 0;
    tracks[i].rx = 0;
    tracks[i].ry = 0;
    tracks[i].bb_x      = 0;
    tracks[i].bb_y      = 0;

}

// ---------------------------------------------------------------------------------------------------
void clear_buffer()
// ---------------------------------------------------------------------------------------------------
{
    MeteorROI *b; 
    for(int i = 0; i < SIZE_BUF; i++){
        b = &buffer[i].stats;
        b->ID   = 0;
        b->S    = 0;
        b->x    = 0;
        b->y    = 0;
        b->Sx   = 0;
        b->Sy   = 0;
        b->Sx2  = 0;
        b->Sy2  = 0;
        b->Sxy  = 0;
        b->xmin = 0;
        b->ymin = 0;
        b->xmax = 0;
        b->ymax = 0;
        b->dx   = 0.0;
        b->dy   = 0.0;
        b->error= 0.0;
        b->prev = 0;
        b->next = 0;
        b->time = 0;
        b->motion = 0;
        
        buffer[i].frame = 0;
    }        
}

// ---------------------------------------------------------------------------------------------------
void clear_index_buffer(int i)
// ---------------------------------------------------------------------------------------------------
{
    MeteorROI *b = &buffer[i].stats;
    b->ID   = 0;
    b->S    = 0;
    b->x    = 0;
    b->y    = 0;
    b->Sx   = 0;
    b->Sy   = 0;
    b->Sx2  = 0;
    b->Sy2  = 0;
    b->Sxy  = 0;
    b->xmin = 0;
    b->ymin = 0;
    b->xmax = 0;
    b->ymax = 0;
    b->dx   = 0.0;
    b->dy   = 0.0;
    b->error= 0.0;
    b->prev = 0;
    b->next = 0;
    b->time = 0;
    b->motion = 0;
    buffer[i].frame = 0;
}



// ---------------------------------------------------------------------------------------------------
void update_buffer(int frame){
// ---------------------------------------------------------------------------------------------------
    int diff;
    for(int i = 0; i < SIZE_BUF; i++){
        if(buffer[i].frame != 0){
            diff = frame - buffer[i].frame;
            if (diff >= 2){
                clear_index_buffer(i);
            }
        }
    }  

}



// ---------------------------------------------------------------------------------------------------
void insert_buffer(MeteorROI stats, int frame)
// ---------------------------------------------------------------------------------------------------
{
    MeteorROI *b;

    for (int i = 0; i < SIZE_BUF; i++){
        b = &buffer[i].stats;
        if (buffer[i].stats.ID == 0){   
            b->ID   = stats.ID;
            b->S    = stats.S;
            b->x    = stats.x;
            b->y    = stats.y;
            b->Sx   = stats.Sx;
            b->Sy   = stats.Sy;
            b->Sx2  = stats.Sx2;
            b->Sy2  = stats.Sy2;
            b->Sxy  = stats.Sxy;
            b->xmin = stats.xmin;
            b->ymin = stats.ymin;
            b->xmax = stats.xmax;
            b->ymax = stats.ymax;
            b->dx   = stats.dx;
            b->dy   = stats.dy;
            b->error= stats.error;
            b->prev = stats.prev;
            b->next = stats.next;
            b->time = stats.time;
            b->motion = stats.motion;
            buffer[i].frame = frame;
            return;
        }
    }
}


// -----------------------------------------------------
int search_buf_stat(MeteorROI stats, int frame)
// -----------------------------------------------------
{
    int n = stats.prev;
    for (int i = 0; i < SIZE_BUF; i++){
        if(frame  == buffer[i].frame + 1 && n == buffer[i].stats.ID){
            return i;
        }
    }  
    return -1;
}


// -----------------------------------------------------
static void Track_extrapolate(Track *t, int theta, int tx, int ty)
// -----------------------------------------------------
{   
    float32 u,v;
    float32 x,y;
    t->state = TRACK_LOST;
    // compensation du mouvement + calcul vitesse entre t-1 et t
    u = t->end.x-t->end.dx - t->x; 
    v = t->end.y-t->end.dy - t->y; 

    x = tx + t->end.x * cos(theta) - t->end.y * sin(theta);
    y = ty + t->end.x * sin(theta) + t->end.y * cos(theta);

    t->x = x + u;
    t->y = y + v;    

    fdisp(t->x);
    fdisp(t->y);
}


static char* state2char(int state)
{
    switch(state)
    {
        case TRACK_NEW:
            return "N";
        case TRACK_UPDATED:
            return "U";
        case TRACK_EXTRAPOLATED:
            return "E";
        case TRACK_FINISHED:
            return "F";
        case TRACK_LOST:
            return "L";
    }
    return "?";
}


void update_bounding_box(Track* track, MeteorROI stats)
{
    PUTS("UPADTAE BB");
    idisp(stats.xmin);
    idisp(stats.xmax);
    track->bb_x      = (uint16)ceil((double)((stats.xmin + stats.xmax))/2);
    track->bb_y      = (uint16)ceil((double)((stats.ymin + stats.ymax))/2);
    track->rx        = track->bb_x - stats.xmin + 5;
    track->ry        = track->bb_y - stats.ymin + 5;
}

// -----------------------------------------------------
void updateTrack(Track *tracks, MeteorROI *stats0, MeteorROI *stats1, int nc1, int frame, int *offset, int *last, int theta, int tx, int ty)
// -----------------------------------------------------
{
    int next;
    int i;

    for (i = *offset; i <= *last; i++){
        next = tracks[i].end.next;
        if(!next){
            *offset = i;
            break; 
        }
    }
    for (i = *offset; i <= *last; i++){
        if(tracks[i].time > 200){
            clear_index_Track(tracks, i);
            continue;
        }
        idisp(tracks[i].time);
        disp(state2char(tracks[i].state));
        if(tracks[i].time  && tracks[i].state != TRACK_FINISHED ){

            if(tracks[i].state == TRACK_EXTRAPOLATED){
                PUTS("TRACK_EXTRAPOLATED");
                for(int j = 1; j <= nc1; j++){
                        if ( (stats0[j].x > tracks[i].x - R) && (stats0[j].x < tracks[i].x + R) && (stats0[j].y < tracks[i].y + R) && (stats0[j].y > tracks[i].y - R)){
                            idisp(stats1[j].ID);
                            tracks[i].end = stats0[j];
                            tracks[i].state = TRACK_UPDATED;
                            update_bounding_box(tracks+i, stats0[j]);
                            // tracks[i].time++;
                        }
                }
            }


            if(tracks[i].state == TRACK_LOST){
                PUTS("TRACK_LOST");
                for(int j = 1; j <= nc1; j++){
                    if (!stats1[j].prev){
                        if ( (stats1[j].x > tracks[i].x - R) && (stats1[j].x < tracks[i].x + R) && (stats1[j].y < tracks[i].y + R) && (stats1[j].y > tracks[i].y - R)){
                            tracks[i].state = TRACK_EXTRAPOLATED;
                            tracks[i].time+=2;
                            stats1[j].state = 1;
                        }
                    }
                }
                if (tracks[i].state != TRACK_EXTRAPOLATED){
                    PUTS("FINISHEd");
                    tracks[i].state = TRACK_FINISHED;
                }
        
            }
            if(tracks[i].state == TRACK_UPDATED || tracks[i].state == TRACK_NEW){
                next = stats0[tracks[i].end.ID].next;
                if(next){
                    tracks[i].x = tracks[i].end.x;
                    tracks[i].y = tracks[i].end.y;
                    tracks[i].end = stats1[next];
                    tracks[i].time++;
                    update_bounding_box(tracks+i, stats1[next]);

                } 
                else{
                    //on extrapole si pas finished
                    Track_extrapolate(&tracks[i], theta, tx, ty);
                    // tracks[i].state = TRACK_FINISHED;
                }
            }
        }
    }


}


// à modifier pour optimisation
// -----------------------------------------------------
void insert_new_track(MeteorROI last_stats, Track *tracks, int *last, int frame, int i)
// -----------------------------------------------------
{
    Track *track = &tracks[++(*last)];

    if (i == -1) return;

    track->begin     =  buffer[i].stats;
    track->end       = last_stats; 
    track->time      = 1; 
    track->timestamp = frame - 2;
    track->state = TRACK_NEW;
    update_bounding_box(track, last_stats);
    

}



// -----------------------------------------------------
void Tracking(MeteorROI *stats0, MeteorROI *stats1, Track *tracks, int nc0, int nc1, int frame, int *last, int *offset, int theta, int tx, int ty)
// -----------------------------------------------------
{
    int j;

    double errMoy = errorMoy(stats0, nc0);
    double eType = ecartType(stats0, nc0, errMoy);

    for(int i = 1; i <= nc0; i++){
        float32 e = stats0[i].error;
        
        int asso = stats0[i].next;

        // si mouvement detecté
        if (fabs(e-errMoy) >  1.5 * eType && asso){
            
            if (stats0[i].state) {
                PUTS("EXTRAPOLATEED");
                continue; // Extrapolated
            }

            stats0[i].motion = 1; // debug
            stats0[i].time++;
            stats1[stats0[i].next].time = stats0[i].time ;
            idisp(stats0[i].time);
            if(stats0[i].time == 1){
                // stocker dans un buf pour savoir si au moins sur 3 frames
                insert_buffer(stats0[i], frame);

            }
            if(stats0[i].time == 2){ 
                // mouvement sur 3 frames donc création de track + suppression du buff    
                for(j = *offset; j <= *last; j++){
                    if(tracks[j].end.ID == stats0[i].ID && tracks[j].end.x == stats0[i].x ){
                        PUTS("BREAK");
                        break;
                    }
                }   
                // idisp(j);
                int k = search_buf_stat(stats0[i], frame);
                idisp(k);
                if(j == *last + 1 || *last == -1){
                    // insertion seulement si dans le buffer (k != -1)
                    insert_new_track(stats0[i], tracks, last, frame, k);
                    printTracks(tracks, *last);
                    idisp(stats0[i].ID);
                }
                clear_index_buffer(k);
                
            }
        } 
    }

    // parcourir les track et update si besoin
    updateTrack(tracks, stats0, stats1, nc1, frame, offset, last, theta, tx, ty);

    // clear/update le buffer
    update_buffer(frame);

    // printBuffer(buffer, 10);
    // printTracks(tracks, *last);
    // printStats(stats0, nc0);
    // printStats(stats1,nc1);
}

int TrackStars(MeteorROI *stats0, MeteorROI *stats1, Track *tracks, int nc0, int nc1, int frame, int *last, int *offset)
{
    int cpt = 0;

    for(int i = 1; i <= nc0; i++){
        stats0[i].time++;
        stats1[stats0[i].next].time = stats0[i].time ;
        if(stats0[i].time == 1){
            cpt++;
        }
    }

    // parcourir les track et update si besoin
    // ck(tracks, stats0, stats1, nc1, frame, offset, last);
    return cpt;
}