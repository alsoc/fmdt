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
    int def_skip_fra = 0;
    int def_light_min = 55;
    int def_light_max = 80;
    int def_surface_min = 3;
    int def_surface_max = 1000;
    int def_k = 3;
    int def_r_extrapol = 5;
    float def_angle_max = 20;
    int def_fra_star_min = 15;
    int def_fra_meteor_min = 3;
    int def_fra_meteor_max = 100;
    float def_diff_dev = 4.f;
    char* def_in_video = NULL;
    char* def_out_frames = NULL;
    char* def_out_bb = NULL;
    char* def_out_stats = NULL;

    // Help
    if (args_find(argc, argv, "-h")) {
        fprintf(stderr,
                "  --in-video          Path to video file                                                     [%s]\n",
                def_in_video);
        fprintf(stderr,
                "  --out-frames        Path to frames output folder                                           [%s]\n",
                def_out_frames);
        fprintf(stderr,
                "  --out-bb            Path to the file containing the bounding boxes (frame by frame)        [%s]\n",
                def_out_bb);
        fprintf(stderr,
                "  --out-stats         TODO! Path to folder                                                   [%s]\n",
                def_out_stats);
        fprintf(stderr,
                "  --fra-start         Starting point of the video                                            [%d]\n",
                def_start_frame);
        fprintf(stderr,
                "  --end-fra           Ending point of the video                                              [%d]\n",
                def_end_frame);
        fprintf(stderr,
                "  --skip-fra          Number of skipped frames                                               [%d]\n",
                def_skip_fra);
        fprintf(stderr,
                "  --light-min         Low hysteresis threshold (grayscale [0;255])                           [%d]\n",
                def_light_min);
        fprintf(stderr,
                "  --light-max         High hysteresis threshold (grayscale [0;255])                          [%d]\n",
                def_light_max);
        fprintf(stderr,
                "  --surface-min       Maximum area of the CC                                                 [%d]\n",
                def_surface_min);
        fprintf(stderr,
                "  --surface-max       Minimum area of the CC                                                 [%d]\n",
                def_surface_max);
        fprintf(stderr,
                "  -k                  Number of neighbours                                                   [%d]\n",
                def_k);
        fprintf(stderr,
                "  --r-extrapol        Search radius for the next CC in case of extrapolation                 [%d]\n",
                def_r_extrapol);
        fprintf(stderr,
                "  --angle-max         Tracking angle max between two consecutive meteor moving points        [%f]\n",
                def_angle_max);
        fprintf(stderr,
                "  --fra-star-min      Minimum number of frames required to track a star                      [%d]\n",
                def_fra_star_min);
        fprintf(stderr,
                "  --fra-meteor-min    Minimum number of frames required to track a meteor                    [%d]\n",
                def_fra_meteor_min);
        fprintf(stderr,
                "  --fra-meteor-max    Maximum number of frames required to track a meteor                    [%d]\n",
                def_fra_meteor_max);
        fprintf(stderr,
                "  --diff-dev          Differential deviation factor for motion detection (motion error of        \n");
        fprintf(stderr,
                "                      one CC has to be superior to 'diff deviation' * 'standard deviation')  [%f]\n",
                def_diff_dev);
        fprintf(stderr,
                "  --track-all         Tracks all object types (star, meteor or noise)                            \n");
        fprintf(stderr,
                "  -h                  This help                                                                  \n");
        exit(1);
    }

    // Parsing Arguments
    int start_fra = args_find_int(argc, argv, "--fra-start", def_start_frame);
    int end_fra = args_find_int(argc, argv, "--end-fra", def_end_frame);
    int skip_fra = args_find_int(argc, argv, "--skip-fra", def_skip_fra);
    int light_min = args_find_int(argc, argv, "--light-min", def_light_min);
    int light_max = args_find_int(argc, argv, "--light-max", def_light_max);
    int surface_min = args_find_int(argc, argv, "--surface-min", def_surface_min);
    int surface_max = args_find_int(argc, argv, "--surface-max", def_surface_max);
    int k = args_find_int(argc, argv, "-k", def_k);
    int r_extrapol = args_find_int(argc, argv, "--r-extrapol", def_r_extrapol);
    float angle_max = args_find_float(argc, argv, "--angle-max", def_angle_max);
    int fra_star_min = args_find_int(argc, argv, "--fra-star-min", def_fra_star_min);
    int fra_meteor_min = args_find_int(argc, argv, "--fra-meteor-min", def_fra_meteor_min);
    int fra_meteor_max = args_find_int(argc, argv, "--fra-meteor-max", def_fra_meteor_max);
    float diff_dev = args_find_float(argc, argv, "--diff-dev", def_diff_dev);
    char* in_video = args_find_char(argc, argv, "--in-video", def_in_video);
    char* out_frames = args_find_char(argc, argv, "--out-frames", def_out_frames);
    char* out_bb = args_find_char(argc, argv, "--out-bb", def_out_bb);
    char* out_stats = args_find_char(argc, argv, "--out-stats", def_out_stats);
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
    printf("#  * in-video       = %s\n", in_video);
    printf("#  * out-bb         = %s\n", out_bb);
    printf("#  * out-frames     = %s\n", out_frames);
    printf("#  * out-stats      = %s\n", out_stats);
    printf("#  * fra-start      = %d\n", start_fra);
    printf("#  * end-fra        = %d\n", end_fra);
    printf("#  * skip-fra       = %d\n", skip_fra);
    printf("#  * light-min      = %d\n", light_min);
    printf("#  * light-max      = %d\n", light_max);
    printf("#  * surface-min    = %d\n", surface_min);
    printf("#  * surface-max    = %d\n", surface_max);
    printf("#  * k              = %d\n", k);
    printf("#  * r-extrapol     = %d\n", r_extrapol);
    printf("#  * angle-max      = %f\n", angle_max);
    printf("#  * fra-star-min   = %d\n", fra_star_min);
    printf("#  * fra-meteor-min = %d\n", fra_meteor_min);
    printf("#  * fra-meteor-max = %d\n", fra_meteor_max);
    printf("#  * diff-dev       = %4.2f\n", diff_dev);
    printf("#  * track-all      = %d\n", track_all);
    printf("#\n");

    if (!in_video) {
        fprintf(stderr, "(EE) '--in-video' is missing\n");
        exit(1);
    }
    if (fra_star_min < 2) {
        fprintf(stderr, "(EE) '--fra-star-min' has to be bigger than 1\n");
        exit(1);
    }
    if (fra_meteor_min < 2) {
        fprintf(stderr, "(EE) '--fra-meteor-min' has to be bigger than 1\n");
        exit(1);
    }
    if (fra_meteor_max < fra_meteor_min) {
        fprintf(stderr, "(EE) '--fra-meteor-max' has to be bigger than '--fra-meteor-min'\n");
        exit(1);
    }
    if (!out_frames)
        fprintf(stderr, "(II) '--out-frames' is missing -> no frames will be saved\n");
    if (!out_stats)
        fprintf(stderr, "(II) '--out-stats' is missing -> no stats will be saved\n");

    // sequence
    double theta, tx, ty;
    int frame;

    // allocations on the heap
    ROI_t* stats_tmp = (ROI_t*)malloc(MAX_ROI_SIZE * sizeof(ROI_t));
    track_t* tracks = (track_t*)malloc(MAX_TRACKS_SIZE * sizeof(track_t));
    BB_t** BB_array = (BB_t**)malloc(MAX_N_FRAMES * sizeof(BB_t*));
    ROI_buffer_t* ROI_buff = tracking_alloc_ROI_buffer(MAX(fra_star_min, fra_meteor_min));

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
    video_t* video = video_init_from_file(in_video, start_fra, end_fra, skip_fra, &i0, &i1, &j0, &j1);

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
    features_init_ROI(stats_tmp, MAX_ROI_SIZE);
    tracking_init_tracks(tracks, MAX_TRACKS_SIZE);
    tracking_init_BB_array(BB_array);
    CCL_data_t* ccl_data = CCL_LSL_init(i0, i1, j0, j1);
    for (int i = 0; i < ROI_buff->size; i++)
        features_init_ROI(ROI_buff->data[i], MAX_ROI_SIZE);

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
        frame = video->frame_current - 2;
        assert(frame < MAX_N_FRAMES);
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
        features_extract(ballon->SM32, i0, i1, j0, j1, stats_tmp, n1);

        PUTS("\t Step 3 : seuillage hysteresis && filter surface");
        features_merge_HI_CCL_v2(ballon->SH32, ballon->SM32, i0, i1, j0, j1, stats_tmp, n1, surface_min, surface_max);
        int n_shrink = features_shrink_stats(stats_tmp, ROI_buff->data[0], n1);

        PUTS("\t Step 4 : mise en correspondance");
        KPPV_match(kppv_data, ROI_buff->data[1], ROI_buff->data[0], n0, n_shrink, k);

        PUTS("\t Step 5 : recalage");
        features_motion(ROI_buff->data[1], ROI_buff->data[0], n0, n_shrink, &theta, &tx, &ty);

        PUTS("\t Step 6: tracking");
        tracking_perform(ROI_buff, tracks, BB_array, n0, n_shrink, frame, &tracks_cnt, &offset, theta, tx, ty,
                         r_extrapol, angle_max, diff_dev, track_all, fra_star_min, fra_meteor_min, fra_meteor_max);

        PUTS("\t [DEBUG] Saving frames");
        if (out_frames) {
            tools_create_folder(out_frames);
            tools_save_frame_ui32matrix(out_frames, ballon->SH32, i0, i1, j0, j1);
        }

        PUTS("\t [DEBUG] Saving stats");
        if (out_stats) {
            tools_create_folder(out_stats);
            KPPV_save_asso_conflicts(out_stats, frame, kppv_data, n0, n_shrink, ROI_buff->data[1], ROI_buff->data[0],
                                     tracks, tracks_cnt + 1);
            // tools_save_motion(path_motion, theta, tx, ty, frame-1);
            // tools_save_motionExtraction(path_extraction, ROI_buff->data[1], ROI_buff->data[0], n0, theta, tx, ty,
            //                             frame-1);
            // tools_save_error(path_error, ROI_buff->data[1], n0);
        }

        SWAP_UI8(ballon->I0, ballon->I1);
        tracking_rotate_ROI_buffer(ROI_buff);
        features_init_ROI(ROI_buff->data[0], MAX_ROI_SIZE);
        n0 = n_shrink;
        n_frames++;

        n_tracks = tracking_count_objects(tracks, (unsigned)tracks_cnt + 1, &n_stars, &n_meteors, &n_noise);
        fprintf(stderr, " -- Tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3d]\r", n_meteors, n_stars,
                n_noise, n_tracks);
        fflush(stderr);
    }
    fprintf(stderr, "\n");

    if (out_bb)
        tracking_save_array_BB(out_bb, BB_array, tracks, MAX_N_FRAMES, track_all);
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
    tracking_free_ROI_buffer(ROI_buff);
    free(stats_tmp);
    free(tracks);
    free(BB_array);

    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
