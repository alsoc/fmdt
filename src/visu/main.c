#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ffmpeg-io/writer.h>
#ifdef OPENCV_LINK
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#endif
#include <nrc2.h>

#include "fmdt/macros.h"
#include "fmdt/args.h"
#include "fmdt/tools.h"
#include "fmdt/tracking.h"
#include "fmdt/validation.h"
#include "fmdt/video.h"
#include "fmdt/images.h"
#include "vec.h"

void add_to_BB_coord_list(vec_BB_t* BB_list, vec_color_e* BB_list_color, size_t elem, int rx, int ry, int bb_x,
                          int bb_y, int frame_id, int track_id, int is_extrapolated, enum color_e color) {
#ifndef NDEBUG
    size_t alloc_amt = vector_get_alloc(*BB_list);
    size_t alloc_amt2 = vector_get_alloc(*BB_list_color);
    assert(alloc_amt == alloc_amt2);
#endif

    size_t vs = vector_size(*BB_list);
#ifndef NDEBUG
    size_t vs2 = vector_size(*BB_list_color);
    assert(vs == vs2);
    assert(elem < vs || elem == vs);
#endif

    BB_t* BB_elem = (vs == elem) ? vector_add_asg(BB_list) : &(*BB_list)[elem];
    BB_elem->frame_id = frame_id;
    BB_elem->track_id = track_id;
    BB_elem->bb_x = bb_x;
    BB_elem->bb_y = bb_y;
    BB_elem->rx = rx;
    BB_elem->ry = ry;
    BB_elem->is_extrapolated = is_extrapolated;

    if (vs == elem)
        vector_add(BB_list_color, MISC);
    enum color_e* BB_color_elem = &(*BB_list_color)[elem];
    *BB_color_elem = color;
}

int get_next_frame(video_t* video, images_t* images, uint8_t** I) {
    if (video)
        return video_get_next_frame(video, I);
    else if (images)
        return images_get_next_frame(images, I);
    return 0;
}

