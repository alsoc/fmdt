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

#define SIZE_BUF 10000
#define INF 9999999


static Buf buffer[SIZE_BUF];
elemBB* tabBB[NB_FRAMES];

extern uint32 **nearest;
extern float32 **distances;
extern uint32 *conflicts;
extern char path_bounding_box[200];


void initTabBB()
{
    for(int i = 0; i < NB_FRAMES; i++){
        tabBB[i] = NULL;
    }
}

static void addToList(uint16 rx, uint16 ry, uint16 bb_x, uint16 bb_y, int frame)
{

    elemBB *new = malloc(sizeof(elemBB));
    new->rx = rx;
    new->ry = ry;
    new->bb_x = bb_x;
    new->bb_y = bb_y;
    new->next = tabBB[frame];
    *(tabBB+frame) = new;

}

// ---------------------------------------------------------------------------------------------------
void init_Track(Track *tracks, int n)
// ---------------------------------------------------------------------------------------------------
{
    for (int i = 0; i < n ; i++){
        tracks[i].timestamp = 0;
        tracks[i].time      = 0;
        tracks[i].state     = 0;
        tracks[i].x         = 0;
        tracks[i].y         = 0;
        tracks[i].rx        = 0;
        tracks[i].ry        = 0;
        tracks[i].bb_x      = 0;
        tracks[i].bb_y      = 0;
        tracks[i].is_valid  = 0;
        tracks[i].is_meteor = 0;
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
    tracks[i].rx        = 0;
    tracks[i].ry        = 0;
    tracks[i].bb_x      = 0;
    tracks[i].bb_y      = 0;
    tracks[i].is_valid  = 0;
    tracks[i].is_meteor  = 0;

}

// ---------------------------------------------------------------------------------------------------
void clear_buffer()
// ---------------------------------------------------------------------------------------------------
{
    MeteorROI *b; 
    for(int i = 0; i < SIZE_BUF; i++){
        b = &buffer[i].stats0;
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
    MeteorROI *b = &buffer[i].stats0;
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
void insert_buffer(MeteorROI stats0, MeteorROI stats1, int frame)
// ---------------------------------------------------------------------------------------------------
{
    MeteorROI *b;
    MeteorROI *b1;

    for (int i = 0; i < SIZE_BUF; i++){
        b = &buffer[i].stats0;
        b1 = &buffer[i].stats1;
        if (buffer[i].stats0.ID == 0){   
            b->ID     = stats0.ID;       b1->ID     = stats1.ID;
            b->S      = stats0.S;        b1->S      = stats1.S;
            b->x      = stats0.x;        b1->x      = stats1.x;
            b->y      = stats0.y;        b1->y      = stats1.y;
            b->Sx     = stats0.Sx;       b1->Sx     = stats1.Sx;
            b->Sy     = stats0.Sy;       b1->Sy     = stats1.Sy;
            b->Sx2    = stats0.Sx2;      b1->Sx2    = stats1.Sx2;
            b->Sy2    = stats0.Sy2;      b1->Sy2    = stats1.Sy2;
            b->Sxy    = stats0.Sxy;      b1->Sxy    = stats1.Sxy;
            b->xmin   = stats0.xmin;     b1->xmin   = stats1.xmin;
            b->ymin   = stats0.ymin;     b1->ymin   = stats1.ymin;
            b->xmax   = stats0.xmax;     b1->xmax   = stats1.xmax;
            b->ymax   = stats0.ymax;     b1->ymax   = stats1.ymax;
            b->dx     = stats0.dx;       b1->dx     = stats1.dx;
            b->dy     = stats0.dy;       b1->dy     = stats1.dy;
            b->error  = stats0.error;    b1->error  = stats1.error;
            b->prev   = stats0.prev;     b1->prev   = stats1.prev;
            b->next   = stats0.next;     b1->next   = stats1.next;
            b->time   = stats0.time;     b1->time   = stats1.time;
            b->motion = stats0.motion; b1->motion = stats1.motion;
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
        if(frame  == buffer[i].frame + 1 && n == buffer[i].stats0.ID){
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


void update_bounding_box(Track* track, MeteorROI stats, int frame)
{
    PUTS("UPADTAE BB");
    idisp(stats.xmin);
    idisp(stats.xmax);
    uint16 rx, ry, bb_x, bb_y;

    // juste pour debug (affichage)
    track->bb_x      = (uint16)ceil((double)((stats.xmin + stats.xmax))/2);
    track->bb_y      = (uint16)ceil((double)((stats.ymin + stats.ymax))/2);
    track->rx        = track->bb_x - stats.xmin + 5;
    track->ry        = track->bb_y - stats.ymin + 5;

    bb_x      = (uint16)ceil((double)((stats.xmin + stats.xmax))/2);
    bb_y      = (uint16)ceil((double)((stats.ymin + stats.ymax))/2);
    rx        = bb_x - stats.xmin + 5;
    ry        = bb_y - stats.ymin + 5;

    addToList(rx, ry, bb_x, bb_y, frame-1);

}

// -----------------------------------------------------
void updateTrack(Track *tracks, MeteorROI *stats0, MeteorROI *stats1, int nc1, int frame, int *offset, int *last, int theta, int tx, int ty, int r_extrapol, int d_line)
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
        if(tracks[i].time > 150){
            clear_index_Track(tracks, i);
            continue;
        }
        idisp(tracks[i].time);
        disp(state2char(tracks[i].state));
        if(tracks[i].time  && tracks[i].state != TRACK_FINISHED ){

            if(tracks[i].state == TRACK_EXTRAPOLATED){
                PUTS("TRACK_EXTRAPOLATED");
                for(int j = 1; j <= nc1; j++){
                        if ( (stats0[j].x > tracks[i].x - r_extrapol) && (stats0[j].x < tracks[i].x + r_extrapol) && (stats0[j].y < tracks[i].y + r_extrapol) && (stats0[j].y > tracks[i].y - r_extrapol)){
                            idisp(stats1[j].ID);
                            tracks[i].end = stats0[j];
                            tracks[i].state = TRACK_UPDATED;
                            update_bounding_box(tracks+i, stats0[j], frame-1);
                        }
                }
            }


            if(tracks[i].state == TRACK_LOST){
                PUTS("TRACK_LOST");
                for(int j = 1; j <= nc1; j++){
                    if (!stats1[j].prev){
                        if ( (stats1[j].x > tracks[i].x - r_extrapol) && (stats1[j].x < tracks[i].x + r_extrapol) && (stats1[j].y < tracks[i].y + r_extrapol) && (stats1[j].y > tracks[i].y - r_extrapol)){
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
                    float32 a;
                    float32 dx = (stats1[next].x - stats0[tracks[i].end.ID].x);
                    float32 dy = (stats1[next].y - stats0[tracks[i].end.ID].y) ;

                    a = (dx == 0) ? INF : (dy/dx);

                    float32 y = tracks[i].a * stats1[next].x + tracks[i].b;

                    if ( (fabs(stats1[next].y - y) < d_line) && ((dx*tracks[i].dx >= 0.0f) && (dy*tracks[i].dy >= 0.0f)) && ((a < 0 && tracks[i].a < 0) ||  (a > 0 && tracks[i].a > 0) || ((a == INF) && (tracks[i].a == INF)))){
                        tracks[i].is_meteor = 2;
                        tracks[i].a = a;
                        tracks[i].dx = dx;
                        tracks[i].dy = dy;
                        tracks[i].b = y - a * stats1[next].x;
                    }
                    else tracks[i].is_meteor = 1;
                    // tracks[i].vitesse[(tracks[i].cur)++] = stats0[tracks[i].end.ID].error;
                    tracks[i].x = tracks[i].end.x;
                    tracks[i].y = tracks[i].end.y;
                    tracks[i].end = stats1[next];
                    tracks[i].time++;
                    update_bounding_box(tracks+i, stats1[next], frame+1);
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

// -----------------------------------------------------
void updateTrackStars(Track *tracks, MeteorROI *stats0, MeteorROI *stats1, int nc1, int frame, int *offset, int *last)
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
        if(tracks[i].time  && tracks[i].state != TRACK_FINISHED ){
                next = stats0[tracks[i].end.ID].next;
                if(next){
                    tracks[i].x = tracks[i].end.x;
                    tracks[i].y = tracks[i].end.y;
                    tracks[i].end = stats1[next];
                    tracks[i].time++;
                    update_bounding_box(tracks+i, stats1[next], frame);
                } 
                else{
                    //on extrapole si pas finished
                    tracks[i].state = TRACK_FINISHED;
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

    track->begin     =  buffer[i].stats0;
    
    track->bb_x      = (uint16)ceil((double)((buffer[i].stats0.xmin + buffer[i].stats0.xmax))/2);
    track->bb_y      = (uint16)ceil((double)((buffer[i].stats0.ymin + buffer[i].stats0.ymax))/2);

    update_bounding_box(track, buffer[i].stats0, frame-1);
    // saveBoundingBox(path_bounding_box, track->rx, track->ry, track->bb_x, track->bb_y, frame-2);
    update_bounding_box(track, buffer[i].stats1, frame);
    // saveBoundingBox(path_bounding_box, track->rx, track->ry, track->bb_x, track->bb_y, frame-1);


    float32 dx = (buffer[i].stats1.x - buffer[i].stats0.x);
    float32 dy = (buffer[i].stats1.y - buffer[i].stats0.y);

    track->a = (dx==0) ? INF : (dy/dx);
    track->b = buffer[i].stats1.y - track->a * buffer[i].stats1.x;
    track->dx = dx;
    track->dy = dy;
    
    track->end       = last_stats; 
    track->time      = 1; 
    track->timestamp = frame - 2;
    track->state = TRACK_NEW;

    // track->vitesse[(track->cur)++] = buffer[i].stats0.error;
    // update_bounding_box(track, last_stats, frame);
}

// -----------------------------------------------------
void insert_new_track_stars(MeteorROI last_stats,  MeteorROI begin, Track *tracks, int *last, int frame)
// -----------------------------------------------------
{
    Track *track = &tracks[++(*last)];

    track->begin     =  begin;
    
    track->bb_x      = (uint16)ceil((double)((begin.xmin + begin.xmax))/2);
    track->bb_y      = (uint16)ceil((double)((begin.ymin + begin.ymax))/2);

    track->end       = last_stats; 
    track->time      = 1; 
    track->timestamp = frame - 2;
    track->state = TRACK_NEW;
}


// -----------------------------------------------------
void Tracking(MeteorROI *stats0, MeteorROI *stats1, Track *tracks, int nc0, int nc1, int frame, int *last, int *offset, int theta, int tx, int ty, int r_extrapol, int d_line, int diff_deviation)
// -----------------------------------------------------
{
    int j;

    double errMoy = errorMoy(stats0, nc0);
    double eType = ecartType(stats0, nc0, errMoy);


    saveErrorMoy("second_error.txt", errMoy, eType);

    for(int i = 1; i <= nc0; i++){
        float32 e = stats0[i].error;
        
        int asso = stats0[i].next;
        

        // si mouvement detecté
        if (fabs(e-errMoy) > diff_deviation * eType && asso){
            
            if (stats0[i].state) {
                PUTS("EXTRAPOLATEED");
                continue; // Extrapolated
            }
            idisp(i);
            idisp(stats0[i].ID);
            stats0[i].motion = 1; // debug
            stats0[i].time++;
            stats1[stats0[i].next].time = stats0[i].time ;
            idisp(stats0[i].time);
            if(stats0[i].time == 1){
                // stocker dans un buf pour savoir si au moins sur 3 frames
                insert_buffer(stats0[i], stats1[stats0[i].next], frame);

            }
            if(stats0[i].time == 2){ 
                // mouvement sur 3 frames donc création de track + suppression du buff    
                for(j = *offset; j <= *last; j++){
                    if(tracks[j].end.ID == stats0[i].ID && tracks[j].end.x == stats0[i].x ){
                        PUTS("BREAK");
                        break;
                    }
                }   
                int k = search_buf_stat(stats0[i], frame);
                idisp(k);
                if(j == *last + 1 || *last == -1){
                    // insertion seulement si dans le buffer (k != -1)
                    idisp(stats0[i].next);
                    insert_new_track(stats0[i], tracks, last, frame, k);
                    // printTracks(tracks, *last);
                    idisp(stats0[i].ID);
                }
                clear_index_buffer(k);
                
            }
        } 
    }

    // parcourir les track et update si besoin
    updateTrack(tracks, stats0, stats1, nc1, frame, offset, last, theta, tx, ty, r_extrapol, d_line);

    // clear/update le buffer
    update_buffer(frame);

    // printBuffer(buffer, 10);
    // printTracks(tracks, *last);
    // printStats(stats0, nc0);
    // printStats(stats1,nc1);
}

void TrackStars(MeteorROI *stats0, MeteorROI *stats1, Track *tracks, int nc0, int nc1, int frame, int *last, int *offset)
{

    for(int i = 1; i <= nc0; i++){
        int asso = stats0[i].next;

        if (asso){
            
            stats0[i].time++;
            stats1[stats0[i].next].time = stats0[i].time ;
           
            if(stats0[i].time == 1)
                insert_new_track_stars(stats0[i], stats1[stats0[i].next], tracks, last, frame);
        } 
    }

    // parcourir les track et update si besoin
    updateTrackStars(tracks, stats0, stats1, nc1, frame, offset, last);

}
