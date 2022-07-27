/*
 * Copyright (c) 2022, Clara Ciocan/ Mathuran Kandeepan
 * LIP6
 */ 

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <nrc2.h>

#include "Features.h"
#include "Tracking.h"
#include "DebugUtil.h"
#include "Video.h"
#include "Validation.h"

#define SIZE_MAX_METEORROI 3000
#define SIZE_MAX_TRACKS 1000
#define TOLERANCE_DISTANCEMIN 20

#define GREEN  1
#define RED    2
#define ORANGE 3
#define BLUE   4
#define YELLOW 5
#define MISC  6

/*DEBUG*/
extern char path_video_tracking[200];
extern char path_bounding_box[200];
// extern char path_tracks[200];
extern char path_frames_output[250];

typedef struct coordBB {
    int rx;
    int ry;
    int bb_x;
    int bb_y;
    int color;
} coordBB;

coordBB listBB[200];

// ==============================================================================================================================
rgb8 get_color(int color)
// ==============================================================================================================================
{
    rgb8 green;     rgb8 red;       rgb8 blue;      rgb8 orange;      rgb8 yellow;      rgb8 misc;     
    green.g = 255;  red.g = 000;    blue.g = 000;   orange.r = 255;   yellow.g = 255;   misc.g = 255;
    green.b = 000;  red.b = 000;    blue.b = 255;   orange.g = 165;   yellow.b = 000;   misc.b = 153;
    green.r = 000;  red.r = 255;    blue.r = 000;   orange.b = 000;   yellow.r = 255;   misc.r = 153;

    switch (color)
    {
        case GREEN:
            return green;
        case RED:
            return red;
        case ORANGE:
            return orange;
        case BLUE:
            return blue;
        case YELLOW:
            return yellow;
        case MISC:
            return misc;

        default:
            break;
    }
    return red;
}

// ==============================================================================================================================
void saveVideoFrame_listBB(const char*filename, uint8** I, int cpt, int i0, int i1, int j0, int j1)
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

    for(int i = 0; i < cpt; i++){
            int ymin = clamp(listBB[i].bb_y - listBB[i].ry, 1, i1-1);
            int ymax = clamp(listBB[i].bb_y + listBB[i].ry, 1, i1-1);
            int xmin = clamp(listBB[i].bb_x - listBB[i].rx, 1, j1-1);
            int xmax = clamp(listBB[i].bb_x + listBB[i].rx, 1, j1-1);
            plot_bouding_box(img, ymin,ymax,xmin,xmax, 2, get_color(listBB[i].color));
    }
    ffmpeg_write2d(&writer, (uint8_t**)img);
    free_rgb8matrix(img, 0, i1, 0, j1);
}

// ==============================================================================================================================
void saveFrame_listBB(const char*filename, uint8** I, int cpt, int i0, int i1, int j0, int j1)
// ==============================================================================================================================
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

    for(int i = 0; i < cpt; i++){
            int ymin = clamp(listBB[i].bb_y - listBB[i].ry, 1, i1-1);
            int ymax = clamp(listBB[i].bb_y + listBB[i].ry, 1, i1-1);
            int xmin = clamp(listBB[i].bb_x - listBB[i].rx, 1, j1-1);
            int xmax = clamp(listBB[i].bb_x + listBB[i].rx, 1, j1-1);
            plot_bouding_box(img, ymin,ymax,xmin,xmax, 2, get_color(listBB[i].color));
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

// ==============================================================================================================================
void printList(int cpt){
// ==============================================================================================================================
    for (int i = 0; i<cpt; i++){
        printf("%d %d %d\n", listBB[i].rx, listBB[i].ry, listBB[i].color);
    }
}

// ==============================================================================================================================
void addToListBB(int rx, int ry, int bb_x, int bb_y, int color, int i)
// ==============================================================================================================================
{
        (listBB + i)->bb_x = bb_x;
        (listBB + i)->bb_y = bb_y;
        (listBB + i)->rx   = rx;
        (listBB + i)->ry   = ry;
        (listBB + i)->color   = color;
}

// ==============================================================================================================================
void calc_BB(Track* tracks, int n, int i0, int i1, int j0, int j1)
// ==============================================================================================================================
{
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

        if(tracks[i].xmin < j0) tracks[i].xmin = j0;
        if(tracks[i].ymin < i0) tracks[i].ymin = i0;
        if(tracks[i].xmax > j1) tracks[i].xmax = j1;
        if(tracks[i].ymax > i1) tracks[i].ymax = i1;
    }
}