int main(int argc, char** argv) {
    // default values
    char* def_p_in_tracks = NULL;
    char* def_p_in_bb = NULL;
    char* def_p_in_video = NULL;
    char* def_p_out_video = NULL;
    char* def_p_out_frames = NULL;
    char* def_p_in_gt = NULL;
    int def_p_ffmpeg_threads = 0;
    int def_p_fra_start = 0;
    int def_p_fra_end = 0;
#ifdef OPENCV_LINK
    char def_p_img_ext[] = "ppm";
#endif

    // help
    if (args_find(argc, argv, "-h")) {
        fprintf(stderr, "  --in-tracks       Path to the tracks file                              [%s]\n",
                def_p_in_tracks ? def_p_in_tracks : "NULL");
        fprintf(stderr, "  --in-bb           Path the bounding boxes file                         [%s]\n",
                def_p_in_bb ? def_p_in_bb : "NULL");
        fprintf(stderr, "  --in-video        Path to video file or to a folder of PGM images      [%s]\n",
                def_p_in_video ? def_p_in_video : "NULL");
        fprintf(stderr, "  --in-gt           Path to ground truth file                            [%s]\n",
                def_p_in_gt ? def_p_in_gt : "NULL");
        fprintf(stderr, "  --out-video       Path to the output video file (MPEG-4 format)        [%s]\n",
                def_p_out_video ? def_p_out_video : "NULL");
        fprintf(stderr, "  --out-frames      Path to the frames output folder                     [%s]\n",
                def_p_out_frames ? def_p_out_frames : "NULL");
#ifdef OPENCV_LINK
        fprintf(stderr, "  --show-id         Show the object ids on the output video and frames       \n");
        fprintf(stderr, "  --nat-num         Natural numbering of the object ids                      \n");
        fprintf(stderr, "  --img-ext         Image extension of saved frames ('jpg', 'png', ...)  [%s]\n",
                def_p_img_ext);
#endif
        fprintf(stderr, "  --only-meteor     Show only meteors                                        \n");
        fprintf(stderr, "  --ffmpeg-threads  Select the number of threads to use to decode video  [%d]\n"
                        "                    input (in ffmpeg)                                        \n",
                def_p_ffmpeg_threads);
        fprintf(stderr, "  --fra-start       Starting point of the video                          [%d]\n",
                def_p_fra_start);
        fprintf(stderr, "  --fra-end         Ending point of the video                            [%d]\n",
                def_p_fra_end);
        fprintf(stderr, "  -h                This help                                                \n");
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
    const int p_show_id = args_find(argc, argv, "--show-id");
    const int p_nat_num = args_find(argc, argv, "--nat-num");
    const char* p_img_ext = args_find_char(argc, argv, "--img-ext", def_p_img_ext);
#else
    const int p_show_id = 0;
    const char p_img_ext[] = "ppm";
#endif
    const int p_only_meteor = args_find(argc, argv, "--only-meteor");
    const int p_ffmpeg_threads = args_find_int_min(argc, argv, "--ffmpeg-threads", def_p_ffmpeg_threads, 0);
    const int p_fra_start = args_find_int_min(argc, argv, "--fra-start", def_p_fra_start, 0);
    const int p_fra_end = args_find_int_min(argc, argv, "--fra-end", def_p_fra_end, 0);

    // heading display
    printf("#  -------------------\n");
    printf("# |        ----*      |\n");
    printf("# | --* FMDT-VISU --* |\n");
    printf("# |  -------*         |\n");
    printf("#  -------------------\n");
    printf("#\n");
    printf("# Parameters:\n");
    printf("# -----------\n");
    printf("#  * in-tracks      = %s\n", p_in_tracks);
    printf("#  * in-bb          = %s\n", p_in_bb);
    printf("#  * in-video       = %s\n", p_in_video);
    printf("#  * in-gt          = %s\n", p_in_gt);
    printf("#  * out-video      = %s\n", p_out_video);
    printf("#  * out-frames     = %s\n", p_out_frames);
#ifdef OPENCV_LINK
    printf("#  * show-id        = %d\n", p_show_id);
    printf("#  * nat-num        = %d\n", p_nat_num);
    printf("#  * img-ext        = %s\n", p_img_ext);
#endif
    printf("#  * only-meteor    = %d\n", p_only_meteor);
    printf("#  * ffmpeg-threads = %d\n", p_ffmpeg_threads);
    printf("#  * fra-start      = %d\n", p_fra_start);
    printf("#  * fra-end        = %d\n", p_fra_end);
    printf("#\n");

    // arguments checking
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
    if (!p_out_video && !p_out_frames) {
        fprintf(stderr, "(EE) No output specified, '--out-video' or '--out-frames' parameter has to be set\n");
        exit(1);
    }
#ifdef OPENCV_LINK
    if (!p_show_id && p_nat_num)
        fprintf(stderr, "(WW) '--nat-num' will not work because '--show-id' is not set.\n");
#endif
    if (p_ffmpeg_threads < 0) {
        fprintf(stderr, "(EE) '--ffmpeg-threads' has to be bigger or equal to 0\n");
        exit(1);
    }
    if (p_fra_end && p_fra_end < p_fra_start) {
        fprintf(stderr, "(EE) '--fra-end' has to be higher than '--fra-start'\n");
        exit(1);
    }

    vec_BB_t BB_list = (vec_BB_t)vector_create();
    vec_color_e BB_list_color = (vec_color_e)vector_create();

    tracking_init_global_data();
    vec_track_t track_array;
    tracking_parse_tracks(p_in_tracks, &track_array);

    size_t max_LUT = 0;
    size_t n_tracks = vector_size(track_array);
    for (size_t i = 0; i < n_tracks; i++)
        if (track_array[i].id > max_LUT)
            max_LUT = (size_t)track_array[i].id;
    int* LUT_tracks_id = (int*)malloc(sizeof(int) * (max_LUT + 1));
    int* LUT_tracks_nat_num = (int*)malloc(sizeof(int) * (max_LUT + 1));
    memset(LUT_tracks_id, -1, max_LUT + 1);
    memset(LUT_tracks_nat_num, -1, max_LUT + 1);
    int j = 1;
    for (size_t i = 0; i < n_tracks; i++) {
        LUT_tracks_id[track_array[i].id] = i;
        if (!p_only_meteor || track_array[i].obj_type == METEOR)
            LUT_tracks_nat_num[track_array[i].id] = j++;

    }

    unsigned n_stars = 0, n_meteors = 0, n_noise = 0;
    tracking_count_objects(track_array, &n_stars, &n_meteors, &n_noise);
    printf("# Tracks read from file = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3lu]\n", n_meteors, n_stars,
           n_noise, (unsigned long)n_tracks);

    // init
    int b = 1;
    int i0, i1, j0, j1; // image dimension (y_min, y_max, x_min, x_max)
    video_t* video = NULL;
    images_t* images = NULL;
    if (!tools_is_dir(p_in_video)) {
        video = video_init_from_file(p_in_video, p_fra_start, p_fra_end, 0, p_ffmpeg_threads, &i0, &i1, &j0, &j1);
    } else {
        images = images_init_from_path(p_in_video, p_fra_start, p_fra_end, 0, 0);
        i0 = images->i0; i1 = images->i1; j0 = images->j0; j1 = images->j1;
    }
    uint8_t** I0 = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b);
    img_data_t* img_data = tools_color_img_alloc1((j1 - j0) - 1, (i1 - i0) + 1, p_out_frames ? p_out_frames : "",
                                                  p_img_ext, p_show_id);

    // validation pour établir si une track est vrai/faux positif
    if (p_in_gt) {
        validation_init(p_in_gt);
        validation_process(track_array);
    } else {
        PUTS("NO VALIDATION");
    }

    // open BB pour l'affichage des rectangles englobants
    FILE* file_bb = fopen(p_in_bb, "r");
    if (file_bb == NULL) {
        fprintf(stderr, "(EE) cannot open file '%s'\n", p_in_bb);
        return EXIT_FAILURE;
    }

    printf("# The program is running...\n");

    ffmpeg_handle writer_video;
    if (p_out_video) {
        ffmpeg_init(&writer_video);

        // set the number of output threads to 4 for ffmpeg-io
        ffmpeg_options ffmpeg_opts;
        ffmpeg_options_init(&ffmpeg_opts);
        ffmpeg_opts.threads_output = 4;

        writer_video.input.width = j1 - j0 + 1;
        writer_video.input.height = i1 - i0 + 1;
        writer_video.input.pixfmt = ffmpeg_str2pixfmt("rgb24");
        if (!ffmpeg_start_writer(&writer_video, p_out_video, &ffmpeg_opts)) {
            fprintf(stderr, "(EE) Something went wrong when starting to write output video.");
            exit(-1);
        }
    }

    // parcours de la video
    enum color_e color = MISC;
    int frame;
    char lines[1000];
    int frame_bb = -1, rx, ry, bb_x, bb_y, track_id, is_extrapolated;
    int is_first_read = 1;
    while ((frame = get_next_frame(video, images, I0)) != -1) {
        fprintf(stderr, "(II) Frame n°%-4d\r", frame);
        fflush(stderr);
        int cpt = 0;

        // skip bounding boxes of previous frames
        while (frame_bb < frame) {
            if (fgets(lines, 100, file_bb) == NULL) {
                if (is_first_read) {
                    fprintf(stderr, "(EE) something went wrong when reading '%s'\n", p_in_bb);
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
            if (!p_only_meteor || track_array[LUT_tracks_id[track_id]].obj_type == METEOR) {
                if (track_array[LUT_tracks_id[track_id]].obj_type != UNKNOWN)
                    color = g_obj_to_color[track_array[LUT_tracks_id[track_id]].obj_type];
                else {
                    fprintf(stderr,
                            "(EE) This should never happen... ('cpt' = %d, 'track_id' = %d, 'LUT_tracks_id[track_id]' "
                            "= %d, 'track_array[LUT_tracks_id[track_id]].obj_type' = %d)\n",
                            cpt, track_id, LUT_tracks_id[track_id], track_array[LUT_tracks_id[track_id]].obj_type);
                    exit(-1);
                }
                if (p_in_gt && g_is_valid_track[LUT_tracks_id[track_id]] == 1)
                    color = GREEN; // GREEN = true  positive 'meteor'
                if (p_in_gt && g_is_valid_track[LUT_tracks_id[track_id]] == 2)
                    color = RED; // RED   = false positive 'meteor'

#ifdef OPENCV_LINK
                int display_track_id = p_nat_num ? LUT_tracks_nat_num[track_id] : track_id;
#else
                int display_track_id = track_id;
#endif
                add_to_BB_coord_list(&BB_list, &BB_list_color, cpt, rx, ry, bb_x, bb_y, frame_bb, display_track_id,
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

        tools_color_img_draw_BB(img_data, (const uint8_t**)I0, (const BB_t*)BB_list, (const enum color_e*)BB_list_color,
                                cpt, p_in_gt ? 1 : 0);
        if (p_out_video) {
#ifdef OPENCV_LINK
            cv::Mat img_rgb;
            // convert BGR to RGB format
            cv::cvtColor(*(cv::Mat*)img_data->pixels, img_rgb, cv::COLOR_BGR2RGB);
            rgb8_t* pixels = (rgb8_t*)img_rgb.data;
#else
            rgb8_t* pixels = tools_color_img_get_pixels(img_data);
#endif
            if (!ffmpeg_write1d(&writer_video, (uint8_t*)pixels, img_data->width * 3)) {
                fprintf(stderr, "(EE) ffmpeg_write1d: %s, frame: %d\n", ffmpeg_error2str(writer_video.error), frame);
                exit(-1);
            }
        }
        if (p_out_frames)
            tools_color_img_write1(img_data, frame);
    }
    if (p_out_video)
        ffmpeg_stop_writer(&writer_video);
    free_ui8matrix(I0, i0 - b, i1 + b, j0 - b, j1 + b);
    vector_free(track_array);
    free(LUT_tracks_id);
    free(LUT_tracks_nat_num);
    vector_free(BB_list);
    vector_free(BB_list_color);
    if (p_in_gt)
        validation_free();
    if (video)
        video_free(video);
    if (images)
        images_free(images);
    if (file_bb)
        fclose(file_bb);
    tools_color_img_free(img_data);

    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
