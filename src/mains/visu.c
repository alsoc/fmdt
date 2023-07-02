#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <nrc2.h>

#include "fmdt/macros.h"
#include "fmdt/args.h"
#include "fmdt/tools.h"
#include "vec.h"

#include "fmdt/image/image_compute.h"
#include "fmdt/tracking/tracking_io.h"
#include "fmdt/tracking/tracking_global.h"
#include "fmdt/validation.h"
#include "fmdt/video.h"
#include "fmdt/version.h"

void add_to_BB_coord_list(vec_BB_t* BBs, vec_color_e* BBs_color, size_t elem, int rx, int ry, int bb_x,
                          int bb_y, int frame_id, int track_id, int is_extrapolated, enum color_e color) {
#ifndef NDEBUG
    size_t alloc_amt = vector_get_alloc(*BBs);
    size_t alloc_amt2 = vector_get_alloc(*BBs_color);
    assert(alloc_amt == alloc_amt2);
#endif

    size_t vs = vector_size(*BBs);
#ifndef NDEBUG
    size_t vs2 = vector_size(*BBs_color);
    assert(vs == vs2);
    assert(elem < vs || elem == vs);
#endif

    BB_t* BB_elem = (vs == elem) ? vector_add_asg(BBs) : &(*BBs)[elem];
    BB_elem->frame_id = frame_id;
    BB_elem->track_id = track_id;
    BB_elem->bb_x = bb_x;
    BB_elem->bb_y = bb_y;
    BB_elem->rx = rx;
    BB_elem->ry = ry;
    BB_elem->is_extrapolated = is_extrapolated;

    if (vs == elem)
        vector_add(BBs_color, COLOR_MISC);
    enum color_e* BB_color_elem = &(*BBs_color)[elem];
    *BB_color_elem = color;
}

