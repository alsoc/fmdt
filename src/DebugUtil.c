/**
 * Copyright (c) 2017-2018, Arthur Hennequin, LIP6, UPMC, CNRS
 * Copyright (c) 2020-2020, Lionel Lacassagne, LIP6 Sorbonne University, CNRS
 * Copyright (c) 2020-2021, MILLET Maxime, LIP6 Sorbonne University
 * Copyright (c) 2020-2021, Clara CIOCAN, LIP6 Sorbonne University
 * Copyright (c) 2020-2021, Mathuran KANDEEPAN, LIP6 Sorbonne University
 */
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

#include <nrc2.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdio.h>
#include <sys/stat.h>


#define SIZE_BUF 20

char path_stats_0[250], path_stats_1[250];
char path_frames_binary[250], path_frames_output[250];
char path_video_tracking[250];
char path_motion[200], path_extraction[200], path_error[200], path_tracks[200], path_bounding_box[200];
char path_debug[250];
char path_frames_binary_dir[200], path_frames_output_dir[200], path_stats_f[200], path_video_f[200];
char path_assoconflicts_f[150];

extern uint32 **nearest;
extern float32 **distances;
extern uint32 *conflicts;




/******************
 * Data gathering *
 *****************/

// ---------------------------------------------------------------------------------------------------
void printStats(MeteorROI* stats, int n)
// ---------------------------------------------------------------------------------------------------
{
    // for(int i = 1; i<= n; i++){
        // stats[i].S = 0;
    // }
    // tmp (le temps de mettre à jour n)
    int cpt = 0;
    for(int i = 1; i<= n; i++){
        // idisp(stats[i].S);
        if(stats[i].S > 0){
            // idisp(stats[i].S);
            cpt++;
        }
    }
    printf("Nombre de CC : %d\n", cpt);

    if (cpt==0) return;

    for(int i = 1; i<= n; i++){
        if(stats[i].S > 0)
            printf("%4d \t %4d \t %4d \t %4d \t %4d \t %3d \t %4d \t %4d \t %7.1f \t %7.1f \t %4d \t %4d \t %4d \t %7.1lf \t %d\n", 
            stats[i].ID, stats[i].xmin, stats[i].xmax, stats[i].ymin, stats[i].ymax, stats[i].S, stats[i].Sx, stats[i].Sy, stats[i].x, stats[i].y, stats[i].prev, stats[i].next, stats[i].time, stats[i].error, stats[i].motion);
    }
    printf("\n");
}


// ---------------------------------------------------------------------------------------------------
void printBuffer(Buf *buffer, int n)
// ---------------------------------------------------------------------------------------------------
{
    for(int i = 0; i<n; i++){
        if(buffer[i].stats0.ID > 0)
            printf("%4d \t %4d \t %4d  \n", 
            buffer[i].stats0.ID, buffer[i].stats0.time, buffer[i].frame);
    }
    printf("\n");
}


void printTabBB(elemBB **tabBB, int n)
{
    for (int i = 0; i < n; i++){
        if(tabBB[i]!= NULL){
            for( elemBB *current =tabBB[i]; current != NULL; current = current->next ){
                printf( "%d %d %d %d %d \n", i, current->rx, current->ry, current->bb_x, current->bb_y );
            }
        }
    }
}


void saveTabBB(const char *filename, elemBB **tabBB, int n)
{

    FILE *f = fopen(filename, "w");
    if (f == NULL){
        printf("error ouverture %s \n", filename);
        exit(1);
    }

    for (int i = 0; i < n; i++){
        if(tabBB[i]!= NULL){
            for( elemBB *current =tabBB[i]; current != NULL; current = current->next ){
                fprintf(f, "%d %d %d %d %d \n", i, current->rx, current->ry, current->bb_x, current->bb_y );
            }
        }
    }

    fclose(f);
}
// ---------------------------------------------------------------------------------------------------
void saveErrorMoy(const char *filename, double errMoy, double eType)
// ---------------------------------------------------------------------------------------------------
{
    char path[200];
    sprintf(path, "%s/%s", path_assoconflicts_f, filename);
    disp(path);
    FILE *f = fopen(path, "a");
    if (f == NULL){
        printf("error ouverture %s \n", path);
        exit(1);
    }
    fprintf(f, "%5.2f \t %5.2f \n", errMoy, eType); 

    fclose(f);
}
// ---------------------------------------------------------------------------------------------------
void printTracks(Track* tracks, int last)
// ---------------------------------------------------------------------------------------------------
{
    puts("Hello");
    printf("%d\n", last);

    if (last==-1) return;

    for(int i = 0; i< last; i++){
                printf("%4d \t %6.1f \t %6.1f \t %4d \t %6.1f \t %6.1f \t %4d \t %4d \t %4d \t %4d \t %4d %4d\n", 
        tracks[i].timestamp, tracks[i].begin.x, tracks[i].begin.y, tracks[i].timestamp+tracks[i].time , tracks[i].end.x , tracks[i].end.y, tracks[i].rx, tracks[i].ry, tracks[i].bb_x, tracks[i].bb_y, tracks[i].is_valid, tracks[i].is_meteor);

        // printf("%4d \t %5f \t %5f \t %4d \t %5f \t %5f \t d\n", 
        // tracks[i].timestamp, tracks[i].begin.x , tracks[i].begin.y, tracks[i].timestamp+tracks[i].time - 1, tracks[i].end.x , tracks[i].end.y);
    }
    printf("\n");
}


// ---------------------------------------------------------------------------------------------------
void parseStats(const char*filename, MeteorROI* stats, int* n)
// ---------------------------------------------------------------------------------------------------
{
    char lines[200];
    int id, xmin, xmax, ymin, ymax, s, sx, sy, prev, next;
    double x, y;
    float32 dx, dy, error;
    FILE * file = fopen(filename, "r"); 
    // printf("%s\n", filename);
    if (file == NULL) { 
        fprintf(stderr, "cannot open file\n");
        return;
    }
    
    // pour l'instant, n représente l'id max des stas mais c'est à changer 
    fgets(lines, 100, file);
    sscanf(lines, "%d", n);

    while (fgets(lines, 200, file)){
        sscanf(lines, "%d %d %d %d %d %d %d %d %lf %lf %f %f %f %d %d", &id, &xmin, &xmax, &ymin, &ymax, &s, &sx, &sy, &x, &y, &dx, &dy, &error, &prev, &next);
        stats[id].ID   = id;
        stats[id].xmin = xmin;
        stats[id].xmax = xmax;
        stats[id].ymin = ymin;
        stats[id].ymax = ymax;
        stats[id].S    = s;
        stats[id].Sx   = sx;
        stats[id].Sy   = sy;
        stats[id].x    = x;
        stats[id].y    = y;
        stats[id].dx   = dx;
        stats[id].dy   = dy;
        stats[id].error= error;
        stats[id].prev = prev;
        stats[id].next = next;
        // (*n) = id;
    }
    fclose(file);
}

