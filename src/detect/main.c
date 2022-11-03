#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <nrc2.h>

#include "fmdt/args.h"
#include "fmdt/defines.h"
#include "fmdt/CCL.h"
#include "fmdt/tools.h"
#include "fmdt/features.h"
#include "fmdt/KPPV.h"
#include "fmdt/threshold.h"
#include "fmdt/tracking.h"
#include "fmdt/video.h"
#include "fmdt/images.h"
#include "fmdt/macros.h"

int get_next_frame(video_t* video, images_t* images, uint8_t** I) {
    if (video)
        return video_get_next_frame(video, I);
    else if (images)
        return images_get_next_frame(images, I);
    return 0;
}

int main(int argc, char** argv) {
    // default values
    int def_p_fra_start = 0;
    int def_p_fra_end = MAX_N_FRAMES;
    int def_p_skip_fra = 0;
    int def_p_light_min = 55;
    int def_p_light_max = 80;
    int def_p_surface_min = 3;
    int def_p_surface_max = 1000;
    int def_p_k = 3;
    int def_p_r_extrapol = 5;
    float def_p_angle_max = 20;
    int def_p_fra_star_min = 15;
    int def_p_fra_meteor_min = 3;
    int def_p_fra_meteor_max = 100;
    float def_p_diff_dev = 4.f;
    char* def_p_in_video = NULL;
    char* def_p_out_frames = NULL;
    char* def_p_out_bb = NULL;
    char* def_p_out_stats = NULL;

    // Help
    if (args_find(argc, argv, "-h")) {
        fprintf(stderr,
                "  --in-video          Path to video file                                                     [%s]\n",
                def_p_in_video ? def_p_in_video : "NULL");
        fprintf(stderr,
                "  --out-frames        Path to frames output folder                                           [%s]\n",
                def_p_out_frames ? def_p_out_frames : "NULL");
        fprintf(stderr,
                "  --out-bb            Path to the file containing the bounding boxes (frame by frame)        [%s]\n",
                def_p_out_bb ? def_p_out_bb : "NULL");
        fprintf(stderr,
                "  --out-stats         Path of the output statistics, only required for debugging purpose     [%s]\n",
                def_p_out_stats ? def_p_out_stats : "NULL");
        fprintf(stderr,
                "  --fra-start         Starting point of the video                                            [%d]\n",
                def_p_fra_start);
        fprintf(stderr,
                "  --fra-end           Ending point of the video                                              [%d]\n",
                def_p_fra_end);
        fprintf(stderr,
                "  --skip-fra          Number of skipped frames                                               [%d]\n",
                def_p_skip_fra);
        fprintf(stderr,
                "  --light-min         Low hysteresis threshold (grayscale [0;255])                           [%d]\n",
                def_p_light_min);
        fprintf(stderr,
                "  --light-max         High hysteresis threshold (grayscale [0;255])                          [%d]\n",
                def_p_light_max);
        fprintf(stderr,
                "  --surface-min       Maximum area of the CC                                                 [%d]\n",
                def_p_surface_min);
        fprintf(stderr,
                "  --surface-max       Minimum area of the CC                                                 [%d]\n",
                def_p_surface_max);
        fprintf(stderr,
                "  -k                  Number of neighbours                                                   [%d]\n",
                def_p_k);
        fprintf(stderr,
                "  --r-extrapol        Search radius for the next CC in case of extrapolation                 [%d]\n",
                def_p_r_extrapol);
        fprintf(stderr,
                "  --angle-max         Tracking angle max between two consecutive meteor moving points        [%f]\n",
                def_p_angle_max);
        fprintf(stderr,
                "  --fra-star-min      Minimum number of frames required to track a star                      [%d]\n",
                def_p_fra_star_min);
        fprintf(stderr,
                "  --fra-meteor-min    Minimum number of frames required to track a meteor                    [%d]\n",
                def_p_fra_meteor_min);
        fprintf(stderr,
                "  --fra-meteor-max    Maximum number of frames required to track a meteor                    [%d]\n",
                def_p_fra_meteor_max);
        fprintf(stderr,
                "  --diff-dev          Differential deviation factor for motion detection (motion error of        \n");
        fprintf(stderr,
                "                      one CC has to be superior to 'diff deviation' * 'standard deviation')  [%f]\n",
                def_p_diff_dev);
        fprintf(stderr,
                "  --track-all         Tracks all object types (star, meteor or noise)                            \n");
        fprintf(stderr,
                "  -h                  This help                                                                  \n");
        exit(1);
    }

    // Parsing Arguments
    const int p_fra_start = args_find_int(argc, argv, "--fra-start", def_p_fra_start);
    const int p_fra_end = args_find_int(argc, argv, "--fra-end", def_p_fra_end);
    const int p_skip_fra = args_find_int(argc, argv, "--skip-fra", def_p_skip_fra);
    const int p_light_min = args_find_int(argc, argv, "--light-min", def_p_light_min);
    const int p_light_max = args_find_int(argc, argv, "--light-max", def_p_light_max);
    const int p_surface_min = args_find_int(argc, argv, "--surface-min", def_p_surface_min);
    const int p_surface_max = args_find_int(argc, argv, "--surface-max", def_p_surface_max);
    const int p_k = args_find_int(argc, argv, "-k", def_p_k);
    const int p_r_extrapol = args_find_int(argc, argv, "--r-extrapol", def_p_r_extrapol);
    const float p_angle_max = args_find_float(argc, argv, "--angle-max", def_p_angle_max);
    const int p_fra_star_min = args_find_int(argc, argv, "--fra-star-min", def_p_fra_star_min);
    const int p_fra_meteor_min = args_find_int(argc, argv, "--fra-meteor-min", def_p_fra_meteor_min);
    const int p_fra_meteor_max = args_find_int(argc, argv, "--fra-meteor-max", def_p_fra_meteor_max);
    const float p_diff_dev = args_find_float(argc, argv, "--diff-dev", def_p_diff_dev);
    const char* p_in_video = args_find_char(argc, argv, "--in-video", def_p_in_video);
    const char* p_out_frames = args_find_char(argc, argv, "--out-frames", def_p_out_frames);
    const char* p_out_bb = args_find_char(argc, argv, "--out-bb", def_p_out_bb);
    const char* p_out_stats = args_find_char(argc, argv, "--out-stats", def_p_out_stats);
    const int p_track_all = args_find(argc, argv, "--track-all");

    // heading display
    printf("#  ---------------------\n");
    printf("# |          ----*      |\n");
    printf("# | --* FMDT-DETECT --* |\n");
    printf("# |   -------*          |\n");
    printf("#  ---------------------\n");
    printf("#\n");
    printf("# Parameters:\n");
    printf("# -----------\n");
    printf("#  * in-video       = %s\n", p_in_video);
    printf("#  * out-bb         = %s\n", p_out_bb);
    printf("#  * out-frames     = %s\n", p_out_frames);
    printf("#  * out-stats      = %s\n", p_out_stats);
    printf("#  * fra-start      = %d\n", p_fra_start);
    printf("#  * fra-end        = %d\n", p_fra_end);
    printf("#  * skip-fra       = %d\n", p_skip_fra);
    printf("#  * light-min      = %d\n", p_light_min);
    printf("#  * light-max      = %d\n", p_light_max);
    printf("#  * surface-min    = %d\n", p_surface_min);
    printf("#  * surface-max    = %d\n", p_surface_max);
    printf("#  * k              = %d\n", p_k);
    printf("#  * r-extrapol     = %d\n", p_r_extrapol);
    printf("#  * angle-max      = %f\n", p_angle_max);
    printf("#  * fra-star-min   = %d\n", p_fra_star_min);
    printf("#  * fra-meteor-min = %d\n", p_fra_meteor_min);
    printf("#  * fra-meteor-max = %d\n", p_fra_meteor_max);
    printf("#  * diff-dev       = %4.2f\n", p_diff_dev);
    printf("#  * track-all      = %d\n", p_track_all);
    printf("#\n");

    // arguments checking
    if (!p_in_video) {
        fprintf(stderr, "(EE) '--in-video' is missing\n");
        exit(1);
    }
    if (p_fra_star_min < 2) {
        fprintf(stderr, "(EE) '--fra-star-min' has to be bigger than 1\n");
        exit(1);
    }
    if (p_fra_meteor_min < 2) {
        fprintf(stderr, "(EE) '--fra-meteor-min' has to be bigger than 1\n");
        exit(1);
    }
    if (p_fra_meteor_max < p_fra_meteor_min) {
        fprintf(stderr, "(EE) '--fra-meteor-max' has to be bigger than '--fra-meteor-min'\n");
        exit(1);
    }
    if ((p_fra_end - p_fra_start) > MAX_N_FRAMES) {
        fprintf(stderr, "(EE) '--fra-end' - '--fra-start' has to be lower than %d\n", MAX_N_FRAMES);
        exit(1);
    }
    if (p_fra_end < p_fra_start) {
        fprintf(stderr, "(EE) '--fra-end' has to be higher than '--fra-start'\n");
        exit(1);
    }
    if (!p_out_frames)
        fprintf(stderr, "(II) '--out-frames' is missing -> no frames will be saved\n");
    if (!p_out_stats)
        fprintf(stderr, "(II) '--out-stats' is missing -> no stats will be saved\n");

    // -------------------------- //
    // -- INITIALISATION VIDEO -- //
    // -------------------------- //

    int i0, i1, j0, j1; // image dimension (y_min, y_max, x_min, x_max)
    video_t* video = NULL;
    images_t* images = NULL;
    if (!tools_is_dir(p_in_video)) {
        const size_t n_ffmpeg_threads = 0; // 0 = use all the threads available
        video = video_init_from_file(p_in_video, p_fra_start, p_fra_end, p_skip_fra, n_ffmpeg_threads, &i0, &i1, &j0,
                                     &j1);
    } else {
        images = images_init_from_path(p_in_video, p_fra_start, p_fra_end, p_skip_fra);
        i0 = images->i0; i1 = images->i1; j0 = images->j0; j1 = images->j1;
    }

    // ---------------- //
    // -- ALLOCATION -- //
    // ---------------- //

    ROI_t* ROI_array_tmp = features_alloc_ROI_array(MAX_ROI_SIZE);
    ROI_t* ROI_array0 = features_alloc_ROI_array(MAX_ROI_SIZE);
    ROI_t* ROI_array1 = features_alloc_ROI_array(MAX_ROI_SIZE);
    track_t* track_array = tracking_alloc_track_array(MAX_TRACKS_SIZE);
    BB_t** BB_array = (BB_t**)malloc(MAX_N_FRAMES * sizeof(BB_t*));
    tracking_data_t* tracking_data = tracking_alloc_data(MAX(p_fra_star_min, p_fra_meteor_min), MAX_ROI_SIZE);
    int b = 1; // image border
    uint8_t **I = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // frame
    uint8_t **SM_0 = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // hysteresis
    uint8_t **SM_1 = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // hysteresis
    uint32_t **SM_2 = ui32matrix(i0 - b, i1 + b, j0 - b, j1 + b); // hysteresis
    uint8_t **SH_0 = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // hysteresis
    uint8_t **SH_1 = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // hysteresis
    uint8_t **SH_2 = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // hysteresis

    // -------------------------- //
    // -- INITIALISATION MATRIX-- //
    // -------------------------- //

    tracking_init_global_data();
    KKPV_data_t* kppv_data = KPPV_alloc_and_init_data(0, MAX_KPPV_SIZE, 0, MAX_KPPV_SIZE);
    features_init_ROI_array(ROI_array_tmp);
    features_init_ROI_array(ROI_array0);
    features_init_ROI_array(ROI_array1);
    tracking_init_track_array(track_array);
    tracking_init_BB_array(BB_array);
    tracking_init_data(tracking_data);
    CCL_data_t* ccl_data = CCL_LSL_alloc_and_init_data(i0, i1, j0, j1);
    zero_ui8matrix(I, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui8matrix(SM_0, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui8matrix(SM_1, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui32matrix(SM_2, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui8matrix(SH_0, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui8matrix(SH_1, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui8matrix(SH_2, i0 - b, i1 + b, j0 - b, j1 + b);

    // ----------------//
    // -- TRAITEMENT --//
    // ----------------//

    printf("# The program is running...\n");
    size_t real_n_tracks = 0;
    unsigned n_frames = 0, n_stars = 0, n_meteors = 0, n_noise = 0;
    while (get_next_frame(video, images, I)) {
        size_t frame = video ? video->frame_current - 1 : images->frame_current - (images->frame_skip + 1);
        assert(frame < MAX_N_FRAMES);
        fprintf(stderr, "(II) Frame n°%4lu", frame);

        // Step 1 : seuillage low/high
        tools_copy_ui8matrix_ui8matrix((const uint8_t**)I, i0, i1, j0, j1, SH_0);
        tools_copy_ui8matrix_ui8matrix((const uint8_t**)I, i0, i1, j0, j1, SM_0);
        threshold_high((const uint8_t**)SM_0, SM_1, i0, i1, j0, j1, p_light_min);
        threshold_high((const uint8_t**)SH_0, SH_1, i0, i1, j0, j1, p_light_max);

        // Step 2 : ECC/ACC
        const int n_ROI = CCL_LSL_apply(ccl_data, (const uint8_t**)SM_1, SM_2);
        features_extract((const uint32_t**)SM_2, i0, i1, j0, j1, n_ROI, ROI_array_tmp);

        // Step 3 : seuillage hysteresis && filter surface
        features_merge_HI_CCL_v2((const uint32_t**)SM_2, (const uint8_t**)SH_1, SH_2, i0, i1, j0, j1, ROI_array_tmp,
                                 p_surface_min, p_surface_max);
        features_init_ROI_array(ROI_array1); // TODO: this is overkill, need to understand why we need to do that
        features_shrink_ROI_array((const ROI_t*)ROI_array_tmp, ROI_array1);

        // Step 4 : mise en correspondance
        KPPV_match(kppv_data, ROI_array0, ROI_array1, p_k);

        // Step 5 : recalage
        double first_theta, first_tx, first_ty, first_mean_error, first_std_deviation;
        double theta, tx, ty, mean_error, std_deviation;
        features_compute_motion((const ROI_t*)ROI_array1, ROI_array0, &first_theta, &first_tx, &first_ty,
                                &first_mean_error, &first_std_deviation, &theta, &tx, &ty, &mean_error, &std_deviation);

        // Step 6: tracking
        tracking_perform(tracking_data, (const ROI_t*)ROI_array0, ROI_array1, track_array, BB_array, frame, theta, tx,
                         ty, mean_error, std_deviation, p_r_extrapol, p_angle_max, p_diff_dev, p_track_all,
                         p_fra_star_min, p_fra_meteor_min, p_fra_meteor_max);

        // Saving frames
        if (p_out_frames) {
            tools_create_folder(p_out_frames);
            char filename[1024];
            sprintf(filename, "%s/%05lu.pgm", p_out_frames, frame);
            tools_save_frame_ui8matrix(filename, (const uint8_t**)SH_2, i0, i1, j0, j1);
        }

        // Saving stats
        if (p_out_stats && n_frames) {
            tools_create_folder(p_out_stats);
            char filename[1024];
            sprintf(filename, "%s/%05lu_%05lu.txt", p_out_stats, frame - 1, frame);
            FILE* f = fopen(filename, "w");
            if (f) {
                features_ROI0_ROI1_write(f, frame, ROI_array0, ROI_array1, track_array);
                fprintf(f, "#\n");
                KPPV_asso_conflicts_write(f, kppv_data, ROI_array0);
                fprintf(f, "#\n");
                features_motion_write(f, first_theta, first_tx, first_ty, first_mean_error, first_std_deviation, theta,
                                      tx, ty, mean_error, std_deviation);
                fprintf(f, "#\n");
                tracking_track_array_write(f, track_array);
                fclose(f);
            } else {
                fprintf(stderr, "(WW) cannot open '%s' file.", filename);
            }
        }

        n_frames++;
        real_n_tracks = tracking_count_objects(track_array, &n_stars, &n_meteors, &n_noise);
        fprintf(stderr, " -- Tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3lu]\r", n_meteors, n_stars,
                n_noise, real_n_tracks);
        fflush(stderr);

        ROI_t* tmp = ROI_array0;
        ROI_array0 = ROI_array1;
        ROI_array1 = tmp;
    }
    fprintf(stderr, "\n");

    if (p_out_bb)
        tracking_save_array_BB(p_out_bb, BB_array, track_array, MAX_N_FRAMES, p_track_all);
    tracking_track_array_write(stdout, track_array);

    printf("# Tracks statistics:\n");
    printf("# -> Processed frames = %4d\n", n_frames);
    printf("# -> Detected tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3lu]\n", n_meteors, n_stars,
           n_noise, real_n_tracks);

    // ----------
    // -- FREE --
    // ----------

    free_ui8matrix(I, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui8matrix(SM_0, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui8matrix(SM_1, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui32matrix(SM_2, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui8matrix(SH_0, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui8matrix(SH_1, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui8matrix(SH_2, i0 - b, i1 + b, j0 - b, j1 + b);
    features_free_ROI_array(ROI_array_tmp);
    features_free_ROI_array(ROI_array0);
    features_free_ROI_array(ROI_array1);
    if (video)
        video_free(video);
    if (images)
        images_free(images);
    CCL_LSL_free_data(ccl_data);
    KPPV_free_data(kppv_data);
    tracking_free_BB_array(BB_array);
    tracking_free_track_array(track_array);
    tracking_free_data(tracking_data);
    free(BB_array);

    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
