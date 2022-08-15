#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#include "args.h"
#include "defines.h"
#include "ballon.h"
#include "CCL.h"
#include "tools.h"
#include "features.h"
#include "KPPV.h"
#include "threshold.h"
#include "tracking.h"
#include "video.h"
#include "macros.h"

int main(int argc, char** argv) {
    // default values
    int def_start_frame = 0;
    int def_end_frame = 200000;
    int def_skip_frames = 0;
    int def_light_min = 55;
    int def_light_max = 80;
    int def_surface_min = 3;
    int def_surface_max = 1000;
    int def_k = 3;
    int def_r_extrapol = 5;
    int def_d_line = 25;
    int def_min_fra_star = 3;
    float def_diff_deviation = 4.f;
    char* def_input_video = NULL;
    char* def_output_frames = NULL;
    char* def_output_bb = NULL;
    char* def_output_stats = NULL;

    // Help
    if (args_find(argc, argv, "-h")) {
        fprintf(stderr,
                "  --input-video       Path to video file                                                   [%s]\n",
                def_input_video);
        fprintf(stderr,
                "  --output-frames     Path to frames output folder                                         [%s]\n",
                def_output_frames);
        fprintf(stderr,
                "  --output-bb         Path to the file containing the bounding boxes (frame by frame)      [%s]\n",
                def_output_bb);
        fprintf(stderr,
                "  --output-stats      TODO! Path to folder                                                 [%s]\n",
                def_output_stats);
        fprintf(stderr,
                "  --start-frame       Starting point of the video                                          [%d]\n",
                def_start_frame);
        fprintf(stderr,
                "  --end-frame         Ending point of the video                                            [%d]\n",
                def_end_frame);
        fprintf(stderr,
                "  --skip-frames       Number of skipped frames                                             [%d]\n",
                def_skip_frames);
        fprintf(stderr,
                "  --light-min         Low hysteresis threshold (grayscale [0;255])                         [%d]\n",
                def_light_min);
        fprintf(stderr,
                "  --light-max         High hysteresis threshold (grayscale [0;255])                        [%d]\n",
                def_light_max);
        fprintf(stderr,
                "  --surface-min       Maximum area of the CC                                               [%d]\n",
                def_surface_min);
        fprintf(stderr,
                "  --surface-max       Minimum area of the CC                                               [%d]\n",
                def_surface_max);
        fprintf(stderr,
                "  -k                  Number of neighbours                                                 [%d]\n",
                def_k);
        fprintf(stderr,
                "  --r-extrapol        Search radius for the next CC in case of extrapolation               [%d]\n",
                def_r_extrapol);
        fprintf(stderr,
                "  --d-line            Position tolerance of a point going through a line                   [%d]\n",
                def_d_line);
        fprintf(stderr,
                "  --min-fra-star      Minimum number of frames required to track a star                    [%d]\n",
                def_min_fra_star);
        fprintf(stderr,
                "  --diff-deviation    Differential deviation factor for motion detection (motion error of      \n");
        fprintf(stderr,
                "                      one CC has to be superior to diff_deviation * standard deviation)    [%f]\n",
                def_diff_deviation);
        fprintf(stderr,
                "  --track-all         Tracks all object types (star, meteor or noise)                          \n");
        fprintf(stderr,
                "  -h                  This help                                                                \n");
        exit(1);
    }

    // Parsing Arguments
    int start_frame = args_find_int(argc, argv, "--start-frame", def_start_frame);
    int end_frame = args_find_int(argc, argv, "--end-frame", def_end_frame);
    int skip_frames = args_find_int(argc, argv, "--skip-frames", def_skip_frames);
    int light_min = args_find_int(argc, argv, "--light-min", def_light_min);
    int light_max = args_find_int(argc, argv, "--light-max", def_light_max);
    int surface_min = args_find_int(argc, argv, "--surface-min", def_surface_min);
    int surface_max = args_find_int(argc, argv, "--surface-max", def_surface_max);
    int k = args_find_int(argc, argv, "-k", def_k);
    int r_extrapol = args_find_int(argc, argv, "--r-extrapol", def_r_extrapol);
    int d_line = args_find_int(argc, argv, "--d-line", def_d_line);
    int min_fra_star = args_find_int(argc, argv, "--min-fra-star", def_min_fra_star);
    float diff_deviation = args_find_float(argc, argv, "--diff-deviation", def_diff_deviation);
    char* input_video = args_find_char(argc, argv, "--input-video", def_input_video);
    char* output_frames = args_find_char(argc, argv, "--output-frames", def_output_frames);
    char* output_bb = args_find_char(argc, argv, "--output-bb", def_output_bb);
    char* output_stats = args_find_char(argc, argv, "--output-stats", def_output_stats);
    int track_all = args_find(argc, argv, "--track-all");

    // heading display
    printf("#  -----------------------\n");
    printf("# |          ----*        |\n");
    printf("# | --* METEOR-DETECT --* |\n");
    printf("# |   -------*            |\n");
    printf("#  -----------------------\n");
    printf("#\n");
    printf("# Parameters:\n");
    printf("# -----------\n");
    printf("#  * input-video   = %s\n", input_video);
    printf("#  * output-frames = %s\n", output_frames);
    printf("#  * output-bb     = %s\n", output_bb);
    printf("#  * output-stats  = %s\n", output_stats);
    printf("#  * start-frame   = %d\n", start_frame);
    printf("#  * end-frame     = %d\n", end_frame);
    printf("#  * skip-frames   = %d\n", skip_frames);
    printf("#  * light-min     = %d\n", light_min);
    printf("#  * light-max     = %d\n", light_max);
    printf("#  * surface-min   = %d\n", surface_min);
    printf("#  * surface-max   = %d\n", surface_max);
    printf("#  * k             = %d\n", k);
    printf("#  * r-extrapol    = %d\n", r_extrapol);
    printf("#  * d-line        = %d\n", d_line);
    printf("#  * min-fra-star  = %d\n", min_fra_star);
    printf("#  * diff-deviaton = %4.2f\n", diff_deviation);
    printf("#  * track-all     = %d\n", track_all);
    printf("#\n");

    if (!input_video) {
        fprintf(stderr, "(EE) '--input-video' is missing\n");
        exit(1);
    }
    if (!output_frames)
        fprintf(stderr, "(II) '--output-frames' is missing -> no frames will be saved\n");
    if (!output_stats)
        fprintf(stderr, "(II) '--output-stats' is missing -> no stats will be saved\n");

    // sequence
    double theta, tx, ty;
    int frame;

    // allocations on the heap
    ROI_t* stats0 = (ROI_t*)malloc(MAX_ROI_SIZE * sizeof(ROI_t));
    ROI_t* stats1 = (ROI_t*)malloc(MAX_ROI_SIZE * sizeof(ROI_t));
    ROI_t* stats_shrink = (ROI_t*)malloc(MAX_ROI_SIZE * sizeof(ROI_t));
    track_t* tracks = (track_t*)malloc(MAX_TRACKS_SIZE * sizeof(track_t));
    BB_t** BB_array = (BB_t**)malloc(MAX_N_FRAMES * sizeof(BB_t*));
    ROIx2_t* ROI_history = (ROIx2_t*)malloc(MAX_ROI_HISTORY_SIZE * sizeof(ROIx2_t));

    int offset = 0;
    int tracks_cnt = -1;

    int n0 = 0;
    int n1 = 0;

    // image
    int b = 1;
    int i0, i1, j0, j1;

    // ------------------------- //
    // -- INITIALISATION VIDEO-- //
    // ------------------------- //

    PUTS("INIT VIDEO");
    video_t* video = video_init_from_file(input_video, start_frame, end_frame, skip_frames, &i0, &i1, &j0, &j1);

    // ---------------- //
    // -- ALLOCATION -- //
    // ---------------- //

    PUTS("ALLOC");

    // struct for image processing
    ballon_t* ballon = ballon_alloc(i0, i1, j0, j1, b);

    // -------------------------- //
    // -- INITIALISATION MATRIX-- //
    // -------------------------- //

    tracking_init_global_data();
    ballon_init(ballon, i0, i1, j0, j1, b);
    KKPV_data_t* kppv_data = KPPV_init(0, MAX_KPPV_SIZE, 0, MAX_KPPV_SIZE);
    features_init_ROI(stats0, MAX_ROI_SIZE);
    features_init_ROI(stats1, MAX_ROI_SIZE);
    tracking_init_tracks(tracks, MAX_TRACKS_SIZE);
    tracking_init_BB_array(BB_array);
    CCL_data_t* ccl_data = CCL_LSL_init(i0, i1, j0, j1);

    // ----------------//
    // -- TRAITEMENT --//
    // ----------------//

    PUTS("LOOP");
    if (!video_get_next_frame(video, ballon->I0))
        exit(1);

    printf("# The program is running...\n");
    unsigned n_frames = 0;
    unsigned n_tracks = 0, n_stars = 0, n_meteors = 0, n_noise = 0;
    while (video_get_next_frame(video, ballon->I1)) {
        assert(frame < MAX_N_FRAMES);
        frame = video->frame_current - 2;
        fprintf(stderr, "(II) Frame n°%4d", frame);

        PUTS("\t Step 1 : seuillage low/high");
        tools_copy_ui8matrix_ui8matrix(ballon->I0, i0, i1, j0, j1, ballon->SH);
        tools_copy_ui8matrix_ui8matrix(ballon->I0, i0, i1, j0, j1, ballon->SM);
        threshold_high(ballon->SM, i0, i1, j0, j1, light_min);
        threshold_high(ballon->SH, i0, i1, j0, j1, light_max);
        tools_convert_ui8matrix_ui32matrix(ballon->SM, i0, i1, j0, j1, ballon->SM32);
        tools_convert_ui8matrix_ui32matrix(ballon->SH, i0, i1, j0, j1, ballon->SH32);

        PUTS("\t Step 2 : ECC/ACC");
        n1 = CCL_LSL_apply(ccl_data, ballon->SM32, i0, i1, j0, j1);
        features_extract(ballon->SM32, i0, i1, j0, j1, stats1, n1);

        PUTS("\t Step 3 : seuillage hysteresis && filter surface");
        features_merge_HI_CCL_v2(ballon->SH32, ballon->SM32, i0, i1, j0, j1, stats1, n1, surface_min, surface_max);
        int n_shrink = features_shrink_stats(stats1, stats_shrink, n1);

        PUTS("\t Step 4 : mise en correspondance");
        KPPV_match(kppv_data, stats0, stats_shrink, n0, n_shrink, k);

        PUTS("\t Step 5 : recalage");
        features_motion(stats0, stats_shrink, n0, n_shrink, &theta, &tx, &ty);

        PUTS("\t Step 6: tracking");
        tracking_perform(stats0, stats_shrink, ROI_history, tracks, BB_array, n0, n_shrink, frame, &tracks_cnt, &offset,
                         theta, tx, ty, r_extrapol, d_line, diff_deviation, track_all, min_fra_star);

        PUTS("\t [DEBUG] Saving frames");
        if (output_frames) {
            tools_create_folder(output_frames);
            tools_save_frame_ui32matrix(output_frames, ballon->SH32, i0, i1, j0, j1);
        }

        PUTS("\t [DEBUG] Saving stats");
        if (output_stats) {
            tools_create_folder(output_stats);
            KPPV_save_asso_conflicts(output_stats, frame, kppv_data, n0, n_shrink, stats0, stats_shrink, tracks,
                                     tracks_cnt + 1);
            // tools_save_motion(path_motion, theta, tx, ty, frame-1);
            // tools_save_motionExtraction(path_extraction, stats0, stats_shrink, n0, theta, tx, ty, frame-1);
            // tools_save_error(path_error, stats0, n0);
        }

        SWAP_UI8(ballon->I0, ballon->I1);
        SWAP_STATS(stats0, stats_shrink, n_shrink);
        n0 = n_shrink;
        n_frames++;

        n_tracks = tracking_count_objects(tracks, (unsigned)tracks_cnt + 1, &n_stars, &n_meteors, &n_noise);
        fprintf(stderr, " -- Tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3d]\r", n_meteors, n_stars,
                n_noise, n_tracks);
        fflush(stderr);
    }
    fprintf(stderr, "\n");

    if (output_bb)
        tracking_save_array_BB(output_bb, BB_array, tracks, MAX_N_FRAMES, track_all);
    tracking_print_tracks(stdout, tracks, tracks_cnt + 1);

    printf("# Statistics:\n");
    printf("# -> Processed frames = %4d\n", n_frames);
    printf("# -> Detected tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3d]\n", n_meteors, n_stars,
           n_noise, n_tracks);

    // ----------
    // -- free --
    // ----------

    ballon_free(ballon, i0, i1, j0, j1, b);
    video_free(video);
    CCL_LSL_free(ccl_data);
    KPPV_free(kppv_data);
    tracking_free_BB_array(BB_array);
    free(stats0);
    free(stats1);
    free(stats_shrink);
    free(tracks);
    free(BB_array);
    free(ROI_history);

    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
