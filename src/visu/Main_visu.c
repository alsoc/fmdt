/*
 * Copyright (c) 2022, Clara Ciocan/ Mathuran Kandeepan
 * LIP6
 */ 

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <nrc2.h>
#ifdef OPENCV_LINK
#include <iostream>
#include <vector>
#include <tuple>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#endif

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
#define MISC   6

/*DEBUG*/
extern char path_video_tracking[200];
extern char path_bounding_box[200];
// extern char path_tracks[200];
extern char path_frames_output[250];

typedef struct coordBB {
    int track_id;
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
    rgb8 green;     rgb8 red;     rgb8 blue;     rgb8 orange;     rgb8 yellow;     rgb8 misc;
    green.g = 255;  red.g = 000;  blue.g = 000;  orange.r = 255;  yellow.g = 255;  misc.g = 255;
    green.b = 000;  red.b = 000;  blue.b = 255;  orange.g = 165;  yellow.b = 000;  misc.b = 153;
    green.r = 000;  red.r = 255;  blue.r = 000;  orange.b = 000;  yellow.r = 255;  misc.r = 153;

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

#ifdef OPENCV_LINK
void plot_bounding_box_id(cv::Mat cv_img, const int y, const int x, const rgb8 color, const char* txt) {
    // Writing over the Image
    cv::Point org(x, y);
    cv::putText(cv_img, txt, org,
                cv::FONT_HERSHEY_DUPLEX, 0.7,
                cv::Scalar(color.b, color.g, color.r), 1, cv::LINE_AA);
}
#endif

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
        plot_bounding_box(img, ymin,ymax,xmin,xmax, 2, get_color(listBB[i].color));
    }

#ifdef OPENCV_LINK
    // Create a blank image of size
    // (500 x 500) with white background
    // (B, G, R) : (255, 255, 255)
    cv::Mat cv_img(i1, j1, CV_8UC3, cv::Scalar(255, 255, 255));

    // Check if the image is created
    // successfully.
    if (!cv_img.data) {
        std::cerr << "(EE) Could not open or find the image" << std::endl;
        exit(-1);
    }

    for (int i = 0; i < i1; i++) {
        for (int j = 0; j < j1; j++) {
            cv_img.at<cv::Vec3b>(i,j)[2] = img[i][j].r;
            cv_img.at<cv::Vec3b>(i,j)[1] = img[i][j].g;
            cv_img.at<cv::Vec3b>(i,j)[0] = img[i][j].b;
        }
    }

    std::vector<std::tuple<int, int, rgb8, std::vector<int>>> list_of_ids;

    for(int i = 0; i < cpt; i++){
        int ymin = clamp(listBB[i].bb_y - listBB[i].ry, 1, i1-1);
        int ymax = clamp(listBB[i].bb_y + listBB[i].ry, 1, i1-1);
        int xmax = clamp(listBB[i].bb_x + listBB[i].rx, 1, j1-1);

        int x = xmax+3;
        int y = (ymin)+((ymax-ymin)/2);

        bool found = false;
        for (auto &l : list_of_ids) {
            rgb8 c = get_color(listBB[i].color);
            if (std::get<0>(l) == x && std::get<1>(l) == y &&
                std::get<2>(l).r == c.r && std::get<2>(l).g == c.g && std::get<2>(l).b == c.b) {
                std::get<3>(l).push_back(listBB[i].track_id);
                found = true;
            }
        }

        if (!found) {
            std::vector<int> v;
            v.push_back(listBB[i].track_id);
            list_of_ids.push_back(std::make_tuple(x, y, get_color(listBB[i].color), v));
        }
    }

    for (auto id : list_of_ids) {
        std::string txt = std::to_string(std::get<3>(id)[std::get<3>(id).size() -1]);
        for (int s = std::get<3>(id).size() -2; s >= 0; s--)
            txt += "," + std::to_string(std::get<3>(id)[s]);
        plot_bounding_box_id(cv_img, std::get<1>(id), std::get<0>(id), std::get<2>(id), txt.c_str());
    }

    // Show our image inside a window.
    // cv::imshow("Output", cv_img);
    // cv::waitKey(0);

    for (int i = 0; i < i1; i++) {
        for (int j = 0; j < j1; j++) {
            img[i][j].r = cv_img.at<cv::Vec3b>(i,j)[2];
            img[i][j].g = cv_img.at<cv::Vec3b>(i,j)[1];
            img[i][j].b = cv_img.at<cv::Vec3b>(i,j)[0];
        }
    }
