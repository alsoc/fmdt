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

enum reduction_e { RED_OP_NONE = 0,
                   RED_OP_MAX,
                   RED_OP_MIN,
                   RED_OP_MAX_MIN,
};

uint32_t g_frames_cnt = 0;
uint8_t g_pix_min = 255;
uint8_t g_pix_max = 0;
uint64_t g_pix_sum = 0;
uint64_t g_pix_cnt = 0;
double g_pix_mean = 0;
double g_pix_M2 = 0;
double g_pix_variance;
double g_pix_sample_variance;
double g_pix_std_dev;
uint32_t g_pix_histo[256]; // initialized to zeros

// see https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm
static inline void update_stats(const uint8_t x, const uint8_t stats_histo) {
    g_pix_min = (x < g_pix_min) ? x : g_pix_min;
    g_pix_max = (x > g_pix_max) ? x : g_pix_max;
    g_pix_sum += x;
    g_pix_cnt++;
    if (stats_histo)
        g_pix_histo[x]++;
    double delta = x - g_pix_mean;
    g_pix_mean = (g_pix_sum * 1.) / (g_pix_cnt * 1.);
    double delta2 = (x * 1.) - g_pix_mean;
    g_pix_M2 += delta * delta2;
}

// see https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Welford's_online_algorithm
static void finalize_stats(const uint8_t stats_histo) {
    if (g_pix_cnt < 2) {
        g_pix_variance = nanf("");
        g_pix_sample_variance = nanf("");
        g_pix_std_dev = nanf("");
    } else {
        g_pix_variance = g_pix_M2 / (g_pix_cnt * 1.);
        g_pix_sample_variance = g_pix_M2 / ((g_pix_cnt - 1) * 1.);
        g_pix_std_dev = sqrtf(g_pix_variance);
    }
    g_frames_cnt++;
}