// ---------------------------------------------------------------------------------------------------
void saveStats_file(FILE *f, MeteorROI* stats, int n)
// ---------------------------------------------------------------------------------------------------
{
    // tmp (le temps de mettre à jour n)
    int cpt = 0;
    for(int i = 1; i<= n; i++){
        // idisp(i);
        if(stats[i].S != 0)
            cpt++;
    }
    fprintf(f, "%d\n", cpt);
    for(int i = 1; i<= n; i++){
        if(stats[i].S != 0)
            fprintf(f, "%4d \t %4d \t %4d \t %4d \t %4d \t %3d \t %8d \t %8d \t %8.1f \t %8.1f \t %8.1f \t %8.1f \t %8.1f \t %4d \t %4d\n", 
            stats[i].ID, stats[i].xmin, stats[i].xmax, stats[i].ymin, stats[i].ymax, stats[i].S, stats[i].Sx, stats[i].Sy, stats[i].x, stats[i].y, stats[i].dx, stats[i].dy, stats[i].error, stats[i].prev, stats[i].next);
    }
}

// ---------------------------------------------------------------------------------------------------
void saveStats(const char*filename, MeteorROI* stats, int n)
// ---------------------------------------------------------------------------------------------------
{
    FILE *f = fopen(filename, "w");
    if (f == NULL){
        printf("error ouverture %s \n", filename);
        exit(1);
    }

    saveStats_file(f, stats, n);

    fclose(f);

}

// ---------------------------------------------------------------------------------------------------
void saveTracks(const char*filename, Track* tracks, int n)
// ---------------------------------------------------------------------------------------------------
{
    FILE *f = fopen(filename, "w");
    if (f == NULL){
        printf("error ouverture %s \n", filename);
        exit(1);
    }

    int cpt = 0;
    for(int i = 0; i <= n; i++){
        if(tracks[i].time)
            cpt++;
    }

    fprintf(f, "%d\n", cpt);

    if (cpt != 0){
        for(int i = 0; i<= n; i++){
            if(tracks[i].time){
                fprintf(f, "%4d \t %6.1f \t %6.1f \t %4d \t %6.1f \t %6.1f \t %4d \t %4d \t %4d\n", 
                tracks[i].timestamp, tracks[i].begin.x , tracks[i].begin.y, tracks[i].timestamp+tracks[i].time , tracks[i].end.x, tracks[i].end.y, tracks[i].bb_x, tracks[i].bb_y, tracks[i].is_meteor);
            }
    
        }
    }
    fclose(f);
}

// ---------------------------------------------------------------------------------------------------
void saveBoundingBox(const char*filename, uint16 rx, uint16 ry, uint16 bb_x, uint16 bb_y, int frame)
// ---------------------------------------------------------------------------------------------------
{
    FILE *f = fopen(filename, "a");
    if (f == NULL){
        printf("error ouverture %s \n", filename);
        exit(1);
    }

    fprintf(f, "%4d \t %4d \t %4d \t %4d \t %4d \n", frame, rx, ry, bb_x, bb_y); 
    
    fclose(f);
}



// ---------------------------------------------------------------------------------------------------
void parseTracks(const char*filename, Track* tracks, int* n)
// ---------------------------------------------------------------------------------------------------
{
    char lines[100];
    int t0, t1;
    float32 x0, x1, y0, y1;
    int bb_x, bb_y;
    int is_meteor;
    FILE * file = fopen(filename, "r"); 
    if (file == NULL) {
        fprintf(stderr, "cannot open file\n");
        return;
    }
    
    fgets(lines, 100, file);
    sscanf(lines, "%d", n);
    // while (fgets(lines, 100, file)){
    // printf("%d\n", *n);
    for(int i = 0; i< *n; i++){
        fgets(lines, 100, file);
        sscanf(lines, "%d %f %f %d %f %f %d %d %d", &t0, &x0, &y0, &t1, &x1, &y1, &bb_x, &bb_y, &is_meteor);
        tracks[i].timestamp = t0;
        tracks[i].time      = t1-t0+1;
        tracks[i].state     = 0;
        tracks[i].begin.x   = x0;
        tracks[i].begin.y   = y0;
        tracks[i].end.x     = x1;
        tracks[i].end.y     = y1;
        tracks[i].bb_x      = bb_x;
        tracks[i].bb_y      = bb_y;
        tracks[i].is_meteor = is_meteor;
    }
    puts("end");
    idisp(*n);
    // (*n)--;
    fclose(file);
    idisp(*n);
}

// ---------------------------------------------------------------------------------------------------
void saveMotion(const char*filename, double theta, double tx, double ty, int frame)
// ---------------------------------------------------------------------------------------------------
{
    FILE *f = fopen(filename, "a");
    if (f == NULL){
        printf("error ouverture %s \n", filename);
        exit(1);
    }

    fprintf(f,"%d - %d\n", frame, frame+1);
    fprintf(f, "%6.7f \t %6.4f \t %6.4f \n", theta, tx, ty);
    fprintf(f,"---------------------------------------------------------------\n"); 
    fclose(f);

}

// ---------------------------------------------------------------------------------------------------
void saveError(const char *filename, MeteorROI *stats, int n)
// ---------------------------------------------------------------------------------------------------
{
    double S = 0;
    int cpt = 0;
    FILE *f = fopen(filename, "a");
    if (f == NULL){
        printf("error ouverture %s \n", filename);
        exit(1);
    }

    for(int i=1; i<=n; i++){
        if(stats[i].S > 0){
            // e = sqrt(stats[i].dx * stats[i].dx + stats[i].dy * stats[i].dy);
            // fprintf(f,"%.2f\n", e);
            S+= stats[i].error;
            cpt++;
        }
    }
    
    fprintf(f,"%.2f\n", S/cpt);
    fclose(f);


}