#endif

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
            plot_bounding_box(img, ymin,ymax,xmin,xmax, 2, get_color(listBB[i].color));
    }

    file = fopen(filename, "wb");
    if (file == NULL){
      char message[256] = "ouverture du fichier %s impossible dans saveFrame_listBB\n";
      nrerror(message);
    }

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
void addToListBB(int rx, int ry, int bb_x, int bb_y, int track_id, int color, int i)
// ==============================================================================================================================
{
        (listBB + i)->track_id = track_id;
        (listBB + i)->bb_x     = bb_x;
        (listBB + i)->bb_y     = bb_y;
        (listBB + i)->rx       = rx;
        (listBB + i)->ry       = ry;
        (listBB + i)->color    = color;
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
    char* def_input_tracks      =             NULL;
    char* def_input_bb          =             NULL;
    char* def_input_video       =             NULL;
    char  def_output_video[256] = "./out_visu.mp4";
    char* def_output_frames     =             NULL;
    char* def_validation        =             NULL;

    if (find_arg(argc, argv, "-h")) {
        fprintf(stderr, "  --input-tracks     Path vers le fichier avec les tracks                [%s]\n", def_input_tracks );
        fprintf(stderr, "  --input-bb         Path vers le fichier avec les bounding boxes        [%s]\n", def_input_bb     );
        fprintf(stderr, "  --input-video      Path vers la video                                  [%s]\n", def_input_video  );
        fprintf(stderr, "  --output-video     Output de la video (MPEG-4 format)                  [%s]\n", def_output_video );
        fprintf(stderr, "  --output-frames    Path to the frames output                           [%s]\n", def_output_frames);
        fprintf(stderr, "  --validation       Fichier contenant la vérité terrain de la séquence  [%s]\n", def_validation   );
        fprintf(stderr, "  -h                 This help                                               \n"                   );
        exit(1);
    }

    // Parsing Arguments
    char *src_path         = find_char_arg (argc, argv, "--input-tracks",  def_input_tracks);
    char *input_bb         = find_char_arg (argc, argv, "--input-bb",      def_input_bb);
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
    printf("#  * input-bb      = %s\n", input_bb);
    printf("#  * input-video   = %s\n", src_path_video);
    printf("#  * output-video  = %s\n", dest_path_video);
    printf("#  * output-frames = %s\n", dest_path_frames);
    printf("#  * validation    = %s\n", validation);
    printf("#\n");

    int b = 1;
    int i0, i1, j0, j1;
	int color = 0;
    int frame, frame_bb;
    int rx, ry, bb_x, bb_y, track_id;
    int start = 0; 
    int end = 100000;

    if (!src_path_video){
        fprintf(stderr, "(EE) '--input-video' is missing\n");
        exit(1);
    }

    if (!src_path){
        fprintf(stderr, "(EE) '--input-tracks' is missing\n");
        exit(1);
    }

    if (!input_bb){
        fprintf(stderr, "(EE) '--input-bb' is missing\n");
        exit(1);
    }

    if (!dest_path_video){
        fprintf(stderr, "(EE) '--output-video' is missing\n");
        exit(1);
    }

    if(!dest_path_frames){
        fprintf(stderr, "(II) '--output-frames' is missing -> no frames will be saved\n");
    }

    // char *path_bounding_box;
    disp(src_path_video);
    sprintf(path_bounding_box, "%s", input_bb);
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
    //printTracks2(tracks, nb_tracks, 1);


    int max_LUT = 0;
    for (int i = 0; i < nb_tracks; i++)
        if (tracks[i].id > max_LUT)
            max_LUT = tracks[i].id;
    int* LUT_tracks_id = (int*)malloc(sizeof(int) * (max_LUT +1));
    memset(LUT_tracks_id, -1, max_LUT +1);
    for (int i = 0; i < nb_tracks; i++)
        LUT_tracks_id[tracks[i].id] = i;

    unsigned n_tracks = 0, n_stars = 0, n_meteors = 0, n_noise = 0;
    n_tracks = track_count_objects(tracks, nb_tracks, &n_stars, &n_meteors, &n_noise);
    printf("# Tracks read from file = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3d]\n", n_meteors, n_stars, n_noise, n_tracks);
    
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

    disp(path_bounding_box);
    // parcours des BB à afficher
    char lines[1000];
    fgets(lines, 100, file_bb);
    sscanf(lines, "%d %d %d %d %d %d ", &frame_bb, &rx, &ry, &bb_x, &bb_y, &track_id);
    printf("# The program is running...\n");
    // parcours de la video
    while(Video_nextFrame(video,I0)) {
        frame = video->frame_current - 1;
		fprintf(stderr, "(II) Frame n°%-4d\r", frame);
        fflush(stderr);
        int cpt = 0;

        // affiche tous les BB de l'image
        while(frame_bb == frame){
            switch(tracks[LUT_tracks_id[track_id]].obj_type){
                case STAR:
                    color = YELLOW;
                    break;
                case METEOR:
                    color = GREEN;
                    break;
                case NOISE:
                    color = ORANGE;
                    break;
                default:
                    fprintf(stderr, "(EE) This should never happen... ('cpt' = %d, 'track_id' = %d, 'LUT_tracks_id[track_id]' = %d, 'tracks[LUT_tracks_id[track_id]].obj_type' = %d)\n", cpt, track_id, LUT_tracks_id[track_id], tracks[LUT_tracks_id[track_id]].obj_type);
                    exit(-1);
                    break;
            }

            if (validation && tracks[LUT_tracks_id[track_id]].is_valid == 1) // GREEN = true positive 'meteor'
                color = GREEN;
            if (validation && tracks[LUT_tracks_id[track_id]].is_valid == 2) // RED = false positive 'meteor'
                color = RED;

            addToListBB(rx, ry, bb_x, bb_y, track_id, color, cpt++);

            if(fgets(lines, 100, file_bb)==NULL){
                frame_bb = -1;
                break;
            }
            // cherche prochain BB à afficher
            sscanf(lines, "%d %d %d %d %d %d ", &frame_bb, &rx, &ry, &bb_x, &bb_y, &track_id);
        }
        if (dest_path_frames)
        {
            create_frames_files(frame);
            saveFrame_listBB(path_frames_output, I0, cpt, i0, i1, j0, j1);
        }
        saveVideoFrame_listBB(path_video_tracking, I0, cpt, i0, i1, j0, j1);
    }
    free_ui8matrix(I0, i0-b, i1+b, j0-b, j1+b);
    free(LUT_tracks_id);

    printf("# The video has been written.\n");
    printf("# End of the program, exiting.\n");
}

// ==============================================================================================================================
void test_max(int argc, char** argv)
// ==============================================================================================================================
{

    if (find_arg(argc, argv, "-h")) {
        fprintf(stderr, "usage: %s %s [options] <input_file>\n", argv[0], argv[1]);
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
    char max_filename[128] = "max.pgm";
    MLoadPGM_ui8matrix(max_filename, i0, i1, j0, j1, I0);

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
        plot_bounding_box(img, tracks0[i].ymin+5, tracks0[i].ymax-5, tracks0[i].xmin+5, tracks0[i].xmax-5, 2, get_color(color));
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
