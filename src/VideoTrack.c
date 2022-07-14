/*
 * Copyright (c) 2022, Clara Ciocan/ Mathuran Kandeepan
 * LIP6
 */ 

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "nrutil.h"
#include "Features.h"
#include "Tracking.h"
#include "DebugUtil.h"
#include "Video.h"
#include "Validation.h"

#define SIZE_MAX_METEORROI 3000
#define SIZE_MAX_TRACKS 1000
#define TOLERANCE_DISTANCEMIN 20

#define GREEN 1
#define RED 2
#define ORANGE 3

/*DEBUG*/
extern char path_video_tracking[200];
extern char path_bounding_box[200];
extern char path_tracks[200];
extern char path_frame[200];

typedef struct coordBB {
    int rx;
    int ry;
    int bb_x;
    int bb_y;
    int color;
} coordBB;

coordBB listBB[200];

// ==============================================================================================================================
void saveVideoFrame(const char*filename, uint8** I, int i0, int i1, int j0, int j1)
// ==============================================================================================================================
{
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
    ffmpeg_write2d(&writer, (uint8_t**)img);
    free_rgb8matrix(img, 0, i1, 0, j1);

}

void videoCopyToRGB(const char *filename, Video *video, uint8 **I, int i0, int i1, int j0, int j1)
{
    while(Video_nextFrame(video,I)) {
        int frame = video->frame_current - 1;
		printf("[Frame] %-4d\n", frame);
        saveVideoFrame(filename, I, i0, i1, j0, j1);
    }
}

// ==============================================================================================================================
void saveVideoFrame_BB(const char*filename, uint8** I, int rx, int ry, int bb_x, int bb_y, int i0, int i1, int j0, int j1, int color)
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

    if (rx != -1 && ry != -1 && bb_x != -1 && bb_y != -1){
        int ymin = clamp(bb_y - ry, 1, i1-1);
        int ymax = clamp(bb_y + ry, 1, i1-1);
        int xmin = clamp(bb_x - rx, 1, j1-1);
        int xmax = clamp(bb_x + rx, 1, j1-1);
        switch (color)
        {
        case GREEN:
            plot_bouding_box(img, ymin,ymax,xmin,xmax, 2, green);
            break;
        case RED:
            plot_bouding_box(img, ymin,ymax,xmin,xmax, 2, red);
            break;
        case ORANGE:
            plot_bouding_box(img, ymin,ymax,xmin,xmax, 2, orange);
            break;
        default:
            break;
        }
    }

    ffmpeg_write2d(&writer, (uint8_t**)img);
    free_rgb8matrix(img, 0, i1, 0, j1);
}


// ==============================================================================================================================
void saveVideoFrame_listBB(const char*filename, uint8** I, int cpt, int i0, int i1, int j0, int j1)
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

    for(int i = 0; i < cpt; i++){
            int ymin = clamp(listBB[i].bb_y - listBB[i].ry, 1, i1-1);
            int ymax = clamp(listBB[i].bb_y + listBB[i].ry, 1, i1-1);
            int xmin = clamp(listBB[i].bb_x - listBB[i].rx, 1, j1-1);
            int xmax = clamp(listBB[i].bb_x + listBB[i].rx, 1, j1-1);
            switch (listBB[i].color )
            {
            case GREEN:
                plot_bouding_box(img, ymin,ymax,xmin,xmax, 2, green);
                break;
            case RED:
                plot_bouding_box(img, ymin,ymax,xmin,xmax, 2, red);
                break;
            case ORANGE:
                plot_bouding_box(img, ymin,ymax,xmin,xmax, 2, orange);
                break;
            default:
                break;
            }
    }
    ffmpeg_write2d(&writer, (uint8_t**)img);
    free_rgb8matrix(img, 0, i1, 0, j1);
}

