/*
 * Copyright (c) 2022, Clara Ciocan/ Mathuran Kandeepan
 * LIP6
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <nrc2.h>
#include <ffmpeg-io/writer.h>

#include "macros.h"
#include "args.h"
#include "defines.h"
#include "tools.h"
#include "tracking.h"
#include "validation.h"
#include "video.h"

#define DELTA_BB 5 // extra pixel size for bounding boxes

void add_to_BB_coord_list(BB_coord_t* coord, int rx, int ry, int bb_x, int bb_y, int track_id, enum color_e color) {
    coord->track_id = track_id;
    coord->ymin = bb_y - (ry + DELTA_BB);
    coord->ymax = bb_y + (ry + DELTA_BB);
    coord->xmin = bb_x - (rx + DELTA_BB);
    coord->xmax = bb_x + (rx + DELTA_BB);
    coord->color = color;
}

int main(int argc, char** argv) {
    // default values
    char* def_p_in_tracks = NULL;
    char* def_p_in_bb = NULL;
    char* def_p_in_video = NULL;
    char def_p_out_video[256] = "./out_visu.mp4";
    char* def_p_out_frames = NULL;
    char* def_p_in_gt = NULL;

    if (args_find(argc, argv, "-h")) {
        fprintf(stderr, "  --in-tracks      Path to the tracks file                             [%s]\n",
                def_p_in_tracks);
        fprintf(stderr, "  --in-bb          Path the bounding boxes file                        [%s]\n", def_p_in_bb);
        fprintf(stderr, "  --in-video       Path to the inpute video file                       [%s]\n",
                def_p_in_video);
        fprintf(stderr, "  --in-gt          Path to ground truth file                           [%s]\n", def_p_in_gt);
        fprintf(stderr, "  --out-video      Path to the output video file (MPEG-4 format)       [%s]\n",
                def_p_out_video);
        fprintf(stderr, "  --out-frames     Path to the frames output folder                    [%s]\n",
                def_p_out_frames);
#ifdef OPENCV_LINK
        fprintf(stderr, "  --show-id        Show the object ids on the output video and frames      \n");
        fprintf(stderr, "  --nat-num        Natural numbering of the object ids                     \n");
#endif
        fprintf(stderr, "  --only-meteor    Show only meteors                                       \n");
        fprintf(stderr, "  -h               This help                                               \n");
        exit(1);
    }

    // Parsing Arguments
    const char* p_in_tracks = args_find_char(argc, argv, "--in-tracks", def_p_in_tracks);
    const char* p_in_bb = args_find_char(argc, argv, "--in-bb", def_p_in_bb);
    const char* p_in_video = args_find_char(argc, argv, "--in-video", def_p_in_video);
    const char* p_out_video = args_find_char(argc, argv, "--out-video", def_p_out_video);
    const char* p_out_frames = args_find_char(argc, argv, "--out-frames", def_p_out_frames);
    const char* p_in_gt = args_find_char(argc, argv, "--in-gt", def_p_in_gt);
#ifdef OPENCV_LINK
    const int show_id = args_find(argc, argv, "--show-id");
    const int nat_num = args_find(argc, argv, "--nat-num");
#endif
    const int only_meteor = args_find(argc, argv, "--only-meteor");

    // heading display
    printf("#  ---------------------\n");
    printf("# |        ----*        |\n");
    printf("# | --* METEOR-VISU --* |\n");
    printf("# |  -------*           |\n");
    printf("#  ---------------------\n");
    printf("#\n");
    printf("# Parameters:\n");
    printf("# -----------\n");
    printf("#  * in-tracks   = %s\n", p_in_tracks);
    printf("#  * in-bb       = %s\n", p_in_bb);
    printf("#  * in-video    = %s\n", p_in_video);
    printf("#  * in-gt       = %s\n", p_in_gt);
    printf("#  * out-video   = %s\n", p_out_video);
    printf("#  * out-frames  = %s\n", p_out_frames);
#ifdef OPENCV_LINK
    printf("#  * show-id     = %d\n", show_id);
    printf("#  * nat-num     = %d\n", nat_num);
#endif
    printf("#  * only-meteor = %d\n", only_meteor);
    printf("#\n");

    int b = 1;
    int i0, i1, j0, j1;
    enum color_e color = MISC;
    int frame, frame_bb;
    int rx, ry, bb_x, bb_y, track_id;
    int start = 0;
    int end = 100000;

    if (!p_in_video) {
        fprintf(stderr, "(EE) '--in-video' is missing\n");
        exit(1);
    }
    if (!p_in_tracks) {
        fprintf(stderr, "(EE) '--in-tracks' is missing\n");
        exit(1);
    }
    if (!p_in_bb) {
        fprintf(stderr, "(EE) '--in-bb' is missing\n");
        exit(1);
    }
    if (!p_out_video) {
        fprintf(stderr, "(EE) '--out-video' is missing\n");
        exit(1);
    }
    if (!p_out_frames)
        fprintf(stderr, "(II) '--out-frames' is missing -> no frames will be saved\n");
#ifdef OPENCV_LINK
    if (!show_id && nat_num)
        fprintf(stderr, "(WW) '--nat-num' will not work because '--show-id' is not set.\n");
#endif

    track_t* tracks = (track_t*)malloc(MAX_TRACKS_SIZE * sizeof(track_t));
    BB_coord_t* BB_list = (BB_coord_t*)malloc(MAX_BB_LIST_SIZE * sizeof(BB_coord_t*));

    int n_tracks = 0;
    tracking_init_global_data();
    tracking_init_tracks(tracks, MAX_TRACKS_SIZE);
    tracking_parse_tracks(p_in_tracks, tracks, &n_tracks);

    int max_LUT = 0;
    for (int i = 0; i < n_tracks; i++)
        if (tracks[i].id > max_LUT)
            max_LUT = tracks[i].id;
    int* LUT_tracks_id = (int*)malloc(sizeof(int) * (max_LUT + 1));
    int* LUT_tracks_nat_num = (int*)malloc(sizeof(int) * (max_LUT + 1));
    memset(LUT_tracks_id, -1, max_LUT + 1);
    memset(LUT_tracks_nat_num, -1, max_LUT + 1);
    int j = 1;
    for (int i = 0; i < n_tracks; i++) {
        LUT_tracks_id[tracks[i].id] = i;
        if (!only_meteor || tracks[i].obj_type == METEOR)
            LUT_tracks_nat_num[tracks[i].id] = j++;

    }

    unsigned n_stars = 0, n_meteors = 0, n_noise = 0;
    if (tracking_count_objects(tracks, n_tracks, &n_stars, &n_meteors, &n_noise) != n_tracks) {
        fprintf(stderr, "(EE) 'tracking_count_objects' returned different number of tracks...\n");
        exit(-1);
    }
    printf("# Tracks read from file = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3d]\n", n_meteors, n_stars,
           n_noise, n_tracks);

    // init
    video_t* video = video_init_from_file(p_in_video, start, end, 0, &i0, &i1, &j0, &j1);
    uint8_t** I0 = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b);

    // validation pour établir si une track est vrai/faux positif
    if (p_in_gt) {
        validation_init(p_in_gt);
        validation_process(tracks, n_tracks);
    } else {
        PUTS("NO VALIDATION");
    }

    // open BB pour l'affichage des rectangles englobants
    FILE* file_bb = fopen(p_in_bb, "r");
    if (file_bb == NULL) {
        fprintf(stderr, "(EE) cannot open file '%s'\n", p_in_bb);
        return EXIT_FAILURE;
    }

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
    if (!ffmpeg_start_writer(&writer_video_out, p_out_video, NULL)) {
        fprintf(stderr, "(EE) Something went wrong when starting to write output video.");
        exit(-1);
    }
    rgb8_t** img_bb = (rgb8_t**)rgb8matrix(0, i1, 0, j1);

    // parcours de la video
    while (video_get_next_frame(video, I0)) {
        frame = video->frame_current - 1;
        fprintf(stderr, "(II) Frame n°%-4d\r", frame);
        fflush(stderr);
        int cpt = 0;

        // affiche tous les BB de l'image
        while (frame_bb == frame) {
            if (!only_meteor || tracks[LUT_tracks_id[track_id]].obj_type == METEOR) {
                if (tracks[LUT_tracks_id[track_id]].obj_type != UNKNOWN)
                    color = g_obj_to_color[tracks[LUT_tracks_id[track_id]].obj_type];
                else {
                    fprintf(stderr,
                            "(EE) This should never happen... ('cpt' = %d, 'track_id' = %d, 'LUT_tracks_id[track_id]' "
                            "= %d, 'tracks[LUT_tracks_id[track_id]].obj_type' = %d)\n",
                            cpt, track_id, LUT_tracks_id[track_id], tracks[LUT_tracks_id[track_id]].obj_type);
                    exit(-1);
                }
                if (p_in_gt && tracks[LUT_tracks_id[track_id]].is_valid == 1)
                    color = GREEN; // GREEN = true  positive 'meteor'
                if (p_in_gt && tracks[LUT_tracks_id[track_id]].is_valid == 2)
                    color = RED; // RED   = false positive 'meteor'

#ifdef OPENCV_LINK
                int display_track_id = nat_num ? LUT_tracks_nat_num[track_id] : track_id;
#else
                int display_track_id = track_id;
#endif
                assert(cpt < MAX_BB_LIST_SIZE);
                add_to_BB_coord_list(BB_list + cpt, rx, ry, bb_x, bb_y, display_track_id, color);
                cpt++;
            }

            if (fgets(lines, 100, file_bb) == NULL) {
                frame_bb = -1;
                break;
            }
            // cherche prochain BB à afficher
            sscanf(lines, "%d %d %d %d %d %d ", &frame_bb, &rx, &ry, &bb_x, &bb_y, &track_id);
        }

        tools_convert_img_grayscale_to_rgb((const uint8_t**)I0, img_bb, i0, i1, j0, j1);
        tools_draw_BB(img_bb, BB_list, cpt, j1, i1);
#ifdef OPENCV_LINK
        tools_draw_text(img_bb, j1, i1, BB_list, cpt, p_in_gt ? 1 : 0, show_id);
#endif
        if (!ffmpeg_write2d(&writer_video_out, (uint8_t**)img_bb)) {
            fprintf(stderr, "(EE) ffmpeg_write2d: %s, frame: %d\n", ffmpeg_error2str(writer_video_out.error), frame);
            exit(-1);
        }
        if (p_out_frames) {
            tools_create_folder(p_out_frames);
            char filename_cur_frame[256];
            sprintf(filename_cur_frame, "%s/%05d.ppm", p_out_frames, frame);
            tools_save_frame(filename_cur_frame, (const rgb8_t**)img_bb, j1, i1);
        }
    }
    free_rgb8matrix((rgb8**)img_bb, 0, i1, 0, j1);
    ffmpeg_stop_writer(&writer_video_out);
    free_ui8matrix(I0, i0 - b, i1 + b, j0 - b, j1 + b);
    free(LUT_tracks_id);
    free(LUT_tracks_nat_num);
    free(tracks);
    free(BB_list);

    printf("# The video has been written.\n");
    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