// // ---------------------------------------------------------------------------------------------------
// void saveAsso(const char*filename, uint32 *asso, uint32 **Nearest, float32 **distances, int nc0, MeteorROI *stats)
// // ---------------------------------------------------------------------------------------------------
// {
//     FILE *f = fopen(filename, "w");
//     if (f == NULL){
//         printf("error ouverture %s \n", filename);
//         exit(1);
//     }
    
//     // tmp (le temps de mettre à jour n)
//     int cpt = 0;
//     for(int i = 1; i<= nc0; i++){
//         if( asso[i] != 0)
//             cpt++;
//     }
//     if (cpt != 0) {

//         fprintf(f, "%d\n", cpt);
        
//         int j; 
//         for(int i = 1; i<= nc0; i++){
//             j = asso[i]; 
//             if(j == 0 ){
//                 if (stats[i].S > 0)
//                     fprintf(f, "%4d \t ->   pas d'association\n", i);
//             }
//             else {
//                 fprintf(f, "%4d \t -> %4d \t  : distance = %10.2f \t ; %4d-voisin\n", i, j, distances[i][j], Nearest[i][j]);

//             }
//         }
//     }
//     fclose(f);
// }


// ---------------------------------------------------------------------------------------------------
void saveAsso(const char*filename, uint32 **Nearest, float32 **distances, int nc0, MeteorROI *stats)
// ---------------------------------------------------------------------------------------------------
{
    FILE *f = fopen(filename, "w");
    if (f == NULL){
        printf("error ouverture %s \n", filename);
        exit(1);
    }
    
    // tmp (le temps de mettre à jour n)
    int cpt = 0;
    for(int i = 1; i<= nc0; i++){
        if( stats[i].S != 0 && stats[i].next )
            cpt++;
    }
    if (cpt != 0) {

        fprintf(f, "%d\n", cpt);
        
        int j; 
        for(int i = 1; i<= nc0; i++){
            j = stats[i].next; 
            if(j == 0 ){
                if (stats[i].S > 0)
                    fprintf(f, "%4d \t ->   pas d'association\n", i);
            }
            else {
                fprintf(f, "%4d \t -> %4d \t  : distance = %10.2f \t ; %4d-voisin\n", i, j, distances[i][j], Nearest[i][j]);

            }
        }
    }
    fclose(f);
}

// ---------------------------------------------------------------------------------------------------
void saveAsso_VT(const char*filename, int nc0, MeteorROI *stats, int frame)
// ---------------------------------------------------------------------------------------------------
{
    FILE *f = fopen(filename, "a");
    if (f == NULL){
        printf("error ouverture %s \n", filename);
        exit(1);
    }
    fprintf(f, "%05d_%05d\n", frame, frame+1);
    
    int j; 
    for(int i = 1; i<= nc0; i++){
        j = stats[i].next; 
        if(j == 0 ){
            if (stats[i].S > 0)
                fprintf(f, "%4d \t ->   pas d'association\n", i);
        }
        else {
                fprintf(f, "%4d \t -> %4d \n", i, j );

        }
    }
    fprintf(f, "------------------------------------------------------------------------------------------------------------\n");
    fclose(f);
}

// ---------------------------------------------------------------------------------------------------
void saveConflicts(const char*filename, uint32 *conflicts, uint32 **Nearest, float32 **distances, int n_asso, int n_conflict)
// ---------------------------------------------------------------------------------------------------
{
    FILE *f = fopen(filename, "w");
    if (f == NULL){
        printf("error ouverture %s \n", filename);
        exit(1);
    }

    // tmp (le temps de mettre à jour n)
    int cpt = 0;
    for(int i = 1; i<= n_conflict; i++){
        if(conflicts[i] != 1 && conflicts[i] != 0)
            cpt++;
    }
    if (cpt != 0){

        fprintf(f, "%d\n", cpt);

        for(int j = 1; j <= n_conflict; j++){
            if (conflicts[j] != 1 && conflicts[j] != 0){
                fprintf(f, "conflit CC = %4d : ", j);
                for(int i = 1 ; i <= n_asso; i++){
                    if (Nearest[i][j] == 1 ){
                        fprintf(f, "%4d\t", i);
                    }
                }
                fprintf(f, "\n");
            }
        }
    }
    fclose(f);
}


// ---------------------------------------------------------------------------------------------------
void saveAssoConflicts(const char*filename, int frame, uint32 *conflicts, uint32 **Nearest, float32 **distances, int n_asso, int n_conflict, MeteorROI *stats0, MeteorROI *stats1)
// ---------------------------------------------------------------------------------------------------
{
    FILE *f = fopen(filename, "a");
    if (f == NULL){
        printf("error ouverture %s \n", filename);
        exit(1);
    }

    // frames
    fprintf(f, "%05d_%05d\n", frame, frame+1);
    
    
    // stats
    fprintf(f, "%05d.txt\n", frame);
    saveStats_file(f, stats0, n_asso);
    fprintf(f, "%05d.txt\n", frame+1);
    saveStats_file(f, stats1, n_conflict);

    // Asso
    int cpt = 0;
    for(int i = 1; i<= n_asso; i++){
        if( stats0[i].next != 0)
            cpt++;
    }
    fprintf(f, "Assos\n%d\n", cpt);
    int j; 

    double errMoy = errorMoy(stats0, n_asso);
    double eType = ecartType(stats0, n_asso, errMoy);
    fprintf(f, "error moy : %.3f \t ecart type : %.3f\n", errMoy, eType);

    for(int i = 1; i<= n_asso; i++){
        if (stats0[i].S == 0) continue;
        
        j = stats0[i].next; 
        if(j == 0 ){
                fprintf(f, "%4d \t ->   pas d'association\n", i);
        }
        else {
            float32 dx = stats0[i].dx;
            float32 dy = stats0[i].dy;
            fprintf(f, "%4d \t -> %4d \t  : distance = %10.2f \t ; %4d-voisin \t dx = %4.1f \t dy = %4.1f \t error: %.3f\n", i, j, distances[i][j], Nearest[i][j], dx , dy , stats0[i].error);
        }
    }

    // // Conflicts
    // cpt = 0;
    // for(int i = 1; i<= n_conflict; i++){
    //     if(conflicts[i] != 1 && conflicts[i] != 0)
    //         cpt++;
    // }

    // fprintf(f, "Conflicts\n%d\n", cpt);

    // for(int j = 1; j <= n_conflict; j++){
    //     if (conflicts[j] != 1 && conflicts[j] != 0){
    //         fprintf(f, "conflit CC = %4d : ", j);
    //         for(int i = 1 ; i <= n_asso; i++){
    //             if (Nearest[i][j] == 1 ){
    //                 fprintf(f, "%4d\t", i);
    //             }
    //         }
    //         fprintf(f, "\n");
    //     }
    // }
    fprintf(f, "\n------------------------------------------------------------------------------------------------------------\n");
    fclose(f);
}

