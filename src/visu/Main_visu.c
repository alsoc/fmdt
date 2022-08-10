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
#include "tools_visu.h"

extern char path_video_tracking[200];
extern char path_bounding_box[200];
extern char path_frames_output[250];

coordBB listBB[200];

// ==============================================================================================================================
void addToListBB(coordBB *coord, int rx, int ry, int bb_x, int bb_y, int track_id, enum Color_t color)
// ==============================================================================================================================
{
    coord->track_id = track_id;
    coord->ymin     = bb_y - ry;
    coord->ymax     = bb_y + ry;
    coord->xmin     = bb_x - rx;
    coord->xmax     = bb_x + rx;
    coord->color    = color;
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
#ifdef OPENCV_LINK
        fprintf(stderr, "  --show-ids         Show the object ids on the output video and frames      \n"                   );
        fprintf(stderr, "  --natural-num     Natural numbering of the object ids                      \n"                   );
#endif
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
#ifdef OPENCV_LINK
    int   show_ids         = find_arg      (argc, argv, "--show-ids");
    int   natural_num      = find_arg      (argc, argv, "--natural-num");
#endif

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
#ifdef OPENCV_LINK
    printf("#  * show-ids      = %d\n", show_ids);
    printf("#  * natural-num   = %d\n", natural_num);
#endif
    printf("#  * validation    = %s\n", validation);
    printf("#\n");

    int b = 1;
    int i0, i1, j0, j1;
	enum Color_t color = MISC;
    int frame, frame_bb;
    int rx, ry, bb_x, bb_y, track_id;
    int start = 0;
    int end = 100000;

    if (!src_path_video){
        fprintf(stderr, "(EE) '--input-video' is missing\n"); exit(1);
    }
    if (!src_path){
        fprintf(stderr, "(EE) '--input-tracks' is missing\n"); exit(1);
    }
    if (!input_bb){
        fprintf(stderr, "(EE) '--input-bb' is missing\n"); exit(1);
    }
    if (!dest_path_video){
        fprintf(stderr, "(EE) '--output-video' is missing\n"); exit(1);
    }
    if(!dest_path_frames)
        fprintf(stderr, "(II) '--output-frames' is missing -> no frames will be saved\n");
#ifdef OPENCV_LINK
    if (!show_ids && natural_num)
        fprintf(stderr, "(WW) '--natural-num' will not work because '--show-ids' is not set.\n");
#endif

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
        fprintf(stderr, "(EE) cannot open file '%s'\n", path_bounding_box);
        return;
    }

    disp(path_bounding_box);
    // parcours des BB à afficher
    char lines[1000];
    fgets(lines, 100, file_bb);
    sscanf(lines, "%d %d %d %d %d %d ", &frame_bb, &rx, &ry, &bb_x, &bb_y, &track_id);
    printf("# The program is running...\n");

    ffmpeg_handle writer_video_out;
    ffmpeg_init(&writer_video_out);
    writer_video_out.input.width = j1 - j0 + 1;
    writer_video_out.input.height = i1 - i0 + 1;
    writer_video_out.input.pixfmt = ffmpeg_str2pixfmt("rgb24");
    if (!ffmpeg_start_writer(&writer_video_out, path_video_tracking, NULL)) {
        fprintf(stderr, "(EE) Something went wrong when starting to write output video.");
        exit(-1);
    }
    rgb8** img_bb = rgb8matrix(0, i1, 0, j1);

    // parcours de la video
    while(Video_nextFrame(video,I0)) {
        frame = video->frame_current - 1;
		fprintf(stderr, "(II) Frame n°%-4d\r", frame);
        fflush(stderr);
        int cpt = 0;

        // affiche tous les BB de l'image
        while(frame_bb == frame){
            switch(tracks[LUT_tracks_id[track_id]].obj_type){
                case STAR:   color = YELLOW; break;
                case METEOR: color = GREEN;  break;
                case NOISE:  color = ORANGE; break;
                default:
                    fprintf(stderr, "(EE) This should never happen... ('cpt' = %d, 'track_id' = %d, 'LUT_tracks_id[track_id]' = %d, 'tracks[LUT_tracks_id[track_id]].obj_type' = %d)\n", cpt, track_id, LUT_tracks_id[track_id], tracks[LUT_tracks_id[track_id]].obj_type);
                    exit(-1);
                    break;
            }

            if (validation && tracks[LUT_tracks_id[track_id]].is_valid == 1) color = GREEN; // GREEN = true  positive 'meteor'
            if (validation && tracks[LUT_tracks_id[track_id]].is_valid == 2) color = RED;   // RED   = false positive 'meteor'

#ifdef OPENCV_LINK
            int display_track_id = natural_num ? (LUT_tracks_id[track_id] +1) : track_id;
#else
            int display_track_id = track_id;
#endif
            addToListBB(listBB+cpt, rx, ry, bb_x, bb_y, display_track_id, color);

            if(fgets(lines, 100, file_bb)==NULL){
                frame_bb = -1;
                break;
            }
            // cherche prochain BB à afficher
            sscanf(lines, "%d %d %d %d %d %d ", &frame_bb, &rx, &ry, &bb_x, &bb_y, &track_id);

            cpt++;
        }

        convert_img_grayscale_to_rgb((const uint8**)I0, img_bb, i0, i1, j0, j1);
        draw_BB(img_bb, listBB, cpt);
#ifdef OPENCV_LINK
        if (show_ids)
            draw_track_ids(img_bb, j1, i1, listBB, cpt);
#endif
        if (!ffmpeg_write2d(&writer_video_out, (uint8_t**)img_bb)) {
            fprintf(stderr, "(EE) ffmpeg_write2d: %s, frame: %d\n", ffmpeg_error2str(writer_video_out.error), frame);
            exit(-1);
        }
        if (dest_path_frames) {
            create_frames_files(frame);
            saveFrame(path_frames_output, (const rgb8**)img_bb, j1, i1);
        }
    }
    free_rgb8matrix(img_bb, 0, i1, 0, j1);
    ffmpeg_stop_writer(&writer_video_out);
    free_ui8matrix(I0, i0-b, i1+b, j0-b, j1+b);
    free(LUT_tracks_id);

    printf("# The video has been written.\n");
    printf("# End of the program, exiting.\n");
}

int main(int argc, char** argv)
{
    main_visu(argc, argv);
    return 0;
}