// ==============================================================================================================================
void saveFrame_listBB(const char*filename, uint8** I, int cpt, int i0, int i1, int j0, int j1)
// ==============================================================================================================================
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

    for (int i=i0 ; i<=i1 ; i++) {
        for (int j=j0 ; j<=j1 ; j++) {
            img[i][j].r = I[i][j];
            img[i][j].g = I[i][j];
            img[i][j].b = I[i][j];
        }
    }

    for(int i = 0; i < cpt; i++){
            int ymin = clamp(listBB[i].bb_y - listBB[i].ry, 1, i1-1);
            int ymax = clamp(listBB[i].bb_y + listBB[i].ry, 1, i1-1);
            int xmin = clamp(listBB[i].bb_x - listBB[i].rx, 1, j1-1);
            int xmax = clamp(listBB[i].bb_x + listBB[i].rx, 1, j1-1);
            switch (listBB[i].color )
            {
            case GREEN:
                plot_bouding_box(img, ymin,ymax,xmin,xmax, 2, green);
                break;
            case RED:
                plot_bouding_box(img, ymin,ymax,xmin,xmax, 2, red);
                break;
            case ORANGE:
                plot_bouding_box(img, ymin,ymax,xmin,xmax, 2, orange);
                break;
            default:
                break;
            }

    }

    file = fopen(filename, "wb");
    if (file == NULL)
      nrerror("ouverture du fichier %s impossible dans saveFrame_listBB\n");

    /* enregistrement de l'image au format rpgm */

    sprintf(buffer,"P6\n%d %d\n255\n",(int)(w-1), (int)(h-1));
    fwrite(buffer,strlen(buffer),1,file);
    for(int i=0; i<=h-1; i++)
      WritePNMrow((uint8*)img[i], w-1, file);

    /* fermeture du fichier */
    fclose(file);

    free_rgb8matrix(img, 0, h-1, 0, w-1);

}

void printList(int cpt){
    for (int i = 0; i<cpt; i++){
        printf("%d %d %d\n", listBB[i].rx, listBB[i].ry, listBB[i].color);
    }
}
void addToListBB(int rx, int ry, int bb_x, int bb_y, int color, int i)
{
        (listBB + i)->bb_x = bb_x;
        (listBB + i)->bb_y = bb_y;
        (listBB + i)->rx   = rx;
        (listBB + i)->ry   = ry;
        (listBB + i)->color   = color;

}

void calc_BB(Track* tracks, int n){
    for (int i = 0; i < n ; i++){
        
        int dirX = tracks[i].end.x > tracks[i].begin.x; // vers la droite 
        int dirY = tracks[i].end.y > tracks[i].begin.y; // vers le bas

        if(dirX){
                if(dirY){
                    tracks[i].ymin   = tracks[i].begin.y - TOLERANCE_DISTANCEMIN;   tracks[i].xmin   = tracks[i].begin.x - TOLERANCE_DISTANCEMIN;
                    tracks[i].ymax   = tracks[i].end.y   + TOLERANCE_DISTANCEMIN;   tracks[i].xmax   = tracks[i].end.x   + TOLERANCE_DISTANCEMIN;
                }else {
                    tracks[i].ymin   = tracks[i].end.y   - TOLERANCE_DISTANCEMIN;   tracks[i].xmin   = tracks[i].begin.x - TOLERANCE_DISTANCEMIN;
                    tracks[i].ymax   = tracks[i].begin.y + TOLERANCE_DISTANCEMIN;   tracks[i].xmax   = tracks[i].end.x   + TOLERANCE_DISTANCEMIN;
                }
                
        } else {
                if(dirY) {
                    tracks[i].ymin   = tracks[i].begin.y - TOLERANCE_DISTANCEMIN;   tracks[i].xmin   = tracks[i].end.x   - TOLERANCE_DISTANCEMIN;
                    tracks[i].ymax   = tracks[i].end.y   + TOLERANCE_DISTANCEMIN;   tracks[i].xmax   = tracks[i].begin.x + TOLERANCE_DISTANCEMIN;
                }else{
                    tracks[i].ymin   = tracks[i].end.y   - TOLERANCE_DISTANCEMIN;   tracks[i].xmin   = tracks[i].end.x   - TOLERANCE_DISTANCEMIN;
                    tracks[i].ymax   = tracks[i].begin.y + TOLERANCE_DISTANCEMIN;   tracks[i].xmax   = tracks[i].begin.x + TOLERANCE_DISTANCEMIN;
                }
        }
    }
}

void test_validation_routine(int argc, char** argv)