// -----------------------------------------------------
void saveMotionExtraction(char *filename, MeteorROI *stats0, MeteorROI *stats1, int nc0, double theta, double tx, double ty, int frame)
// -----------------------------------------------------
{
    // Version DEBUG : il faut implémenter une version pour le main
    FILE * f = fopen(filename, "a"); 
    if (f == NULL) {
        fprintf(stderr, "motion : cannot open file\n");
        return;
    }

    double errMoy = errorMoy(stats0, nc0);
    double eType = ecartType(stats0, nc0, errMoy);


    for(int i=1; i<=nc0; i++){
        float32 e = stats0[i].error;
        // si mouvement detecté
        if (fabs(e-errMoy) >  1.5 * eType){
                fprintf(f,"%d - %d\n", frame, frame+1);
                fprintf(f,"CC en mouvement: %2d \t dx:%.3f \t dy: %.3f \t xmin: %3d \t xmax: %3d \t ymin: %3d \t ymax: %3d\n",stats0[i].ID, stats0[i].dx, stats0[i].dy, stats0[i].xmin, stats0[i].xmax, stats0[i].ymin, stats0[i].ymax);
                fprintf(f,"---------------------------------------------------------------\n"); 
            
        }
    }
    fclose(f);
}

/*************
 * Analysing *
 *************/

// ---------------------------------------------------------------------------------------------------
void filter_speed_binarize(uint32** in, int i0, int i1, int j0, int j1, uint8** out, MeteorROI* stats)
// ---------------------------------------------------------------------------------------------------
{
    for (int i=i0 ; i<=i1 ; i++) {
        for (int j=j0 ; j<=j1 ; j++) {
            if (stats[in[i][j]].S != 0) {
                out[i][j] = 0xFF;
            } else {
                out[i][j] = 0;
            }
        }
    }
}

/********
 * Draw *
 ********/
 
void plot_bouding_box(rgb8** img, int ymin, int ymax, int xmin, int xmax, int border, rgb8 color)
{
    for (int b=0; b<border ; b++) {
        ymin++; ymax--;xmin++;xmax--;
        for (int i=ymin ; i<=ymax ; i++) {
            img[i][xmin] = color;
            img[i][xmax] = color;
        }
        for (int j=xmin ; j<=xmax ; j++) {
            img[ymin][j] = color;
            img[ymax][j] = color;
        }
    }
}

/*********
 * Image *
 *********/

// ----------------------------------------------------------------------------------
rgb8** load_image_color(const char* filename, long* i0, long* i1, long* j0, long* j1)
// ----------------------------------------------------------------------------------
{
    VERBOSE(printf("%s\n", filename););
    ffmpeg_handle reader;
    ffmpeg_init(&reader);
    ffmpeg_options options;
    ffmpeg_options_init(&options);
    
    options.debug = 1;
    
    if(!ffmpeg_probe(&reader, filename, &options)) {
        fprintf(stderr, "Error in load_image_color: %s (%d)\n", ffmpeg_error2str(reader.error), reader.error);
        exit(0);
    }
    reader.output.pixfmt = ffmpeg_str2pixfmt("rgb24");
    *i0 = 0;
    *j0 = 0;
    *i1 = reader.input.height - 1;
    *j1 = reader.input.width - 1;
    VERBOSE(printf("i1=%ld j1=%ld\n", *i1,*j1););
    rgb8** img = rgb8matrix(*i0, *i1, *j0, *j1);

    ffmpeg_start_reader(&reader, filename, NULL);
    ffmpeg_read2d(&reader, (uint8_t**)img);
    ffmpeg_stop_reader(&reader);
    return img;
}

/*********
 * Video *
 *********/

// ==========================================================================================================================================================================
void saveFrame_threshold(const char*filename, uint8**I0, uint8**I1, int i0, int i1, int j0, int j1)
// ==========================================================================================================================================================================
{
    int w = (j1-j0+1);
    int h = (i1-i0+1);


    char buffer[80];

    FILE *file;

    rgb8** img = rgb8matrix(0, h-1, 0, 2*w-1);
    if (img == NULL) return;

    // (0,0) : video
    for (int i=i0 ; i<=i1 ; i++) {
        for (int j=j0 ; j<=j1 ; j++) {
            img[i][j].r = I0[i][j];
            img[i][j].g = I0[i][j];
            img[i][j].b = I0[i][j];
        }
    }
    // (0,1) : seuillage luminosité
    for (int i=i0 ; i<=i1 ; i++) {
        for (int j=j0 ; j<=j1 ; j++) {
            img[i][j+w].r = I1[i][j];
            img[i][j+w].g = I1[i][j];
            img[i][j+w].b = I1[i][j];
        }
    }


    file = fopen(filename, "wb");
    if (file == NULL)
      nrerror("ouverture du fichier %s impossible dans saveVideoFrame_quad\n");

    /* enregistrement de l'image au format rpgm */

    sprintf(buffer,"P6\n%d %d\n255\n",(int)(2*w-1), (int)(h-1));
    fwrite(buffer,strlen(buffer),1,file);
    for(int i=0; i<=h-1; i++)
      WritePNMrow((uint8*)img[i], 2*w-1, file);


    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix(img, 0, h-1, 0, 2*w-1);
}

