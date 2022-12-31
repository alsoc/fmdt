#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <nrc2.h>

#include "fmdt/macros.h"
#include "fmdt/args.h"
#include "fmdt/tools.h"
#include "fmdt/tracking.h"
#include "fmdt/validation.h"
#include "fmdt/video.h"
#include "vec.h"

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
    char* def_p_in_video = NULL;
    char* def_p_in_tracks = NULL;
    char* def_p_out_frame = NULL;
    int def_p_fra_start = 0;
    int def_p_fra_end = 0;
    char* def_p_in_gt = NULL;
    int def_p_ffmpeg_threads = 0;
#ifdef OPENCV_LINK
    char def_p_img_ext[] = "pgm";
#endif

    // help
    if (args_find(argc, argv, "-h")) {
        fprintf(stderr, "  --in-video        Video source                                        [%s]\n",
                def_p_in_video ? def_p_in_video : "NULL");
        fprintf(stderr, "  --in-tracks       Path to the tracks files                            [%s]\n",
                def_p_in_tracks ? def_p_in_tracks : "NULL");
        fprintf(stderr, "  --in-gt           File containing the ground truth                    [%s]\n",
                def_p_in_gt ? def_p_in_gt : "NULL");
        fprintf(stderr, "  --out-frame       Path to the frame output file                       [%s]\n",
                def_p_out_frame ? def_p_out_frame : "NULL");
        fprintf(stderr, "  --fra-start       Starting frame in the video                         [%d]\n",
                def_p_fra_start);
        fprintf(stderr, "  --fra-end         Ending frame in the video                           [%d]\n",
                def_p_fra_end);
#ifdef OPENCV_LINK
        fprintf(stderr, "  --show-id         Show the object ids on the output frame                 \n");
        fprintf(stderr, "  --nat-num         Natural numbering of the object ids                     \n");
        fprintf(stderr, "  --img-ext         Image extension of saved frame ('jpg', 'png', ...)  [%s]\n",
                def_p_img_ext);
#endif
        fprintf(stderr, "  --only-meteor     Show only meteors                                       \n");
        fprintf(stderr, "  --ffmpeg-threads  Select the number of threads to use to              [%d]\n"
                        "                    decode video input (in ffmpeg)                          \n",
                def_p_ffmpeg_threads);
        fprintf(stderr, "  -h                This help                                               \n");
        exit(1);
    }

    // Parsing Arguments
    const char* p_in_video = args_find_char(argc, argv, "--in-video", def_p_in_video);
    const char* p_in_tracks = args_find_char(argc, argv, "--in-tracks", def_p_in_tracks);
    const char* p_out_frame = args_find_char(argc, argv, "--out-frame", def_p_out_frame);
    const int p_fra_start = args_find_int_min(argc, argv, "--fra-start", def_p_fra_start, 0);
    const int p_fra_end = args_find_int_min(argc, argv, "--fra-end", def_p_fra_end, 0);
    const char* p_in_gt = args_find_char(argc, argv, "--in-gt", def_p_in_gt);
#ifdef OPENCV_LINK
    const int p_show_id = args_find(argc, argv, "--show-id");
    const int p_nat_num = args_find(argc, argv, "--nat-num");
    const char* p_img_ext = args_find_char(argc, argv, "--img-ext", def_p_img_ext);
#else
    const int p_show_id = 0;
    const char p_img_ext[] = "ppm";
#endif
    const int p_only_meteor = args_find(argc, argv, "--only-meteor");
    const int p_ffmpeg_threads = args_find_int_min(argc, argv, "--ffmpeg-threads", def_p_ffmpeg_threads, 0);

    // heading display
    printf("#  ---------------------\n");
    printf("# |         ----*       |\n");
    printf("# | --* FMDT-MAXRED --* |\n");
    printf("# |   -------*          |\n");
    printf("#  ---------------------\n");
    printf("#\n");
    printf("# Parameters:\n");
    printf("# -----------\n");
    printf("#  * in-video       = %s\n", p_in_video);
    printf("#  * in-tracks      = %s\n", p_in_tracks);
    printf("#  * in-gt          = %s\n", p_in_gt);
    printf("#  * out-frame      = %s\n", p_out_frame);
    printf("#  * fra-start      = %d\n", p_fra_start);
    printf("#  * fra-end        = %d\n", p_fra_end);