void reduce_and_compute_stats(uint8_t** Max, uint8_t** Min, int i0, int i1, int j0, int j1, const uint8_t** I,
                              const uint8_t stats, const uint8_t stats_histo) {
    if (stats) {
        if (Max && Min) {
            for (int i = i0; i <= i1; i++) {
                for (int j = j0; j <= j1; j++) {
                    uint8_t x = I[i][j];
                    uint8_t max = Max[i][j];
                    uint8_t min = Min[i][j];
                    if (x < min) {
                        Min[i][j] = x;
                    }
                    if (x > max) {
                        Max[i][j] = x;
                    }
                    update_stats(x, stats_histo);
                }
            }
        } else if (Min) {
            for (int i = i0; i <= i1; i++) {
                for (int j = j0; j <= j1; j++) {
                    uint8_t x = I[i][j];
                    uint8_t min = Min[i][j];
                    if (x < min) {
                        Min[i][j] = x;
                    }
                    update_stats(x, stats_histo);
                }
            }
        } else if (Max) {
            for (int i = i0; i <= i1; i++) {
                for (int j = j0; j <= j1; j++) {
                    uint8_t x = I[i][j];
                    uint8_t max = Max[i][j];
                    if (x > max) {
                        Max[i][j] = x;
                    }
                    update_stats(x, stats_histo);
                }
            }
        } else {
            for (int i = i0; i <= i1; i++) {
                for (int j = j0; j <= j1; j++) {
                    uint8_t x = I[i][j];
                    update_stats(x, stats_histo);
                }
            }
        }
        finalize_stats(stats_histo);
    } else {
        if (Max && Min) {
            for (int i = i0; i <= i1; i++) {
                for (int j = j0; j <= j1; j++) {
                    uint8_t x = I[i][j];
                    uint8_t max = Max[i][j];
                    uint8_t min = Min[i][j];
                    if (x < min) {
                        Min[i][j] = x;
                    }
                    if (x > max) {
                        Max[i][j] = x;
                    }
                }
            }
        } else if (Min) {
            for (int i = i0; i <= i1; i++) {
                for (int j = j0; j <= j1; j++) {
                    uint8_t x = I[i][j];
                    uint8_t min = Min[i][j];
                    if (x < min) {
                        Min[i][j] = x;
                    }
                }
            }
        } else if (Max) {
            for (int i = i0; i <= i1; i++) {
                for (int j = j0; j <= j1; j++) {
                    uint8_t x = I[i][j];
                    uint8_t max = Max[i][j];
                    if (x > max) {
                        Max[i][j] = x;
                    }
                }
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
    char def_p_red_op[16] = "NONE";
    char* def_p_gt_path = NULL;
    char* def_p_fra_out_path = NULL;

    // help
    if (args_find(argc, argv, "--help,-h")) {
        fprintf(stderr, "  --vid-in-path      Video source                                               [%s]\n",
                def_p_vid_in_path ? def_p_vid_in_path : "NULL");
        fprintf(stderr, "  --vid-in-start     Starting frame in the video                                [%d]\n",
                def_p_vid_in_start);
        fprintf(stderr, "  --vid-in-stop      Ending frame in the video                                  [%d]\n",
                def_p_vid_in_stop);
        fprintf(stderr, "  --vid-in-threads   Select the number of threads to use to                     [%d]\n"
                        "                     decode video input (in ffmpeg)                                 \n",
                def_p_vid_in_threads);
        fprintf(stderr, "  --red-op           The reduction operator ('NONE', MAX', 'MIN' or 'MAX-MIN')  [%s]\n",
                def_p_red_op);
        fprintf(stderr, "  --trk-path         Path to the tracks files                                   [%s]\n",
                def_p_trk_path ? def_p_trk_path : "NULL");
#ifdef FMDT_OPENCV_LINK
        fprintf(stderr, "  --trk-id           Show the object ids on the output frame                        \n");
        fprintf(stderr, "  --trk-nat-num      Natural numbering of the object ids                            \n");
#endif
        fprintf(stderr, "  --trk-only-meteor  Show only meteors                                              \n");
        fprintf(stderr, "  --gt-path          File containing the ground truth                           [%s]\n",
                def_p_gt_path ? def_p_gt_path : "NULL");
        fprintf(stderr, "  --fra-out-path     Path to the frame output file                              [%s]\n",
                def_p_fra_out_path ? def_p_fra_out_path : "NULL");
        fprintf(stderr, "  --stats            Enable pixel statistics                                        \n");
        fprintf(stderr, "  --stats-histo      Enable histogram statistics (automatically sets '--stats')     \n");
        fprintf(stderr, "  --help, -h         This help                                                      \n");
        fprintf(stderr, "  --version, -v      Print the version                                              \n");
        exit(1);
    }

    // version
    if (args_find(argc, argv, "--version,-v")) {
        version_print("reduce");
        exit(0);
    }

    // Parsing Arguments
    const char* p_vid_in_path = args_find_char(argc, argv, "--vid-in-path,--in-video", def_p_vid_in_path);
    const int p_vid_in_start = args_find_int_min(argc, argv, "--vid-in-start,--fra-start", def_p_vid_in_start, 0);
    const int p_vid_in_stop = args_find_int_min(argc, argv, "--vid-in-stop,--fra-end", def_p_vid_in_stop, 0);
    const int p_vid_in_threads = args_find_int_min(argc, argv, "--vid-in-threads,--ffmpeg-threads", def_p_vid_in_threads, 0);
    const char* p_trk_path = args_find_char(argc, argv, "--trk-path,--in-tracks", def_p_trk_path);
    const char* p_red_op = args_find_char(argc, argv, "--red-op", def_p_red_op);
#ifdef FMDT_OPENCV_LINK
    const int p_trk_id = args_find(argc, argv, "--trk-id,--show-id");
    const int p_trk_nat_num = args_find(argc, argv, "--trk-nat-num,--nat-num");
#else
    const int p_trk_id = 0;
#endif
    const int p_trk_only_meteor = args_find(argc, argv, "--trk-only-meteor,--only-meteor");
    const char* p_gt_path = args_find_char(argc, argv, "--gt-path,--in-gt", def_p_gt_path);
    const char* p_fra_out_path = args_find_char(argc, argv, "--fra-out-path,--out-frame", def_p_fra_out_path);
    const int p_stats_histo = args_find(argc, argv, "--stats-histo");
    const int p_stats = p_stats_histo ? 1 : args_find(argc, argv, "--stats");

    // heading display
    printf("#  ---------------------\n");
    printf("# |         ----*       |\n");
    printf("# | --* FMDT-REDUCE --* |\n");
    printf("# |   -------*          |\n");
    printf("#  ---------------------\n");
    printf("#\n");
    printf("# Parameters:\n");
    printf("# -----------\n");
    printf("#  * vid-in-path     = %s\n", p_vid_in_path);
    printf("#  * vid-in-start    = %d\n", p_vid_in_start);
    printf("#  * vid-in-stop     = %d\n", p_vid_in_stop);
    printf("#  * vid-in-threads  = %d\n", p_vid_in_threads);
    printf("#  * red-op          = %s\n", p_red_op);
    printf("#  * trk-path        = %s\n", p_trk_path);
#ifdef FMDT_OPENCV_LINK
    printf("#  * trk-id          = %d\n", p_trk_id);
    printf("#  * trk-nat-num     = %d\n", p_trk_nat_num);
#endif
    printf("#  * trk-only-meteor = %d\n", p_trk_only_meteor);
    printf("#  * gt-path         = %s\n", p_gt_path);
    printf("#  * fra-out-path    = %s\n", p_fra_out_path);
    printf("#  * stats           = %d\n", p_stats);
    printf("#  * stats-histo     = %d\n", p_stats_histo);
    printf("#\n");

    enum reduction_e red_op;
    if (strcmp(p_red_op, "NONE") == 0)
        red_op = RED_OP_NONE;
    else if (strcmp(p_red_op, "MAX") == 0)
        red_op = RED_OP_MAX;
    else if (strcmp(p_red_op, "MIN") == 0)
        red_op = RED_OP_MIN;
    else if (strcmp(p_red_op, "MAX-MIN") == 0)
        red_op = RED_OP_MAX_MIN;
    else {
        fprintf(stderr, "(EE) '--red-op' contains an unsupported string (cur str = \"%s\")\n", p_red_op);
        exit(1);
    }

    // arguments checking
    if (!p_vid_in_path) {
        fprintf(stderr, "(EE) '--vid-in-path' is missing\n");
        exit(1);
    }
    if (p_fra_out_path && red_op == RED_OP_NONE) {
        fprintf(stderr, "(EE) '--fra-out-path' is defined, a reduction operator is expected\n");
        exit(1);
    }
    if (!p_fra_out_path && (int)red_op > 0) {
        fprintf(stderr, "(EE) '--fra-out-path' is missing\n");
        exit(1);
    }
    if (red_op == RED_OP_NONE && p_stats == 0) {
        fprintf(stderr, "(EE) You should at least specify a reduction operator or enable the statistics.\n");
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
    uint8_t** Max = NULL;
    if (red_op == RED_OP_MAX || red_op == RED_OP_MAX_MIN) {
        Max = (uint8_t**)ui8matrix(i0, i1, j0, j1);
        zero_ui8matrix(Max, i0, i1, j0, j1);
    }
    uint8_t** Min = NULL;
    if (red_op == RED_OP_MIN || red_op == RED_OP_MAX_MIN) {
        Min = (uint8_t**)ui8matrix(i0, i1, j0, j1);
        for (int i = i0; i <= i1; i++)
            for (int j = j0; j <= j1; j++)
                Min[i][j] = 255;
    }
    img_data_t* img_data = NULL;
    video_writer_t* video_writer = NULL;
    if (p_fra_out_path) {
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
    }

    // ----------------//
    // -- TRAITEMENT --//
    // ----------------//

    PUTS("LOOP");
    unsigned n_frames = 0;
    int frame;
    TIME_POINT(start_compute);
    while ((frame = video_reader_get_frame(video, I, NULL)) != -1) {
        fprintf(stderr, "(II) Frame n°%4d", frame);
        reduce_and_compute_stats(Max, Min, i0, i1, j0, j1, (const uint8_t**)I, p_stats, p_stats_histo);

        n_frames++;
        TIME_POINT(stop_compute);
        fprintf(stderr, " -- Time = %6.3f sec", TIME_ELAPSED_SEC(start_compute, stop_compute));
        fprintf(stderr, " -- FPS = %4d\r", (int)(n_frames / (TIME_ELAPSED_SEC(start_compute, stop_compute))));
        fflush(stderr);
    }
    fprintf(stderr, "\n");

    uint8_t** M = NULL;
    switch (red_op) {
    case RED_OP_NONE:
        break;
    case RED_OP_MAX:
        M = Max;
        break;
    case RED_OP_MIN:
        M = Min;
        break;
    case RED_OP_MAX_MIN:
        for (int i = i0; i <= i1; i++)
            for (int j = j0; j <= j1; j++)
                Max[i][j] -= Min[i][j];
        M = Max;
        break;
    default:
        fprintf(stderr, "(EE) It should never happen :-(\n");
        exit(1);
        break;
    }

    if (red_op != RED_OP_NONE) {
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
    }

    // -----------
    // -- stats --
    // -----------

    if (p_stats) {
        printf("#\n");
        printf("# Pixel Statistics:\n");
        printf("# -----------------\n");
        printf("#  * frames_cnt   = %u\n",   g_frames_cnt);
        printf("#  * pix_min      = %u\n",   g_pix_min);
        printf("#  * pix_max      = %u\n",   g_pix_max);
        printf("#  * pix_sum      = %llu\n", (unsigned long long int)g_pix_sum);
        printf("#  * pix_cnt      = %llu\n", (unsigned long long int)g_pix_cnt);
        printf("#  * pix_mean     = %f\n",   g_pix_mean);
        printf("#  * pix_variance = %f\n",   g_pix_variance);
        printf("#  * pix_std_dev  = %f\n",   g_pix_std_dev);
        printf("#\n");

        if (p_stats_histo) {
            printf("# Pixels histogram:\n");
            printf("# -----------------\n");
            for (uint16_t p = 0; p < 256; p++)
                printf("%u %u\n", p, g_pix_histo[p]);
            printf("# [tips] Display the histogram in the command line with `gnuplot`:\n");
            printf("#    ");
            printf("%s ", argv[0]);
            for (int a = 1; a < argc; a++)
               printf("%s ", argv[a]);
            printf("| \\\n");
            printf("#    gnuplot -e \\\n");
            printf("#    \"set terminal dumb size 120, 30; set autoscale; set xrange [0:255]; set title 'pixel histogram';\n");
            printf("#     set ylabel 'count'; set xlabel 'pixel value'; plot '-'  with lines notitle\";\n");
            printf("#\n");
        }
    }

    // ----------
    // -- free --
    // ----------

    free_ui8matrix(I, i0, i1, j0, j1);
    if (Max)
        free_ui8matrix(Max, i0, i1, j0, j1);
    if (Min)
        free_ui8matrix(Min, i0, i1, j0, j1);
    if (img_data) {
        if (p_trk_path)
            image_color_free(img_data);
        else
            image_gs_free(img_data);
    }
    if (video_writer)
        video_writer_free(video_writer);
    video_reader_free(video);

    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