// ==========================================================================================================================================================================
void saveFrame_ui32matrix(const char*filename, uint32**I, int i0, int i1, int j0, int j1)
// ==========================================================================================================================================================================
{
    int w = (j1-j0+1);
    int h = (i1-i0+1);


    char buffer[80];

    FILE *file;

    rgb8** img = rgb8matrix(0, h-1, 0, w-1);
    if (img == NULL) return;

    // (1,1) : filtrage surface
    for (int i=i0 ; i<=i1 ; i++) {
        for (int j=j0 ; j<=j1 ; j++) {
            img[i][j].r = (I[i][j] == 0)? 0 : 255;
            img[i][j].g = (I[i][j] == 0)? 0 : 255;
            img[i][j].b = (I[i][j] == 0)? 0 : 255;
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL)
      nrerror("ouverture du fichier %s impossible dans saveVideoFrame_quad\n");

    /* enregistrement de l'image au format rpgm */

    sprintf(buffer,"P6\n%d %d\n255\n",(int)(w-1), (int)(h-1));
    fwrite(buffer,strlen(buffer),1,file);
    for(int i=0; i<=h-1; i++)
      WritePNMrow((uint8*)img[i], w-1, file);


    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix(img, 0, h-1, 0, w-1);
}

// ==========================================================================================================================================================================
void saveFrame_tracking(const char*filename, uint8**I, Track* tracks, int tracks_nb, int i0, int i1, int j0, int j1)
// ==========================================================================================================================================================================
{
    rgb8 green;     rgb8 red;       rgb8 blue;      rgb8 orange;
    green.g = 255;  red.g = 000;    blue.g = 000;   orange.r = 255;
    green.b = 000;  red.b = 000;    blue.b = 255;   orange.g = 165;
    green.r = 000;  red.r = 255;    blue.r = 000;   orange.b = 000;

    int w = (j1-j0+1);
    int h = (i1-i0+1);


    char buffer[80];

    FILE *file;

    rgb8** img = rgb8matrix(0, h-1, 0, w-1);
    if (img == NULL) return;

    // (1,1) : filtrage surface
    for (int i=i0 ; i<=i1 ; i++) {
        for (int j=j0 ; j<=j1 ; j++) {
            img[i][j].r = I[i][j];
            img[i][j].g = I[i][j];
            img[i][j].b = I[i][j];
        }
    }

    for (int i=0 ; i<tracks_nb+1 ; i++) {
        idisp(tracks[i].time);
        idisp(tracks[i].state);
        if (tracks[i].state != TRACK_LOST && tracks[i].time >= 2 && tracks[i].state != TRACK_EXTRAPOLATED && tracks[i].state != TRACK_FINISHED) {
            int ymin = clamp(tracks[i].bb_y - tracks[i].ry, 1, i1-1);
            int ymax = clamp(tracks[i].bb_y + tracks[i].ry, 1, i1-1);
            int xmin = clamp(tracks[i].bb_x - tracks[i].rx, 1, j1-1);
            int xmax = clamp(tracks[i].bb_x + tracks[i].rx, 1, j1-1);
            idisp(xmin);
            idisp(xmax);
            idisp(ymin);
            idisp(ymax);
            if(tracks[i].time >= 2) {
                // if(tracks[i].is_valid) {
                    
                    // if(tracks[i].state != TRACK_EXTRAPOLATED) 
                    plot_bouding_box(img, ymin,ymax,xmin,xmax, 1, green);
                    // else plot_bouding_box(img, ymin,ymax,xmin,xmax, 2, blue);
            }
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL)
      nrerror("ouverture du fichier %s impossible dans saveVideoFrame_quad\n");

    /* enregistrement de l'image au format rpgm */

    sprintf(buffer,"P6\n%d %d\n255\n",(int)(w-1), (int)(h-1));
    fwrite(buffer,strlen(buffer),1,file);
    for(int i=0; i<=h-1; i++)
      WritePNMrow((uint8*)img[i], w-1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix(img, 0, h-1, 0, w-1);
}


// ==============================================================================================================================
void saveVideoFrame_tracking(const char*filename, uint8** I, Track* tracks, int tracks_nb, int i0, int i1, int j0, int j1)
// ==============================================================================================================================
{
    rgb8 green;     rgb8 red;       rgb8 blue;      rgb8 orange;
    green.g = 255;  red.g = 000;    blue.g = 000;   orange.r = 255;
    green.b = 000;  red.b = 000;    blue.b = 255;   orange.g = 165;
    green.r = 000;  red.r = 255;    blue.r = 000;   orange.b = 000;

    static ffmpeg_handle writer;
    if (writer.pipe == NULL) {
      ffmpeg_init(&writer);
      writer.input.width = j1 - j0 + 1;
      writer.input.height = i1 - i0 + 1;
      writer.input.pixfmt = ffmpeg_str2pixfmt("rgb24");
      if (!ffmpeg_start_writer(&writer, filename, NULL)) return;
    }
    rgb8** img = rgb8matrix(0, i1, 0, j1);
    for (int i=i0 ; i<=i1 ; i++) {
        for (int j=j0 ; j<=j1 ; j++) {
            img[i][j].r = I[i][j];
            img[i][j].g = I[i][j];
            img[i][j].b = I[i][j];
        }
    }

    for (int i=0 ; i<tracks_nb+1 ; i++) {
        idisp(tracks[i].time);
        idisp(tracks[i].state);
        if (tracks[i].state != TRACK_LOST && tracks[i].time >= 3 && tracks[i].state != TRACK_EXTRAPOLATED && tracks[i].state != TRACK_FINISHED) {
            int ymin = clamp(tracks[i].bb_y - tracks[i].ry - 5, 1, i1-1);
            int ymax = clamp(tracks[i].bb_y + tracks[i].ry + 5, 1, i1-1);
            int xmin = clamp(tracks[i].bb_x - tracks[i].rx - 5, 1, j1-1);
            int xmax = clamp(tracks[i].bb_x + tracks[i].rx + 5, 1, j1-1);
            idisp(xmin);
            idisp(xmax);
            idisp(ymin);
            idisp(ymax);
            if(tracks[i].time >= 3) {
                // if(tracks[i].is_valid) {
                    
                    // if(tracks[i].state != TRACK_EXTRAPOLATED) 
                    plot_bouding_box(img, ymin,ymax,xmin,xmax, 2, green);
                    // else plot_bouding_box(img, ymin,ymax,xmin,xmax, 2, blue);
            }
        }
    }
    ffmpeg_write2d(&writer, (uint8_t**)img);
    free_rgb8matrix(img, 0, i1, 0, j1);
}



// ==========================================================================================================================================================================
void saveFrame_ui8matrix(const char*filename, uint8**I, int i0, int i1, int j0, int j1)
// ==========================================================================================================================================================================
{
    int w = (j1-j0+1);
    int h = (i1-i0+1);


    char buffer[80];

    FILE *file;

    rgb8** img = rgb8matrix(0, h-1, 0, w-1);
    if (img == NULL) return;

    for (int i=i0 ; i<=i1 ; i++) {
        for (int j=j0 ; j<=j1 ; j++) {
            img[i][j].r = I[i][j];
            img[i][j].g = I[i][j];
            img[i][j].b = I[i][j];
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL)
      nrerror("ouverture du fichier %s impossible dans saveVideoFrame_quad\n");

    /* enregistrement de l'image au format rpgm */

    sprintf(buffer,"P6\n%d %d\n255\n",(int)(w-1), (int)(h-1));
    fwrite(buffer,strlen(buffer),1,file);
    for(int i=0; i<=h-1; i++)
      WritePNMrow((uint8*)img[i], w-1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix(img, 0, h-1, 0, w-1);
}

// --------------------------------------------------
void HsvToRgb(rgb8* pixel, uint8 h, uint8 s, uint8 v)
// --------------------------------------------------
{
    unsigned char region, remainder, p, q, t;

    if (s == 0) {
        pixel->r = v;
        pixel->g = v;
        pixel->b = v;
        return;
    }

    region = h / 43;
    remainder = (h - (region * 43)) * 6;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
        case 0:
            pixel->r = v; pixel->g = t; pixel->b = p;
            break;
        case 1:
            pixel->r = q; pixel->g = v; pixel->b = p;
            break;
        case 2:
            pixel->r = p; pixel->g = v; pixel->b = t;
            break;
        case 3:
            pixel->r = p; pixel->g = q; pixel->b = v;
            break;
        case 4:
            pixel->r = t; pixel->g = p; pixel->b = v;
            break;
        default:
            pixel->r = v; pixel->g = p; pixel->b = q;
            break;
    }
}

// ==========================================================================================================================================================================
void saveFrame_quad(const char*filename, uint8**I0, uint8**I1, uint32**I2, uint32**I3, int nbLabel, MeteorROI* stats, int i0, int i1, int j0, int j1)
// ==========================================================================================================================================================================
{
    int w = (j1-j0+1);
    int h = (i1-i0+1);


    char buffer[80];

    FILE *file;

    rgb8** img = rgb8matrix(0, 2*h-1, 0, 2*w-1);
    if (img == NULL) return;

    // (0,0) : video
    for (int i=i0 ; i<=i1 ; i++) {
        for (int j=j0 ; j<=j1 ; j++) {
            img[i][j].r = I0[i][j];
            img[i][j].g = I0[i][j];
            img[i][j].b = I0[i][j];
        }
    }
    // (0,1) : seuillage luminosité
    for (int i=i0 ; i<=i1 ; i++) {
        for (int j=j0 ; j<=j1 ; j++) {
            img[i][j+w].r = I1[i][j];
            img[i][j+w].g = I1[i][j];
            img[i][j+w].b = I1[i][j];
        }
    }
    // (1,0) : CCL
    for (int i=i0 ; i<=i1 ; i++) {
        for (int j=j0 ; j<=j1 ; j++) {
            if(I2[i][j] != 0){
                HsvToRgb((img[i+h])+ j, (uint8)(I2[i][j]*360/5), 255, 255);
            }
            else{
                HsvToRgb((img[i+h])+ j, 0, 0, 0);
            }
        }
    }
    // Carrés vert sur les CC
    // for (int k=1 ; k<=nbLabel ; k++) {
    //     if (stats[k].S > 3) {
    //         rgb8 color;
    //         //HsvToRgb(&color, k*255 / nbLabel, 255, 255);
    //         color.r = 0;
    //         color.g = 255;
    //         color.b = 0;
    //         int ymin = min(stats[k].ymin + h - 10, h*2-1);
    //         int ymax = min(stats[k].ymax + h + 10, h*2-1);
    //         int xmin = min(stats[k].xmin     - 10, w  -1);
    //         int xmax = min(stats[k].xmax     + 10, w  -1);
            
    //         for (int b=0; b<5 ; b++) {
    //             ymin++; ymax--;xmin++;xmax--;
    //             for (int i=ymin ; i<=ymax ; i++) {
    //                 img[i][xmin] = color;
    //                 img[i][xmax] = color;
    //             }
    //             for (int j=xmin ; j<=xmax ; j++) {
    //                 img[ymin][j] = color;
    //                 img[ymax][j] = color;
    //             }
    //         }
    //     }
    // }


    // (1,1) : filtrage surface
    for (int i=i0 ; i<=i1 ; i++) {
        for (int j=j0 ; j<=j1 ; j++) {
            img[i+h][j+w].r = (I3[i][j] == 0)? 0 : 255;
            img[i+h][j+w].g = (I3[i][j] == 0)? 0 : 255;
            img[i+h][j+w].b = (I3[i][j] == 0)? 0 : 255;
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL)
      nrerror("ouverture du fichier %s impossible dans saveVideoFrame_quad\n");

    /* enregistrement de l'image au format rpgm */

    sprintf(buffer,"P6\n%d %d\n255\n",(int)(2*w-1), (int)(2*h-1));
    fwrite(buffer,strlen(buffer),1,file);
    for(int i=0; i<=2*h-1; i++)
      WritePNMrow((uint8*)img[i], 2*w-1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix(img, 0, 2*h-1, 0, 2*w-1);
}



// ==========================================================================================================================================================================
void saveFrame_quad_hysteresis(const char*filename, uint8**I0, uint32**SH, uint32**SB, uint32**Y, int i0, int i1, int j0, int j1)
// ==========================================================================================================================================================================
{
    int w = (j1-j0+1);
    int h = (i1-i0+1);


    char buffer[80];

    FILE *file;

    rgb8** img = rgb8matrix(0, 2*h-1, 0, 2*w-1);
    if (img == NULL) return;

    // (0,0) : video
    for (int i=i0 ; i<=i1 ; i++) {
        for (int j=j0 ; j<=j1 ; j++) {
            img[i][j].r = I0[i][j];
            img[i][j].g = I0[i][j];
            img[i][j].b = I0[i][j];
        }
    }
    // (0,1) : seuillage low
    for (int i=i0 ; i<=i1 ; i++) {
        for (int j=j0 ; j<=j1 ; j++) {
            if(SB[i][j] != 0){
                HsvToRgb((img[i])+ j+w, (uint8)(SB[i][j]*360/5), 255, 255);
            }
            else{
                HsvToRgb((img[i])+ j+w, 0, 0, 0);
            }
        }
    }

    // (0,1) : seuillage high
    for (int i=i0 ; i<=i1 ; i++) {
        for (int j=j0 ; j<=j1 ; j++) {
            if(SH[i][j] != 0){
                HsvToRgb((img[i+h])+ j, (uint8)(SH[i][j]*360/5), 255, 255);
            }
            else{
                HsvToRgb((img[i+h])+ j, 0, 0, 0);
            }
        }
    }
    
    // (0,0) : out
    for (int i=i0 ; i<=i1 ; i++) {
        for (int j=j0 ; j<=j1 ; j++) {
            img[i+h][j+w].r = (Y[i][j] == 0) ? 0 : 255;
            img[i+h][j+w].g = (Y[i][j] == 0) ? 0 : 255;
            img[i+h][j+w].b = (Y[i][j] == 0) ? 0 : 255;
        }
    }

    file = fopen(filename, "wb");
    if (file == NULL)
      nrerror("ouverture du fichier %s impossible dans saveVideoFrame_quad\n");

    /* enregistrement de l'image au format rpgm */

    sprintf(buffer,"P6\n%d %d\n255\n",(int)(2*w-1), (int)(2*h-1));
    fwrite(buffer,strlen(buffer),1,file);
    for(int i=0; i<=2*h-1; i++)
      WritePNMrow((uint8*)img[i], 2*w-1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix(img, 0, 2*h-1, 0, 2*w-1);
}



// ==========================================================================================================================================================================
void saveMax(const char*filename, uint8**I, int i0, int i1, int j0, int j1)
// ==========================================================================================================================================================================
{
    uint8 m;
    uint8* res = ui8vector(i0, i1);

    zero_ui8vector(res, i0, i1);

    for (int i = i0; i <= i1 ; i++) {
        m = 0;
        for (int j = j0 ; j <= j1 ; j++){
            m = max(m, I[i][j]);
        }
        res[i] = m;
    }


    FILE *f = fopen(filename, "w");
    if (f == NULL){
        printf("error ouverture %s \n", filename);
        exit(1);
    }
    
    for(int i = i1; i>= i0; i--){
        fprintf(f, "%4d\t%4d\n", i, res[i]);

    }
    fclose(f);
}

// =========================================================================================================================================================================
void split_path_file(char** p, char** f, char *pf) 
// =========================================================================================================================================================================
{
    assert(pf != NULL);
    assert(p != NULL);
    assert(f != NULL);

    char *slash = pf, *next;
    while ((next = strpbrk(slash + 1, "\\/"))) 
        slash = next;

    if (pf != slash) slash++;
    *p = strndup(pf, slash - pf);
    next = strpbrk(slash+1, ".");
    *f = strndup(slash, next-slash);
}

// =========================================================================================================================================================================
void get_data_from_tracks_path(char* path, int *light_min, int *light_max, char **filename) 
// =========================================================================================================================================================================
{
    char *res, *tmp, *min, *max;
    res = strstr(path, "SB_");
    res += 3;
    tmp = strchr(res, '_');
    min = strndup(res, tmp - res);
    *light_min = atoi(min);

    res = tmp + 4;
    tmp = strchr(res, '/');
    max = strndup(res, tmp - res);
    *light_max = atoi(max);

    res = tmp + 1;
    tmp = strchr(res, '/');
    *filename = strndup(res, tmp - res);

    idisp(*light_min);
    idisp(*light_max);
    disp(*filename);
    free(min);
    free(max);
}

// =========================================================================================================================================================================
void get_bouding_box_path_from_tracks_path(char* path_tracks)
// =========================================================================================================================================================================
{
    assert(path_tracks != NULL);
    // assert(path_bounding_box != NULL);

    char *slash = path_tracks, *next;
    while ((next = strpbrk(slash + 1, "\\/"))) 
        slash = next;

    if (path_tracks != slash) slash++;
    next = strndup(path_tracks, slash - path_tracks);
    disp(next);
    
    // *path_bb = malloc(sizeof(strlen(next)) + strlen("/bounding_box.txt") + 1);
    sprintf(path_bounding_box, "%sbounding_box.txt", next);
    disp(path_bounding_box);
    disp(next);
    free(next);
}


// ==========================================================================================================================================================================
void create_debug_dir(char *output_dest, char *filename, int light_min, int light_max, int edt)
// ==========================================================================================================================================================================
{
        char tmp_asso[100], tmp_stats[100];
        char tmp_debug[100];
        char path_assoconflicts[100], path_stats[100];
        struct stat status = { 0 };
        sprintf(tmp_debug,    "%s/debug",                            output_dest);
        sprintf(tmp_asso,     "%s/assoconflicts",                   tmp_debug);
        sprintf(tmp_stats,    "%s/stats",                           tmp_debug);


        if ((light_min != -1) && (light_max != -1)){
                sprintf(path_assoconflicts,     "%s/SB_%d_SH_%d",                               tmp_asso, light_min, light_max);
                sprintf(path_stats,             "%s/SB_%d_SH_%d",                               tmp_stats, light_min, light_max);
                sprintf(path_assoconflicts_f,   "%s/%s",                                        path_assoconflicts, filename);
                sprintf(path_stats_f,           "%s/%s",                                        path_stats, filename);
                sprintf(path_motion,            "%s/motion.txt",                                 path_assoconflicts_f);
                sprintf(path_bounding_box,      "%s/bounding_box.txt",                           path_assoconflicts_f);
                sprintf(path_extraction,        "%s/extraction.txt",                             path_assoconflicts_f);
                sprintf(path_error,             "%s/error.txt",                                  path_assoconflicts_f);
                sprintf(path_tracks,            "%s/tracks.txt",                                 path_assoconflicts_f);
                sprintf(path_debug,             "%s/%s.txt",                                     path_assoconflicts_f, filename);
                goto next;
        }
        // default
        if( stat("./debug", &status) == -1 ) {
              mkdir( "./debug", 0700 );
        }
        return ; 
next: 
        if( stat(tmp_debug, &status) == -1 ) {
                mkdir( tmp_debug, 0700 );
        }
        if( stat(tmp_asso, &status) == -1 ) {
                mkdir( tmp_asso, 0700 );
        }
        if( stat(tmp_stats, &status) == -1 ) {
                mkdir( tmp_stats, 0700 );
        }        
        if( stat(path_assoconflicts, &status) == -1 ) {
                mkdir(path_assoconflicts, 0700 );
        }
        if( stat(path_assoconflicts_f, &status) == -1 ) {
                mkdir(path_assoconflicts_f, 0700 );
        }
        if( stat(path_stats, &status) == -1 ) {
                mkdir(path_stats, 0700 );
        }
        if( stat(path_stats_f, &status) == -1 ) {
                mkdir(path_stats_f, 0700 );
        }
}


// ==========================================================================================================================================================================
void create_frames_dir(char *dest_path, char *filename, int light_min, int light_max, int edt)
// ==========================================================================================================================================================================
{
        char path_frames[100];
        char path_frames_f[150];
        struct stat status = { 0 };

        if ((light_min != -1) && (light_max != -1)){
                sprintf(path_frames,            "%sSB_%d_SH_%d/",                 dest_path, light_min, light_max);
                sprintf(path_frames_f,          "%s%s/",                          path_frames, filename);
                sprintf(path_frames_binary_dir, "%sbinary/",                      path_frames_f);
                sprintf(path_frames_output_dir, "%soutput/",                      path_frames_f);
                goto next;
        }
        // default
        if( stat("./frames", &status) == -1 ) {
              mkdir( "./frames", 0700 );
        }

        return ; 
next: 
        if( stat(dest_path, &status) == -1 ) {
              mkdir( dest_path, 0700 );
        }
        if( stat(path_frames, &status) == -1 ) {
                mkdir( path_frames, 0700 );
        }
        if( stat(path_frames_f, &status) == -1 ) {
                mkdir( path_frames_f, 0700 );
        }
        if( stat(path_frames_binary_dir, &status) == -1 ) {
                mkdir( path_frames_binary_dir, 0700 );
        }
        if( stat(path_frames_output_dir, &status) == -1 ) {
                mkdir( path_frames_output_dir, 0700 );
        }
}

// ==========================================================================================================================================================================
void create_video_dir(char *dest_path, char *filename, int light_min, int light_max, int edt)
// ==========================================================================================================================================================================
{
        char path_video[150];
        struct stat status = { 0 };


        if ((light_min != -1) && (light_max != -1)){
                sprintf(path_video,            "%sSB_%d_SH_%d/",                               dest_path, light_min, light_max);
                sprintf(path_video_f,          "%s%s/",                                        path_video, filename);
                goto next;
        }
        // default
        if( stat("./debug", &status) == -1 ) {
              mkdir( "./debug", 0700 );
        }
        return ; 
next: 
        if( stat(dest_path, &status) == -1 ) {
              mkdir( dest_path, 0700 );
        }
        if( stat(path_video, &status) == -1 ) {
                mkdir(path_video, 0700 );
        }
        if( stat(path_video_f, &status) == -1 ) {
                mkdir( path_video_f, 0700 );
        }
        create_videos_files(filename);
}


// ==========================================================================================================================================================================
void create_debug_files(int frame)
// ==========================================================================================================================================================================
{
    sprintf(path_stats_0,           "%s%05d.txt",      path_stats_f, frame);
    sprintf(path_stats_1,           "%s%05d.txt",      path_stats_f, frame+1);
}

// ==========================================================================================================================================================================
void create_frames_files(int frame)
// ==========================================================================================================================================================================
{
    sprintf(path_frames_binary,     "%s%05d.ppm",      path_frames_binary_dir, frame);
    sprintf(path_frames_output,     "%s%05d.ppm",      path_frames_output_dir, frame);
}

// ==========================================================================================================================================================================
void create_videos_files(char *filename)
// ==========================================================================================================================================================================
{
    sprintf(path_video_tracking,    "%s%s.mp4",      path_video_f,     filename);
}

/* ----------------------------------------------------------------------------- */
void copy_ui8matrix_ui8matrix(uint8 **X, int i0, int i1, int j0, int j1, uint8 **Y)
/* ----------------------------------------------------------------------------- */
{
    int i, j;

    for(i=i0; i<=i1; i++) {
        for(j=j0; j<=j1; j++) {
            Y[i][j] = X[i][j];
        }
    }
}

/* -------------------------------------------------------------------------------- */
void convert_ui8vector_ui32vector(uint8 *X, long nl, long nh, uint32 *Y)
/* -------------------------------------------------------------------------------- */
{
    long i;
    for(i=nl; i<=nh; i++)
        Y[i] = (uint32) X[i];
}

/* ---------------------------------------------------------------------------------------------------- */
void convert_ui8matrix_ui32matrix(uint8 **X, int nrl, int nrh, int ncl, int nch, uint32 **Y)
/* ---------------------------------------------------------------------------------------------------- */
{
    long i;
    for(i=nrl; i<=nrh; i++) {
        convert_ui8vector_ui32vector(X[i], ncl, nch, Y[i]);
    }
}

/* ------------------------------------------------------- */
void WritePNMrow(uint8  *line, int width, FILE  *file)
/* ------------------------------------------------------- */
{
    /* Le fichier est deja ouvert et ne sera pas ferme a la fin */

    fwrite(&(line[0]), sizeof(byte), 3*sizeof(byte)*width, file);
}