#ifdef OPENCV_LINK
    printf("#  * show-id        = %d\n", p_show_id);
    printf("#  * nat-num        = %d\n", p_nat_num);
    printf("#  * img-ext        = %s\n", p_img_ext);
#endif
    printf("#  * only-meteor    = %d\n", p_only_meteor);
    printf("#  * ffmpeg-threads = %d\n", p_ffmpeg_threads);
    printf("#\n");

    // arguments checking
    if (!p_in_video) {
        fprintf(stderr, "(EE) '--in-video' is missing\n");
        exit(1);
    }
    if (!p_out_frame) {
        fprintf(stderr, "(EE) '--out-frame' is missing\n");
        exit(1);
    }
    if (p_fra_end && p_fra_end < p_fra_start) {
        fprintf(stderr, "(EE) '--fra-end' has to be higher than '--fra-start'\n");
        exit(1);
    }
#ifdef OPENCV_LINK
    if (p_show_id && !p_in_tracks)
        fprintf(stderr, "(WW) '--show-id' will not work because '--in-tracks' is not set.\n");
    if (!p_show_id && p_nat_num)
        fprintf(stderr, "(WW) '--nat-num' will not work because '--show-id' is not set.\n");
#endif
    if (p_in_gt && !p_in_tracks)
        fprintf(stderr, "(WW) '--in-gt' will not work because '--in-tracks' is not set.\n");
    if (p_ffmpeg_threads < 0) {
        fprintf(stderr, "(EE) '--ffmpeg-threads' has to be bigger or equal to 0\n");
        exit(1);
    }

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
    video_t* video = video_init_from_file(p_in_video, p_fra_start, p_fra_end, skip, 0, p_ffmpeg_threads, &i0, &i1, &j0,
                                          &j1);

    // ---------------- //
    // -- ALLOCATION -- //
    // ---------------- //
    PUTS("ALLOC");
    uint8_t** img = (uint8_t**)ui8matrix(i0, i1, j0, j1);
    uint8_t** Max = (uint8_t**)ui8matrix(i0, i1, j0, j1);
    zero_ui8matrix(Max, i0, i1, j0, j1);
    img_data_t* img_data = NULL;
    if (p_in_tracks) {
        char ext[32];
        snprintf(ext, sizeof(ext), "%s", p_img_ext);
        if (strcmp(ext, "pgm") == 0) {
            snprintf(ext, sizeof(ext), "%s", "ppm");
            fprintf(stderr, "(WW) Image format has been forced from PGM to PPM because the output will be in "
                            "color.\n");
        }
        img_data = tools_color_img_alloc2((j1 - j0) + 1, (i1 - i0) + 1, ext, p_show_id);
    } else {
        char ext[32];
        snprintf(ext, sizeof(ext), "%s", p_img_ext);
        if (strcmp(ext, "ppm") == 0) {
            snprintf(ext, sizeof(ext), "%s", "pgm");
            fprintf(stderr, "(WW) Image format has been forced from PPM to PGM because the output will be in "
                            "grayscale.\n");
        }
        img_data = tools_gray_img_alloc2((j1 - j0) + 1, (i1 - i0) + 1, ext, 0);
    }

    // ----------------//
    // -- TRAITEMENT --//
    // ----------------//
    PUTS("LOOP");
    while ((frame = video_get_next_frame(video, img)) != -1) {
        fprintf(stderr, "(II) Frame n°%4d\r", frame);
        max_reduce(Max, i0, i1, j0, j1, img);
    }
    fprintf(stderr, "\n");

    if (p_in_tracks) {
        vec_track_t track_array;
        tracking_parse_tracks(p_in_tracks, &track_array);

        if (p_in_gt) {
            validation_init(p_in_gt);
            validation_process(track_array);
        }

        size_t n_tracks = vector_size(track_array);
        BB_t* BB_list = (BB_t*)malloc(sizeof(BB_t) * n_tracks);
        enum color_e* BB_list_color = (enum color_e*)malloc(sizeof(enum color_e) * n_tracks);
        rgb8_t** img_bb = (rgb8_t**)rgb8matrix(i0, i1, j0, j1);
        size_t m = 0;
        for (size_t t = 0; t < n_tracks; t++) {
            if (!p_only_meteor || track_array[t].obj_type == METEOR) {
                BB_list[m].frame_id = 0;
#ifdef OPENCV_LINK
                BB_list[m].track_id = p_nat_num ? (m + 1) : track_array[t].id;
#else
                BB_list[m].track_id = track_array[t].id;
#endif
                int xmin =
                    track_array[t].begin.x < track_array[t].end.x ? track_array[t].begin.x : track_array[t].end.x;
                int xmax =
                    track_array[t].begin.x < track_array[t].end.x ? track_array[t].end.x : track_array[t].begin.x;
                int ymin =
                    track_array[t].begin.y < track_array[t].end.y ? track_array[t].begin.y : track_array[t].end.y;
                int ymax =
                    track_array[t].begin.y < track_array[t].end.y ? track_array[t].end.y : track_array[t].begin.y;

                BB_list[m].bb_x = (uint16_t)ceilf((float)(xmin + xmax) / 2.f);
                BB_list[m].bb_y = (uint16_t)ceilf((float)(ymin + ymax) / 2.f);
                BB_list[m].rx = BB_list[m].bb_x - xmin;
                BB_list[m].ry = BB_list[m].bb_y - ymin;
                BB_list[m].is_extrapolated = 0;

                if (track_array[t].obj_type != UNKNOWN)
                    BB_list_color[m] = g_obj_to_color[track_array[t].obj_type];
                else {
                    fprintf(stderr, "(EE) This should never happen... ('t' = %lu, 'track_array[t].obj_type' = %d)\n",
                            (unsigned long)t, track_array[t].obj_type);
                    exit(-1);
                }

                if (p_in_gt && g_is_valid_track[t] == 1)
                    BB_list_color[m] = GREEN; // GREEN = true positive 'meteor'
                if (p_in_gt && g_is_valid_track[t] == 2)
                    BB_list_color[m] = RED; // RED = false positive 'meteor'
                m++;
            }
        }

        int n_BB = m;
        tools_color_img_draw_BB(img_data, (const uint8_t**)Max, (const BB_t*)BB_list,
                                (const enum color_e*)BB_list_color, n_BB, p_in_gt ? 1 : 0);
        tools_color_img_write2(img_data, p_out_frame);

        vector_free(track_array);

        free_rgb8matrix((rgb8**)img_bb, i0, i1, j0, j1);
        free(BB_list);
        free(BB_list_color);

        if (p_in_gt)
            validation_free();
    } else {
        uint8_t* pixels = tools_gray_img_get_pixels(img_data);
        // these copy could be avoided...
        for (int i = i0; i <= i1; i++)
            for (int j = j0; j <= j1; j++)
                pixels[i * img_data->width + j] = Max[i][j];
        tools_gray_img_write2(img_data, p_out_frame);
    }

    // ----------
    // -- free --
    // ----------
    free_ui8matrix(img, i0, i1, j0, j1);
    free_ui8matrix(Max, i0, i1, j0, j1);
    if (p_in_tracks)
        tools_color_img_free(img_data);
    else
        tools_gray_img_free(img_data);
    video_free(video);

    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