// ==============================================================================================================================
void main_visu(int argc, char** argv)
// ==============================================================================================================================
{
    // default values
    char* def_input_tracks  =             NULL;
    char* def_input_video   =             NULL;
    char* def_output_video  = "./out_visu.mp4";
    char* def_output_frames =             NULL;
    char* def_validation    =             NULL;

    if (find_arg(argc, argv, "-h")) {
        fprintf(stderr, "  --input-tracks     Path vers le fichier avec les tracks                [%s]\n", def_input_tracks );
        fprintf(stderr, "  --input-video      Path vers la video                                  [%s]\n", def_input_video  );
        fprintf(stderr, "  --output-video     Output de la video (MPEG-4 format)                  [%s]\n", def_output_video );
        fprintf(stderr, "  --output-frames    Path to the frames output                           [%s]\n", def_output_frames);
        fprintf(stderr, "  --validation       Fichier contenant la vérité terrain de la séquence  [%s]\n", def_validation   );
        fprintf(stderr, "  -h                 This help                                               \n"                   );
        exit(1);
    }

    // Parsing Arguments
    char *src_path         = find_char_arg (argc, argv, "--input-tracks",  def_input_tracks);
    char *src_path_video   = find_char_arg (argc, argv, "--input-video",   def_input_video);
    char *dest_path_video  = find_char_arg (argc, argv, "--output-video",  def_output_video);
    char *dest_path_frames = find_char_arg (argc, argv, "--output-frames", def_output_frames);
    char *validation       = find_char_arg (argc, argv, "--validation",    def_validation);

    // heading display
    printf("#  ---------------------\n");
    printf("# |        ----*        |\n");
    printf("# | --* METEOR-VISU --* |\n");
    printf("# |  -------*           |\n");
    printf("#  ---------------------\n");
    printf("#\n");
    printf("# Parameters:\n");
    printf("# -----------\n");
    printf("#  * input-tracks  = %s\n", src_path);
    printf("#  * input-video   = %s\n", src_path_video);
    printf("#  * output-video  = %s\n", dest_path_video);
    printf("#  * output-frames = %s\n", dest_path_frames);
    printf("#  * validation    = %s\n", validation);
    printf("#\n");

    int b = 1;
    int i0, i1, j0, j1;
	int color = 0;
    int frame, frame_bb;
    int rx, ry, bb_x, bb_y;
    int start = 0; 
    int end = 100000;

    if (!src_path || !src_path_video){
        printf("(EE) Input(s) missing\n");
        exit(1);
    }

    if (!dest_path_video){
        printf("(EE) output missing\n");
        exit(1);
    }

    if(!dest_path_frames){
        printf("(II) Output missing -> no frames will be saved\n");
    }

    // char *path_bounding_box;
    disp(src_path_video);
    get_bouding_box_path_from_tracks_path(src_path);
    disp(path_bounding_box);

    Track tracks[SIZE_MAX_TRACKS];

    int nb_tracks = 0;
	init_Track(tracks, SIZE_MAX_TRACKS);
        
    // debug/output paths and files
    if(dest_path_frames)
	   create_video_dir (dest_path_frames);
    create_videos_files(dest_path_video);

    if (dest_path_frames)
	   create_frames_dir(dest_path_frames);

    disp(src_path);
    disp(path_bounding_box);
    
    // recupere les tracks
    parseTracks(src_path, tracks, &nb_tracks);
    printTracks(tracks, nb_tracks);
    
    // init 
    Video* video = Video_init_from_file(src_path_video, start, end, 0, &i0, &i1, &j0, &j1);
    uint8 **I0   = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    
    // validation pour établir si une track est vrai/faux positif
    if (validation) {
        disp(validation);
        Validation_init(validation);
        Validation(tracks, nb_tracks);
    }
    else {
        PUTS("NO VALIDATION");
    }

    // open BB pour l'affichage des rectangles englobants
    FILE * file_bb = fopen(path_bounding_box, "r"); 
    if (file_bb == NULL) {
        fprintf(stderr, "(EE) cannot open file yo\n");
        return;
    }

    // calculs des BB (bounding box) des tracks 
    calc_BB(tracks, nb_tracks, i0, i1, j0, j1);
    disp(path_bounding_box);
    // parcours des BB à afficher
    char lines[1000];
    fgets(lines, 100, file_bb);
    sscanf(lines, "%d %d %d %d %d", &frame_bb, &rx, &ry, &bb_x, &bb_y);
    printf("%d %d %d %d %d \n", frame_bb, rx, ry, bb_x, bb_y);

    // parcours de la video
    while(Video_nextFrame(video,I0)) {
        frame = video->frame_current - 1;
		printf("[Frame] n°%-4d\r", frame);
        fflush(stdout);
        int cpt = 0;

        // affiche tous les BB de l'image
        while(frame_bb == frame){
            // cherche la piste correspondante
            for (int i = 0; i <= nb_tracks ; i++){
                if (tracks[i].timestamp <= frame  && frame <= tracks[i].timestamp+tracks[i].time &&
                    tracks[i].xmin <= bb_x  && bb_x <= tracks[i].xmax  &&
                    tracks[i].ymin <= bb_y  && bb_y <= tracks[i].ymax){

                        if(validation)
                            color = tracks[i].is_valid ? GREEN : RED;
                        else{
                            // color = ORANGE;
                            color = GREEN;

                            if(tracks[i].is_meteor == 1)
                                color = YELLOW;
                        }
                        
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
            // printf("%d %d %d %d %d \n", frame_bb, rx, ry, bb_x, bb_y);
        }
        if (dest_path_frames)
        {
            create_frames_files(frame);
            saveFrame_listBB(path_frames_output, I0, cpt, i0, i1, j0, j1);
        }
        saveVideoFrame_listBB(path_video_tracking, I0, cpt, i0, i1, j0, j1);
    }
    free_ui8matrix(I0, i0-b, i1+b, j0-b, j1+b);
}

// ==============================================================================================================================
void test_max(int argc, char** argv)
// ==============================================================================================================================
{

    if (find_arg(argc, argv, "-h")) {
        fprintf(stderr, "usage: %s %s [options] <input_file>\n", argv[0], argv[1]);
        fprintf(stderr, "  --input-video Path vers le fichier avec les tracks\n");
        fprintf(stderr, "  --input-video Path vers le fichier avec les tracks\n");
        fprintf(stderr, "  --validation0 Fichier contenant la vérité terrain de la séquence\n");
        fprintf(stderr, "  --validation1 Fichier contenant la 2eme vérité terrain de la séquence\n");
        fprintf(stderr, "  --start-frame Image de départ dans la séquence\n");
        fprintf(stderr, "  --end-frame   Dernière image de la séquence\n");
        exit(1);
    }

    // Parsing Arguments
    char* src_path       = find_char_arg (argc, argv, "--input-tracks", NULL);
    char* src_path_video = find_char_arg (argc, argv, "--input-video",  NULL);
    char* validation0    = find_char_arg (argc, argv, "--validation0",  NULL);
    char* validation1    = find_char_arg (argc, argv, "--validation1",  NULL);

    int b = 1;
    int i0 = 0, i1 = 1200, j0 = 0, j1 = 1900;
    int nb_tracks = 0;
    int color;

    if (!src_path || !src_path_video || !validation0){
        printf("Input(s) missing\n");
        exit(1);
    }

    char *path;
    char *filename;
    disp(src_path_video);
    split_path_file(&path, &filename, src_path_video);
    disp(filename);
    
    Track tracks0[SIZE_MAX_TRACKS];
    Track tracks1[SIZE_MAX_TRACKS];

	init_Track(tracks0, SIZE_MAX_TRACKS);
	init_Track(tracks1, SIZE_MAX_TRACKS);
        
    // recupere les tracks
    parseTracks(src_path, tracks0, &nb_tracks);
    parseTracks(src_path, tracks1, &nb_tracks);

    uint8 **I0    = ui8matrix(i0-b, i1+b, j0-b, j1+b);
    MLoadPGM_ui8matrix("max.pgm", i0, i1, j0, j1, I0);

    // calculs des BB (bounding box) des tracks 
    calc_BB(tracks0, nb_tracks, i0, i1, j0, j1);
    calc_BB(tracks1, nb_tracks, i0, i1, j0, j1);

    static ffmpeg_handle writer;
    if (writer.pipe == NULL) {
      ffmpeg_init(&writer);
      writer.input.width = j1 - j0 + 1;
      writer.input.height = i1 - i0 + 1;
      writer.input.pixfmt = ffmpeg_str2pixfmt("rgb24");
      if (!ffmpeg_start_writer(&writer, "out.png", NULL)) return;
    }
    rgb8** img = rgb8matrix(0, i1, 0, j1);
    for (int i=i0 ; i<=i1 ; i++) {
        for (int j=j0 ; j<=j1 ; j++) {
            img[i][j].r = I0[i][j];
            img[i][j].g = I0[i][j];
            img[i][j].b = I0[i][j];
        }
    }

    // validation pour établir si une track est vrai/faux positif
    if (validation0) {
        Validation_init(validation0);
        Validation(tracks0, nb_tracks);
    }
    else {
        PUTS("NO VALIDATION");
    }
    if (validation1) {
        Validation_init(validation1);
        Validation(tracks1, nb_tracks);
    }
    else {
        PUTS("NO VALIDATION");
    }


    for (int i = 0; i <= nb_tracks ; i++){
        printf("i = %d\n", i);
        if (tracks0[i].is_valid == 1){
            if (tracks1[i].is_valid == 1){
                color = GREEN;
            }
            else 
                color = BLUE;
        }
        else 
            color = RED;
        plot_bouding_box(img, tracks0[i].ymin+5, tracks0[i].ymax-5, tracks0[i].xmin+5, tracks0[i].xmax-5, 2, get_color(color));
    }
    ffmpeg_write2d(&writer, (uint8_t**)img);
    free_rgb8matrix(img, 0, i1, 0, j1);
    free_ui8matrix(I0, i0-b, i1+b, j0-b, j1+b);
}

int main(int argc, char** argv)
{
    main_visu(argc, argv);
    // test_max(argc, argv);
    
    return 0;
}
