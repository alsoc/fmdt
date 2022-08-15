/*
 * Copyright (c) 2022, Clara Ciocan/ Mathuran Kandeepan
 * LIP6
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <nrc2.h>

#include "macros.h"
#include "args.h"
#include "defines.h"
#include "tools.h"
#include "tracking.h"
#include "validation.h"
#include "video.h"

void max_reduce(uint8_t** M, int i0, int i1, int j0, int j1, uint8_t** I) {
    for (int i = i0; i <= i1; i++) {
        for (int j = j0; j <= j1; j++) {
            uint8_t x = I[i][j];
            uint8_t m = M[i][j];
            if (x > m) {
                M[i][j] = x;
            }
        }
    }
}

void main_maxred(int argc, char** argv) {
    // default values
    char* def_input_video = NULL;
    char* def_input_tracks = NULL;
    char* def_output_frame = NULL;
    int def_start_frame = 0;
    int def_end_frame = 200000;
    char* def_validation = NULL;

    if (args_find_arg(argc, argv, "-h")) {
        fprintf(stderr, "  --input-video     Video source                             [%s]\n", def_input_video);
        fprintf(stderr, "  --input-tracks    Path to the tracks files                 [%s]\n", def_input_tracks);
        fprintf(stderr, "  --output-frame    Path to the frames output                [%s]\n", def_output_frame);
        fprintf(stderr, "  --start-frame     Starting frame in the video              [%d]\n", def_start_frame);
        fprintf(stderr, "  --end-frame       Ending frame in the video                [%d]\n", def_end_frame);
#ifdef OPENCV_LINK
        fprintf(stderr, "  --show-ids        Show the object ids on the output frame      \n");
        fprintf(stderr, "  --natural-num     Natural numbering of the object ids          \n");
#endif
        fprintf(stderr, "  --validation      File containing the ground truth         [%s]\n", def_validation);
        fprintf(stderr, "  -h                This help                                    \n");
        exit(1);
    }

    // Parsing Arguments
    char* input_video = args_find_char_arg(argc, argv, "--input-video", def_input_video);
    char* input_tracks = args_find_char_arg(argc, argv, "--input-tracks", def_input_tracks);
    char* output_frame = args_find_char_arg(argc, argv, "--output-frame", def_output_frame);
    int start_frame = args_find_int_arg(argc, argv, "--start-frame", def_start_frame);
    int end_frame = args_find_int_arg(argc, argv, "--end-frame", def_end_frame);
    char* validation = args_find_char_arg(argc, argv, "--validation", def_validation);
#ifdef OPENCV_LINK
    int show_ids = args_find_arg(argc, argv, "--show-ids");
    int natural_num = args_find_arg(argc, argv, "--natural-num");
#endif

    // heading display
    printf("#  -----------------------\n");
    printf("# |         ----*         |\n");
    printf("# | --* METEOR-MAXRED --* |\n");
    printf("# |   -------*            |\n");
    printf("#  -----------------------\n");
    printf("#\n");
    printf("# Parameters:\n");
    printf("# -----------\n");
    printf("#  * input-video  = %s\n", input_video);
    printf("#  * input-tracks = %s\n", input_tracks);
    printf("#  * output-frame = %s\n", output_frame);
    printf("#  * start-frame  = %d\n", start_frame);
    printf("#  * end-frame    = %d\n", end_frame);
#ifdef OPENCV_LINK
    printf("#  * show-ids     = %d\n", show_ids);
    printf("#  * natural-num  = %d\n", natural_num);
#endif
    printf("#  * validation   = %s\n", validation);
    printf("#\n");

    if (!input_video) {
        fprintf(stderr, "(EE) '--input-video' is missing\n");
        exit(1);
    }
    if (!output_frame) {
        fprintf(stderr, "(EE) '--output-frame' is missing\n");
        exit(1);
    }
#ifdef OPENCV_LINK
    if (show_ids && !input_tracks)
        fprintf(stderr, "(WW) '--show-ids' will not work because '--input-tracks' is not set.\n");
    if (!show_ids && natural_num)
        fprintf(stderr, "(WW) '--natural-num' will not work because '--show-ids' is not set.\n");
#endif
    if (validation && !input_tracks)
        fprintf(stderr, "(WW) '--validation' will not work because '--input-tracks' is not set.\n");

    printf("# The program is running...\n");

    tracking_init_global_data();

    // sequence
    int frame;
    int skip = 0;

    // image
    // int b = 1;
    int i0, i1, j0, j1;

    // ------------------------- //
    // -- INITIALISATION VIDEO-- //
    // ------------------------- //
    PUTS("INIT VIDEO");
    video_t* video = video_init_from_file(input_video, start_frame, end_frame, skip, &i0, &i1, &j0, &j1);

    // ---------------- //
    // -- ALLOCATION -- //
    // ---------------- //
    PUTS("ALLOC");
    uint8_t** img = (uint8_t**)ui8matrix(i0, i1, j0, j1);
    uint8_t** Max = (uint8_t**)ui8matrix(i0, i1, j0, j1);

    // ----------------//
    // -- TRAITEMENT --//
    // ----------------//
    PUTS("LOOP");
    while (video_get_next_frame(video, img)) {
        frame = video->frame_current - 1;
        fprintf(stderr, "(II) Frame n°%4d\r", frame);
        max_reduce(Max, i0, i1, j0, j1, img);
    }
    fprintf(stderr, "\n");

    if (input_tracks) {
        track_t tracks[SIZE_MAX_TRACKS];
        int n_tracks = 0;
        tracking_init_tracks(tracks, SIZE_MAX_TRACKS);
        tracking_parse_tracks(input_tracks, tracks, &n_tracks);

        if (validation) {
            validation_init(validation);
            validation_process(tracks, n_tracks);
        }

        BB_coord_t* listBB = (BB_coord_t*)malloc(sizeof(BB_coord_t) * n_tracks);
        for (int t = 0; t < n_tracks; t++) {
#ifdef OPENCV_LINK
            listBB[t].track_id = natural_num ? (t + 1) : tracks[t].id;
#else
            listBB[t].track_id = tracks[t].id;
#endif
            int delta = 5;
            listBB[t].xmin = (tracks[t].begin.x < tracks[t].end.x ? tracks[t].begin.x : tracks[t].end.x) - delta;
            listBB[t].xmax = (tracks[t].begin.x < tracks[t].end.x ? tracks[t].end.x : tracks[t].begin.x) + delta;
            listBB[t].ymin = (tracks[t].begin.y < tracks[t].end.y ? tracks[t].begin.y : tracks[t].end.y) - delta;
            listBB[t].ymax = (tracks[t].begin.y < tracks[t].end.y ? tracks[t].end.y : tracks[t].begin.y) + delta;

            if (tracks[t].obj_type != UNKNOWN)
                listBB[t].color = g_obj_to_color[tracks[t].obj_type];
            else {
                fprintf(stderr, "(EE) This should never happen... ('t' = %d, 'tracks[t].obj_type' = %d)\n", t,
                        tracks[t].obj_type);
                exit(-1);
            }

            if (validation && tracks[t].is_valid == 1)
                listBB[t].color = GREEN; // GREEN = true  positive 'meteor'
            if (validation && tracks[t].is_valid == 2)
                listBB[t].color = RED; // RED   = false positive 'meteor'
        }

        rgb8_t** img_bb = (rgb8_t**)rgb8matrix(i0, i1, j0, j1);
        tools_convert_img_grayscale_to_rgb((const uint8_t**)Max, img_bb, i0, i1, j0, j1);
        int n_BB = n_tracks;
        tools_draw_BB(img_bb, listBB, n_BB);
#ifdef OPENCV_LINK
        tools_draw_text(img_bb, j1, i1, listBB, n_BB, validation ? 1 : 0, show_ids);
#endif
        tools_save_frame(output_frame, (const rgb8_t**)img_bb, j1, i1);

        free(listBB);
        free(img_bb);
    } else {
        SavePGM_ui8matrix(Max, i0, i1, j0, j1, output_frame);
    }

    // ----------
    // -- free --
    // ----------
    free_ui8matrix(img, i0, i1, j0, j1);
    free_ui8matrix(Max, i0, i1, j0, j1);
    video_free(video);

    printf("# End of the program, exiting.\n");
}

int main(int argc, char** argv) {
    main_maxred(argc, argv);
    return 0;
}