int main(int argc, char** argv) {
    // default values
    char* def_p_vid_in_path = NULL;
    int def_p_vid_in_start = 0;
    int def_p_vid_in_stop = 0;
    int def_p_vid_in_threads = 0;
    char* def_p_trk_path = NULL;
    char* def_p_trk_bb_path = NULL;
    char* def_p_gt_path = NULL;
    char* def_p_vid_out_path = NULL;

    // help
    if (args_find(argc, argv, "--help,-h")) {
        fprintf(stderr, "  --vid-in-path      Path to video file or to a folder of PGM images      [%s]\n",
                def_p_vid_in_path ? def_p_vid_in_path : "NULL");
        fprintf(stderr, "  --vid-in-start     Starting point of the video                          [%d]\n",
                def_p_vid_in_start);
        fprintf(stderr, "  --vid-in-stop      Ending point of the video                            [%d]\n",
                def_p_vid_in_stop);
        fprintf(stderr, "  --vid-in-threads   Select the number of threads to use to decode video  [%d]\n"
                        "                     input (in ffmpeg)                                        \n",
                def_p_vid_in_threads);

        fprintf(stderr, "  --trk-path         Path to the tracks file                              [%s]\n",
                def_p_trk_path ? def_p_trk_path : "NULL");
        fprintf(stderr, "  --trk-bb-path      Path the bounding boxes file                         [%s]\n",
                def_p_trk_bb_path ? def_p_trk_bb_path : "NULL");
#ifdef FMDT_OPENCV_LINK
        fprintf(stderr, "  --trk-id           Show the object ids on the output video and frames       \n");
        fprintf(stderr, "  --trk-nat-num      Natural numbering of the object ids                      \n");
#endif
        fprintf(stderr, "  --trk-only-meteor  Show only meteors                                        \n");
        fprintf(stderr, "  --gt-path          Path to ground truth file                            [%s]\n",
                def_p_gt_path ? def_p_gt_path : "NULL");
        fprintf(stderr, "  --vid-out-path     Path to the output video file (MPEG-4 format)        [%s]\n",
                def_p_vid_out_path ? def_p_vid_out_path : "NULL");
#ifdef FMDT_OPENCV_LINK
        fprintf(stderr, "  --vid-out-id       Write frame id on each frame                             \n");
#endif
        fprintf(stderr, "  --help, -h         This help                                                \n");
        fprintf(stderr, "  --version, -v      Print the version                                        \n");
        exit(1);
    }

    // version
    if (args_find(argc, argv, "--version,-v")) {
        version_print("visu");
        exit(0);
    }

    // Parsing Arguments
    const char* p_vid_in_path = args_find_char(argc, argv, "--vid-in-path,--in-video", def_p_vid_in_path);
    const int p_vid_in_start = args_find_int_min(argc, argv, "--vid-in-start,--fra-start", def_p_vid_in_start, 0);
    const int p_vid_in_stop = args_find_int_min(argc, argv, "--vid-in-stop,--fra-end", def_p_vid_in_stop, 0);
    const int p_vid_in_threads = args_find_int_min(argc, argv, "--vid-in-threads,--ffmpeg-threads", def_p_vid_in_threads, 0);
    const char* p_trk_path = args_find_char(argc, argv, "--trk-path,--in-tracks", def_p_trk_path);
    const char* p_trk_bb_path = args_find_char(argc, argv, "--trk-bb-path,--in-bb", def_p_trk_bb_path);
#ifdef FMDT_OPENCV_LINK
    const int p_trk_id = args_find(argc, argv, "--trk-id,--show-id");
    const int p_trk_nat_num = args_find(argc, argv, "--trk-nat-num,--nat-num");
#else
    const int p_trk_id = 0;
#endif
    const int p_trk_only_meteor = args_find(argc, argv, "--trk-only-meteor,--only-meteor");
    const char* p_gt_path = args_find_char(argc, argv, "--gt-path,--in-gt", def_p_gt_path);
    const char* p_vid_out_path = args_find_char(argc, argv, "--vid-out-path,--out-video", def_p_vid_out_path);
#ifdef FMDT_OPENCV_LINK
    const int p_vid_out_id = args_find(argc, argv, "--vid-out-id");
#endif

    // heading display
    printf("#  -------------------\n");
    printf("# |        ----*      |\n");
    printf("# | --* FMDT-VISU --* |\n");
    printf("# |  -------*         |\n");
    printf("#  -------------------\n");
    printf("#\n");
    printf("# Parameters:\n");
    printf("# -----------\n");
    printf("#  * vid-in-path     = %s\n", p_vid_in_path);
    printf("#  * vid-in-start    = %d\n", p_vid_in_start);
    printf("#  * vid-in-stop     = %d\n", p_vid_in_stop);
    printf("#  * vid-in-threads  = %d\n", p_vid_in_threads);
    printf("#  * trk-path        = %s\n", p_trk_path);
    printf("#  * trk-bb-path     = %s\n", p_trk_bb_path);
#ifdef FMDT_OPENCV_LINK
    printf("#  * trk-id          = %d\n", p_trk_id);
    printf("#  * trk-nat-num     = %d\n", p_trk_nat_num);
#endif
    printf("#  * trk-only-meteor = %d\n", p_trk_only_meteor);
    printf("#  * gt-path         = %s\n", p_gt_path);
    printf("#  * vid-out-path    = %s\n", p_vid_out_path);
#ifdef FMDT_OPENCV_LINK
    printf("#  * vid-out-id      = %d\n", p_vid_out_id);
#endif
    printf("#\n");

    // arguments checking
    if (!p_vid_in_path) {
        fprintf(stderr, "(EE) '--vid-in-path' is missing\n");
        exit(1);
    }
    if (!p_trk_path) {
        fprintf(stderr, "(EE) '--trk-path' is missing\n");
        exit(1);
    }
    if (!p_trk_bb_path) {
        fprintf(stderr, "(EE) '--trk-bb-path' is missing\n");
        exit(1);
    }
    if (!p_vid_out_path) {
        fprintf(stderr, "(EE) No output specified, '--vid-out-path' parameter has to be set\n");
        exit(1);
    }
#ifdef FMDT_OPENCV_LINK
    if (!p_trk_id && p_trk_nat_num)
        fprintf(stderr, "(WW) '--trk-nat-num ' will not work because '--trk-id' is not set.\n");
#endif
    if (p_vid_in_stop && p_vid_in_stop < p_vid_in_start) {
        fprintf(stderr, "(EE) '--vid-in-stop' has to be higher than '--vid-in-start'\n");
        exit(1);
    }

    vec_BB_t BBs = (vec_BB_t)vector_create();
    vec_color_e BBs_color = (vec_color_e)vector_create();

    tracking_init_global_data();
    vec_track_t tracks;
    tracking_parse_tracks(p_trk_path, &tracks);

    size_t max_LUT = 0;
    size_t n_tracks = vector_size(tracks);
    for (size_t i = 0; i < n_tracks; i++)
        if (tracks[i].id > max_LUT)
            max_LUT = (size_t)tracks[i].id;
    int* LUT_tracks_id = (int*)malloc(sizeof(int) * (max_LUT + 1));
    int* LUT_tracks_nat_num = (int*)malloc(sizeof(int) * (max_LUT + 1));
    memset(LUT_tracks_id, -1, max_LUT + 1);
    memset(LUT_tracks_nat_num, -1, max_LUT + 1);
    int j = 1;
    for (size_t i = 0; i < n_tracks; i++) {
        LUT_tracks_id[tracks[i].id] = i;
        if (!p_trk_only_meteor || tracks[i].obj_type == OBJ_METEOR)
            LUT_tracks_nat_num[tracks[i].id] = j++;
    }

    unsigned n_stars = 0, n_meteors = 0, n_noise = 0;
    tracking_count_objects(tracks, &n_stars, &n_meteors, &n_noise);
    printf("# Tracks read from file = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3lu]\n", n_meteors, n_stars,
           n_noise, (unsigned long)n_tracks);

    // init
    int b = 1;
    int i0, i1, j0, j1; // image dimension (y_min, y_max, x_min, x_max)
    video_reader_t* video = video_reader_alloc_init(p_vid_in_path, p_vid_in_start, p_vid_in_stop, 0, 0,
                                                    p_vid_in_threads, VCDC_FFMPEG_IO, &i0, &i1, &j0, &j1);
    uint8_t** I0 = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b);
    img_data_t* img_data = image_color_alloc((j1 - j0) - 1, (i1 - i0) + 1);

    // validation pour établir si une track est vrai/faux positif
    if (p_gt_path) {
        validation_init(p_gt_path);
        validation_process(tracks);
    } else {
        PUTS("NO VALIDATION");
    }

    // open BB pour l'affichage des rectangles englobants
    FILE* file_bb = fopen(p_trk_bb_path, "r");
    if (file_bb == NULL) {
        fprintf(stderr, "(EE) cannot open file '%s'\n", p_trk_bb_path);
        return EXIT_FAILURE;
    }

    printf("# The program is running...\n");

    size_t n_threads = 4;
    video_writer_t* video_writer = video_writer_alloc_init(p_vid_out_path, p_vid_in_start, n_threads, i1 - i0 + 1,
                                                           j1 - j0 + 1, PIXFMT_RGB24, VCDC_FFMPEG_IO);

    // parcours de la video
    enum color_e color = COLOR_MISC;
    int frame;
    char lines[1000];
    int frame_bb = -1, rx, ry, bb_x, bb_y, track_id, is_extrapolated;
    int is_first_read = 1;
    while ((frame = video_reader_get_frame(video, I0)) != -1) {
        fprintf(stderr, "(II) Frame n°%-4d\r", frame);
        fflush(stderr);
        int cpt = 0;

        // skip bounding boxes of previous frames
        while (frame_bb < frame) {
            if (fgets(lines, 100, file_bb) == NULL) {
                if (is_first_read) {
                    fprintf(stderr, "(EE) something went wrong when reading '%s'\n", p_trk_bb_path);
                    exit(1);
                }
                frame_bb = -1;
                break;
            }
            sscanf(lines, "%d %d %d %d %d %d %d ", &frame_bb, &rx, &ry, &bb_x, &bb_y, &track_id, &is_extrapolated);
        }
        is_first_read = 0;

        // affiche tous les BB de l'image
        while (frame_bb == frame) {
            if (!p_trk_only_meteor || tracks[LUT_tracks_id[track_id]].obj_type == OBJ_METEOR) {
                if (tracks[LUT_tracks_id[track_id]].obj_type != OBJ_UNKNOWN)
                    color = g_obj_to_color[tracks[LUT_tracks_id[track_id]].obj_type];
                else {
                    fprintf(stderr,
                            "(EE) This should never happen... ('cpt' = %d, 'track_id' = %d, 'LUT_tracks_id[track_id]' "
                            "= %d, 'tracks[LUT_tracks_id[track_id]].obj_type' = %d)\n",
                            cpt, track_id, LUT_tracks_id[track_id], tracks[LUT_tracks_id[track_id]].obj_type);
                    exit(-1);
                }
                if (p_gt_path && g_is_valid_track[LUT_tracks_id[track_id]] == 1)
                    color = COLOR_GREEN; // COLOR_GREEN = true  positive 'meteor'
                if (p_gt_path && g_is_valid_track[LUT_tracks_id[track_id]] == 2)
                    color = COLOR_RED; // COLOR_RED = false positive 'meteor'

#ifdef FMDT_OPENCV_LINK
                int display_track_id = p_trk_nat_num ? LUT_tracks_nat_num[track_id] : track_id;
#else
                int display_track_id = track_id;
#endif
                add_to_BB_coord_list(&BBs, &BBs_color, cpt, rx, ry, bb_x, bb_y, frame_bb, display_track_id,
                                     is_extrapolated, color);
                cpt++;
            }

            if (fgets(lines, 100, file_bb) == NULL) {
                frame_bb = -1;
                break;
            }
            // cherche prochain BB à afficher
            sscanf(lines, "%d %d %d %d %d %d %d ", &frame_bb, &rx, &ry, &bb_x, &bb_y, &track_id, &is_extrapolated);
        }

        image_color_draw_BBs(img_data, (const uint8_t**)I0, (const BB_t*)BBs, (const enum color_e*)BBs_color,
                             cpt, p_trk_id, p_gt_path ? 1 : 0);
#ifdef FMDT_OPENCV_LINK
        if (p_vid_out_id)
            image_color_draw_frame_id(img_data, frame);
#endif
        video_writer_save_frame(video_writer, (const uint8_t**)image_color_get_pixels_2d(img_data));
    }
    video_writer_free(video_writer);
    free_ui8matrix(I0, i0 - b, i1 + b, j0 - b, j1 + b);
    vector_free(tracks);
    free(LUT_tracks_id);
    free(LUT_tracks_nat_num);
    vector_free(BBs);
    vector_free(BBs_color);
    if (p_gt_path)
        validation_free();
    video_reader_free(video);
    if (file_bb)
        fclose(file_bb);
    image_color_free(img_data);

    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
