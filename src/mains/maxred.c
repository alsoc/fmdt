#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <nrc2.h>

#include "fmdt/macros.h"
#include "fmdt/args.h"
#include "fmdt/tools.h"
#include "fmdt/video/video_struct.h"
#include "vec.h"

#include "fmdt/image/image_compute.h"
#include "fmdt/tracking/tracking_global.h"
#include "fmdt/tracking/tracking_io.h"
#include "fmdt/validation.h"
#include "fmdt/video.h"
#include "fmdt/version.h"

void max_reduce(uint8_t** M, int i0, int i1, int j0, int j1, const uint8_t** I) {
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
    char* def_p_vid_in_path = NULL;
    int def_p_vid_in_start = 0;
    int def_p_vid_in_stop = 0;
    int def_p_vid_in_threads = 0;
    char* def_p_trk_path = NULL;
    char* def_p_gt_path = NULL;
    char* def_p_fra_out_path = NULL;

    // help
    if (args_find(argc, argv, "--help,-h")) {
        fprintf(stderr, "  --vid-in-path      Video source                             [%s]\n",
                def_p_vid_in_path ? def_p_vid_in_path : "NULL");
        fprintf(stderr, "  --vid-in-start     Starting frame in the video              [%d]\n",
                def_p_vid_in_start);
        fprintf(stderr, "  --vid-in-stop      Ending frame in the video                [%d]\n",
                def_p_vid_in_stop);
        fprintf(stderr, "  --vid-in-threads   Select the number of threads to use to   [%d]\n"
                        "                     decode video input (in ffmpeg)               \n",
                def_p_vid_in_threads);
        fprintf(stderr, "  --trk-path         Path to the tracks files                 [%s]\n",
                def_p_trk_path ? def_p_trk_path : "NULL");
#ifdef FMDT_OPENCV_LINK
        fprintf(stderr, "  --trk-id           Show the object ids on the output frame      \n");
        fprintf(stderr, "  --trk-nat-num      Natural numbering of the object ids          \n");
#endif
        fprintf(stderr, "  --trk-only-meteor  Show only meteors                            \n");
        fprintf(stderr, "  --gt-path          File containing the ground truth         [%s]\n",
                def_p_gt_path ? def_p_gt_path : "NULL");
        fprintf(stderr, "  --fra-out-path     Path to the frame output file            [%s]\n",
                def_p_fra_out_path ? def_p_fra_out_path : "NULL");
        fprintf(stderr, "  --help, -h         This help                                    \n");
        fprintf(stderr, "  --version, -v      Print the version                            \n");
        exit(1);
    }

    // version
    if (args_find(argc, argv, "--version,-v")) {
        version_print("maxred");
        exit(0);
    }

    // Parsing Arguments
    const char* p_vid_in_path = args_find_char(argc, argv, "--vid-in-path,--in-video", def_p_vid_in_path);
    const int p_vid_in_start = args_find_int_min(argc, argv, "--vid-in-start,--fra-start", def_p_vid_in_start, 0);
    const int p_vid_in_stop = args_find_int_min(argc, argv, "--vid-in-stop,--fra-end", def_p_vid_in_stop, 0);
    const int p_vid_in_threads = args_find_int_min(argc, argv, "--vid-in-threads,--ffmpeg-threads", def_p_vid_in_threads, 0);
    const char* p_trk_path = args_find_char(argc, argv, "--trk-path,--in-tracks", def_p_trk_path);
#ifdef FMDT_OPENCV_LINK
    const int p_trk_id = args_find(argc, argv, "--trk-id,--show-id");
    const int p_trk_nat_num = args_find(argc, argv, "--trk-nat-num,--nat-num");
#else
    const int p_trk_id = 0;
#endif
    const int p_trk_only_meteor = args_find(argc, argv, "--trk-only-meteor,--only-meteor");
    const char* p_gt_path = args_find_char(argc, argv, "--gt-path,--in-gt", def_p_gt_path);
    const char* p_fra_out_path = args_find_char(argc, argv, "--fra-out-path,--out-frame", def_p_fra_out_path);

    // heading display
    printf("#  ---------------------\n");
    printf("# |         ----*       |\n");
    printf("# | --* FMDT-MAXRED --* |\n");
    printf("# |   -------*          |\n");
    printf("#  ---------------------\n");
    printf("#\n");
    printf("# Parameters:\n");
    printf("# -----------\n");
    printf("#  * vid-in-path     = %s\n", p_vid_in_path);
    printf("#  * vid-in-start    = %d\n", p_vid_in_start);
    printf("#  * vid-in-stop     = %d\n", p_vid_in_stop);
    printf("#  * vid-in-threads  = %d\n", p_vid_in_threads);
    printf("#  * trk-path        = %s\n", p_trk_path);
#ifdef FMDT_OPENCV_LINK
    printf("#  * trk-id          = %d\n", p_trk_id);
    printf("#  * trk-nat-num     = %d\n", p_trk_nat_num);
#endif
    printf("#  * trk-only-meteor = %d\n", p_trk_only_meteor);
    printf("#  * gt-path         = %s\n", p_gt_path);
    printf("#  * fra-out-path    = %s\n", p_fra_out_path);
    printf("#\n");

    // arguments checking
    if (!p_vid_in_path) {
        fprintf(stderr, "(EE) '--vid-in-path' is missing\n");
        exit(1);
    }
    if (!p_fra_out_path) {
        fprintf(stderr, "(EE) '--fra-out-path' is missing\n");
        exit(1);
    }
    if (p_vid_in_stop && p_vid_in_stop < p_vid_in_start) {
        fprintf(stderr, "(EE) '--vid-in-stop' has to be higher than '--vid-in-start'\n");
        exit(1);
    }
#ifdef FMDT_OPENCV_LINK
    if (p_trk_id && !p_trk_path)
        fprintf(stderr, "(WW) '--trk-id' will not work because '--trk-path' is not set\n");
    if (!p_trk_id && p_trk_nat_num)
        fprintf(stderr, "(WW) '--trk-nat-num' will not work because '--trk-id' is not set\n");
#endif
    if (p_gt_path && !p_trk_path)
        fprintf(stderr, "(WW) '--gt-path' will not work because '--trk-path' is not set\n");

    printf("# The program is running...\n");

    tracking_init_global_data();

    // ------------------------- //
    // -- INITIALISATION VIDEO-- //
    // ------------------------- //

    PUTS("INIT VIDEO");
    int skip = 0;
    int i0, i1, j0, j1;
    video_reader_t* video = video_reader_alloc_init(p_vid_in_path, p_vid_in_start, p_vid_in_stop, skip, 0,
                                                    p_vid_in_threads, VCDC_FFMPEG_IO, VCDC_HWACCEL_NONE, PIXFMT_GRAY8,
                                                    0, NULL, &i0, &i1, &j0, &j1);

    // ---------------- //
    // -- ALLOCATION -- //
    // ---------------- //

    PUTS("ALLOC");
    uint8_t** I = (uint8_t**)ui8matrix(i0, i1, j0, j1);
    uint8_t** M = (uint8_t**)ui8matrix(i0, i1, j0, j1);
    zero_ui8matrix(M, i0, i1, j0, j1);
    img_data_t* img_data = NULL;
    video_writer_t* video_writer = NULL;
    size_t n_threads = 4;
    if (p_trk_path) {
        img_data = image_color_alloc((i1 - i0) + 1, (j1 - j0) + 1);
        video_writer = video_writer_alloc_init(p_fra_out_path, p_vid_in_start, n_threads, i1 - i0 + 1, j1 - j0 + 1,
                                               PIXFMT_RGB24, VCDC_FFMPEG_IO, 0, 0, NULL);
    } else {
        img_data = image_gs_alloc((i1 - i0) + 1, (j1 - j0) + 1);
        video_writer = video_writer_alloc_init(p_fra_out_path, p_vid_in_start, n_threads, i1 - i0 + 1, j1 - j0 + 1,
                                               PIXFMT_GRAY8, VCDC_FFMPEG_IO, 0, 0, NULL);
    }

    // ----------------//
    // -- TRAITEMENT --//
    // ----------------//

    PUTS("LOOP");
    int frame;
    while ((frame = video_reader_get_frame(video, I, NULL)) != -1) {
        fprintf(stderr, "(II) Frame n°%4d\r", frame);
        max_reduce(M, i0, i1, j0, j1, (const uint8_t**)I);
    }
    fprintf(stderr, "\n");

    if (p_trk_path) {
        vec_track_t tracks;
        tracking_parse_tracks(p_trk_path, &tracks);

        if (p_gt_path) {
            validation_init(p_gt_path);
            validation_process(tracks);
        }

        size_t n_tracks = vector_size(tracks);
        BB_t* BBs = (BB_t*)malloc(sizeof(BB_t) * n_tracks);
        enum color_e* BBs_color = (enum color_e*)malloc(sizeof(enum color_e) * n_tracks);
        size_t m = 0;
        for (size_t t = 0; t < n_tracks; t++) {
            if ((!p_trk_only_meteor || tracks[t].obj_type == OBJ_METEOR) &&
                (tracks[t].end.frame >= (size_t)p_vid_in_start)) {
                BBs[m].frame_id = 0;
#ifdef FMDT_OPENCV_LINK
                BBs[m].track_id = p_trk_nat_num ? (m + 1) : tracks[t].id;
#else
                BBs[m].track_id = tracks[t].id;
#endif
                int xmin =
                    tracks[t].begin.x < tracks[t].end.x ? tracks[t].begin.x : tracks[t].end.x;
                int xmax =
                    tracks[t].begin.x < tracks[t].end.x ? tracks[t].end.x : tracks[t].begin.x;
                int ymin =
                    tracks[t].begin.y < tracks[t].end.y ? tracks[t].begin.y : tracks[t].end.y;
                int ymax =
                    tracks[t].begin.y < tracks[t].end.y ? tracks[t].end.y : tracks[t].begin.y;

                BBs[m].bb_x = (uint16_t)ceilf((float)(xmin + xmax) / 2.f);
                BBs[m].bb_y = (uint16_t)ceilf((float)(ymin + ymax) / 2.f);
                BBs[m].rx = BBs[m].bb_x - xmin;
                BBs[m].ry = BBs[m].bb_y - ymin;
                BBs[m].is_extrapolated = 0;

                if (tracks[t].obj_type != OBJ_UNKNOWN)
                    BBs_color[m] = g_obj_to_color[tracks[t].obj_type];
                else {
                    fprintf(stderr, "(EE) This should never happen... ('t' = %lu, 'tracks[t].obj_type' = %d)\n",
                            (unsigned long)t, tracks[t].obj_type);
                    exit(-1);
                }

                if (p_gt_path && g_is_valid_track[t] == 1)
                    BBs_color[m] = COLOR_GREEN; // COLOR_GREEN = true positive 'meteor'
                if (p_gt_path && g_is_valid_track[t] == 2)
                    BBs_color[m] = COLOR_RED; // COLOR_RED = false positive 'meteor'
                m++;
            }
        }

        int n_BB = m;
        image_color_draw_BBs(img_data, (const uint8_t**)M, PIXFMT_GRAY8, (const BB_t*)BBs,
                             (const enum color_e*)BBs_color, n_BB, p_trk_id, p_gt_path ? 1 : 0, 1);
        video_writer_save_frame(video_writer, (const uint8_t**)image_color_get_pixels_2d(img_data));
        vector_free(tracks);
        free(BBs);
        free(BBs_color);
        if (p_gt_path)
            validation_free();
    } else {
        uint8_t* pixels = image_gs_get_pixels(img_data);
        // this copy could be avoided...
        for (int i = i0; i <= i1; i++)
            for (int j = j0; j <= j1; j++)
                pixels[i * img_data->width + j] = M[i][j];
        video_writer_save_frame(video_writer, (const uint8_t**)image_gs_get_pixels_2d(img_data));
    }

    // ----------
    // -- free --
    // ----------

    free_ui8matrix(I, i0, i1, j0, j1);
    free_ui8matrix(M, i0, i1, j0, j1);
    if (p_trk_path)
        image_color_free(img_data);
    else
        image_gs_free(img_data);
    video_writer_free(video_writer);
    video_reader_free(video);

    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
