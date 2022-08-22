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

int main(int argc, char** argv) {
    // default values
    char* def_in_video = NULL;
    char* def_in_tracks = NULL;
    char* def_out_frame = NULL;
    int def_fra_start = 0;
    int def_fra_end = 200000;
    char* def_validation = NULL;

    if (args_find(argc, argv, "-h")) {
        fprintf(stderr, "  --in-video     Video source                             [%s]\n", def_in_video);
        fprintf(stderr, "  --in-tracks    Path to the tracks files                 [%s]\n", def_in_tracks);
        fprintf(stderr, "  --out-frame    Path to the frame output file            [%s]\n", def_out_frame);
        fprintf(stderr, "  --fra-start    Starting frame in the video              [%d]\n", def_fra_start);
        fprintf(stderr, "  --fra-end      Ending frame in the video                [%d]\n", def_fra_end);
#ifdef OPENCV_LINK
        fprintf(stderr, "  --show-id      Show the object ids on the output frame      \n");
        fprintf(stderr, "  --nat-num      Natural numbering of the object ids          \n");
#endif
        fprintf(stderr, "  --in-gt        File containing the ground truth         [%s]\n", def_validation);
        fprintf(stderr, "  -h             This help                                    \n");
        exit(1);
    }

    // Parsing Arguments
    char* in_video = args_find_char(argc, argv, "--in-video", def_in_video);
    char* in_tracks = args_find_char(argc, argv, "--in-tracks", def_in_tracks);
    char* out_frame = args_find_char(argc, argv, "--out-frame", def_out_frame);
    int fra_start = args_find_int(argc, argv, "--fra-start", def_fra_start);
    int fra_end = args_find_int(argc, argv, "--fra-end", def_fra_end);
    char* in_gt = args_find_char(argc, argv, "--in-gt", def_validation);
#ifdef OPENCV_LINK
    int show_id = args_find(argc, argv, "--show-id");
    int nat_num = args_find(argc, argv, "--nat-num");
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
    printf("#  * in-video  = %s\n", in_video);
    printf("#  * in-tracks = %s\n", in_tracks);
    printf("#  * out-frame = %s\n", out_frame);
    printf("#  * fra-start = %d\n", fra_start);
    printf("#  * fra-end   = %d\n", fra_end);
#ifdef OPENCV_LINK
    printf("#  * show-id   = %d\n", show_id);
    printf("#  * nat-num   = %d\n", nat_num);
#endif
    printf("#  * in-gt     = %s\n", in_gt);
    printf("#\n");

    if (!in_video) {
        fprintf(stderr, "(EE) '--in-video' is missing\n");
        exit(1);
    }
    if (!out_frame) {
        fprintf(stderr, "(EE) '--out-frame' is missing\n");
        exit(1);
    }
#ifdef OPENCV_LINK
    if (show_id && !in_tracks)
        fprintf(stderr, "(WW) '--show-id' will not work because '--in-tracks' is not set.\n");
    if (!show_id && nat_num)
        fprintf(stderr, "(WW) '--nat-num' will not work because '--show-id' is not set.\n");
#endif
    if (in_gt && !in_tracks)
        fprintf(stderr, "(WW) '--in-gt' will not work because '--in-tracks' is not set.\n");

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
    video_t* video = video_init_from_file(in_video, fra_start, fra_end, skip, &i0, &i1, &j0, &j1);

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

    if (in_tracks) {
        track_t tracks[MAX_TRACKS_SIZE];
        int n_tracks = 0;
        tracking_init_tracks(tracks, MAX_TRACKS_SIZE);
        tracking_parse_tracks(in_tracks, tracks, &n_tracks);

        if (in_gt) {
            validation_init(in_gt);
            validation_process(tracks, n_tracks);
        }

        BB_coord_t* listBB = (BB_coord_t*)malloc(sizeof(BB_coord_t) * n_tracks);
        for (int t = 0; t < n_tracks; t++) {
#ifdef OPENCV_LINK
            listBB[t].track_id = nat_num ? (t + 1) : tracks[t].id;
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

            if (in_gt && tracks[t].is_valid == 1)
                listBB[t].color = GREEN; // GREEN = true  positive 'meteor'
            if (in_gt && tracks[t].is_valid == 2)
                listBB[t].color = RED; // RED   = false positive 'meteor'
        }

        rgb8_t** img_bb = (rgb8_t**)rgb8matrix(i0, i1, j0, j1);
        tools_convert_img_grayscale_to_rgb((const uint8_t**)Max, img_bb, i0, i1, j0, j1);
        int n_BB = n_tracks;
        tools_draw_BB(img_bb, listBB, n_BB, j1, i1);
#ifdef OPENCV_LINK
        tools_draw_text(img_bb, j1, i1, listBB, n_BB, in_gt ? 1 : 0, show_id);
#endif
        tools_save_frame(out_frame, (const rgb8_t**)img_bb, j1, i1);

        free(listBB);
        free(img_bb);
    } else {
        SavePGM_ui8matrix(Max, i0, i1, j0, j1, out_frame);
    }

    // ----------
    // -- free --
    // ----------
    free_ui8matrix(img, i0, i1, j0, j1);
    free_ui8matrix(Max, i0, i1, j0, j1);
    video_free(video);

    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