{

    if (find_arg(argc, argv, "-h")) {
        fprintf(stderr, "usage: %s %s [options] <input_file>\n", argv[0], argv[1]);
        fprintf(stderr, "  -input_video  : Path vers le fichier avec les tracks\n");
        fprintf(stderr, "  -input_video  : Path vers le fichier avec les tracks\n");
        fprintf(stderr, "  -validation   : Fichier contenant la vérité terrain de la séquence\n");
        fprintf(stderr, "  -start_frame  : Image de départ dans la séquence\n");
        fprintf(stderr, "  -end_frame    : Dernière image de la séquence\n");
        exit(1);
    }

    // Parsing Arguments
    char* src_path          = find_char_arg (argc, argv, "-input_tracks",   NULL);
    char* src_path_video    = find_char_arg (argc, argv, "-input_video",    NULL);
    char* validation        = find_char_arg (argc, argv, "-validation",     NULL);
    int light_min           = find_int_arg  (argc, argv, "-light_min",      60); 
    int light_max           = find_int_arg  (argc, argv, "-light_max",      85); 
    int start               = find_int_arg  (argc, argv, "-start_frame",    0);
    int end                 = find_int_arg  (argc, argv, "-end_frame",      10000);

    //int n = 0;
    int b = 1;
    int i0, i1, j0, j1;
	int color = 0;
    int frame, frame_bb;
    uint16 rx, ry, bb_x, bb_y;

    if (!src_path || !src_path_video){
        printf("Input(s) missing\n");
        exit(1);
    }

    char *path;
    char *filename;
    disp(src_path_video);
    split_path_file(&path, &filename, src_path_video);
    disp(filename);
    
    Track tracks[SIZE_MAX_TRACKS];

    int nb_tracks = 0;
	init_Track(tracks, SIZE_MAX_TRACKS);
        
    // debug/output paths and files
  	create_debug_dir (filename, light_min , light_max, -1);
	create_videos_files(filename);

    disp(path_tracks);
    disp(path_bounding_box);
    
    // recupere les tracks
    parseTracks(path_tracks, tracks, &nb_tracks);
    printTracks(tracks, nb_tracks);
    
    // init 
    Video* video = Video_init_from_file(src_path_video, start, end, 0, &i0, &i1, &j0, &j1);

    uint8 **I0    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    
    // validation pour établir si une track est vrai/faux positif
    if (validation) {
        disp(validation);
        Validation(validation, tracks, nb_tracks, "./debug/");
        Validation_final();
        Validation_free();
    }
    else {
        PUTS("NO VALIDATION");
    }

    // open BB pour l'affichage des rectangles englobants
    FILE * file_bb = fopen(path_bounding_box, "r"); 
    if (file_bb == NULL) {
        fprintf(stderr, "cannot open file yo\n");
        return;
    }

    // calculs des BB (bounding box) des tracks 
    calc_BB(tracks, nb_tracks);

    // parcours des BB à afficher
    char lines[1000];
    fgets(lines, 100, file_bb);
    sscanf(lines, "%d %d %d %d %d ", (int*)&frame_bb, (int*)&rx, (int*)&ry, (int*)&bb_x, (int*)&bb_y);
    printf("%d %d %d %d %d \n", frame_bb, rx, ry, bb_x, bb_y);

    // parcours de la video
    while(Video_nextFrame(video,I0)) {
        frame = video->frame_current - 1;
		printf("[Frame] %-4d\n", frame);
        int cpt = 0;

        // affiche tous les BB de l'image
        while(frame_bb == frame){
            printf("for %d %d %d \n", frame_bb, bb_x, bb_y);
            
            // cherche la piste correspondante
            for (int i = 0; i < nb_tracks ; i++){
                if (tracks[i].timestamp <= frame  && frame <= tracks[i].timestamp+tracks[i].time &&
                    tracks[i].xmin <= bb_x  && bb_x <= tracks[i].xmax  &&
                    tracks[i].ymin <= bb_y  && bb_y <= tracks[i].ymax){
                        if(validation)
                            color = tracks[i].is_valid ? GREEN : RED;
                        else
                            color = ORANGE;
                        addToListBB(rx, ry, bb_x, bb_y, color, cpt++);
                        break;
                }
            }
            if(fgets(lines, 100, file_bb)==NULL){
                frame_bb = -1;
                break;
            }
            // cherche prochain BB à afficher
            sscanf(lines, "%d %d %d %d %d ", (int*)&frame_bb, (int*)&rx, (int*)&ry, (int*)&bb_x, (int*)&bb_y);
            printf("%d %d %d %d %d \n", frame_bb, rx, ry, bb_x, bb_y);
        }
        saveVideoFrame_listBB(path_video_tracking, I0, cpt, i0, i1, j0, j1);
    }
    
    free_ui8matrix(I0, i0-b, i1+b, j0-b, j1+b);
}

/*
COOL POUR DEBUG mais obsolète
void test_validation_routine_frame(int argc, char** argv)

{

    if (find_arg(argc, argv, "-h")) {
        fprintf(stderr, "usage: %s %s [options] <input_file>\n", argv[0], argv[1]);
        fprintf(stderr, "  -input        : Path vers le fichier avec les tracks\n");
        fprintf(stderr, "  -input_frames : Path vers le fichier avec les tracks\n");
        fprintf(stderr, "  -validation   : Fichier contenant la vérité terrain de la séquence\n");
        fprintf(stderr, "  -start_frame  : Image de départ dans la séquence\n");
        fprintf(stderr, "  -end_frame    : Dernière image de la séquence\n");
        exit(1);
    }

    // Parsing Arguments
    char* src_path   = find_char_arg (argc, argv, "-input",      NULL); //  tracks
    char* src_path_video   = find_char_arg (argc, argv, "-input_video",      NULL); // path_frame
    char* validation = find_char_arg (argc, argv, "-validation", NULL); // validation
    int start        = find_int_arg  (argc, argv, "-start_frame", 0 );
    int end          = find_int_arg  (argc, argv, "-end_frame", 10000);
    char *filename   = find_char_arg (argc, argv, "-filename",      NULL); //  filename

    disp(filename); 

    int n = 0;
    int b = 1;
    int i0, i1, j0, j1;
    int light_min = 60;
    int light_max = 85;
	int color = 0;
	int color2 = 0;

    Track tracks[SIZE_MAX_TRACKS];

    int nb_tracks = 0;
	init_Track(tracks, SIZE_MAX_TRACKS);
        
  	create_debug_dir (filename, light_min , light_max, -1);
	create_frames_dir(filename, light_min , light_max, -1);
	create_videos_files(filename);

    char path_track[250];
    
    printf("%s\n",path_tracks); 
    sprintf(path_track,"../Tau-h/%s", path_tracks);

    printf("%s\n",path_bounding_box);
    parseTracks(path_track, tracks, &nb_tracks);
    printTracks(tracks, nb_tracks);
    
    // Video* video = Video_init_from_file(src_path_video, start, end, 0, &i0, &i1, &j0, &j1); 
    i0 = 0; i1 = 1199; j0 = 0; j1 = 1899;

    uint8 **I0    = ui8matrix(i0-b, i1+b, j0-b, j1+b);


    char path_bb[250];
    int frame, frame_bb;
    uint16 rx, ry, bb_x, bb_y;
    sprintf(path_bb,"../Tau-h/%s", path_bounding_box );

    printf("%s\n",path_bb);

    FILE * file_bb = fopen(path_bb, "r"); 
    disp(path_bb);
    if (file_bb == NULL) {
        fprintf(stderr, "cannot open file yo\n");
        return;
    }

    if (validation) {
        disp(validation);
        Validation(validation, tracks, nb_tracks, "./debug/");
        Validation_final();
        Validation_free();
    }

    printTracks(tracks, nb_tracks);
    calc_BB(tracks, nb_tracks);

    //
    frame = start; 
    char lines[1000];
    char src[300];

    fgets(lines, 100, file_bb);
    sscanf(lines, "%d %d %d %d %d ", (int*)&frame_bb, (int*)&rx, (int*)&ry, (int*)&bb_x, (int*)&bb_y);
    printf("%d %d %d %d %d \n", frame_bb, rx, ry, bb_x, bb_y);

    while(frame <= end) {
        sprintf(src, "%s%04d.pgm", src_path_video, frame+1); 
        MLoadPGM_ui8matrix(src, i0, i1, j0, j1, I0);
		printf("[Frame] %-4d\n", frame);

        create_debug_files (frame);
	    create_frames_files(frame);
        int cpt = 0;

        while(frame_bb == frame){
            printf("for %d %d %d \n", frame_bb, bb_x, bb_y);
            for (int i = 0; i < nb_tracks ; i++){
                // if (tracks[i].timestamp == frame && tracks[i].bb_x == bb_x && tracks[i].bb_y == bb_y){
                if (tracks[i].timestamp <= frame  && frame <= tracks[i].timestamp+tracks[i].time &&
                    tracks[i].xmin <= bb_x  && bb_x <= tracks[i].xmax  &&
                    tracks[i].ymin <= bb_y  && bb_y <= tracks[i].ymax){
                        color = tracks[i].is_valid ? GREEN : RED;
                        addToListBB(rx, ry, bb_x, bb_y, color, cpt++);
                        break;
                }
            }
            if(fgets(lines, 100, file_bb)==NULL){
                frame_bb = -1;
                break;
            }
            sscanf(lines, "%d %d %d %d %d ", (int*)&frame_bb, (int*)&rx, (int*)&ry, (int*)&bb_x, (int*)&bb_y);
            printf("%d %d %d %d %d \n", frame_bb, rx, ry, bb_x, bb_y);
        }
        saveVideoFrame_listBB(path_video_tracking, I0, cpt, i0, i1, j0, j1);
        // saveFrame_listBB(path_frame, I0, cpt, i0, i1, j0, j1);

        frame++;
    }

    
    free_ui8matrix(I0, i0-b, i1+b, j0-b, j1+b);
}
*/


int main(int argc, char** argv)
{
    test_validation_routine(argc, argv);
    
    return 0;
}
