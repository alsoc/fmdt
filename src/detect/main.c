#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <nrc2.h>

#include "args.h"
#include "defines.h"
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
                def_p_in_video);
        fprintf(stderr,
                "  --out-frames        Path to frames output folder                                           [%s]\n",
                def_p_out_frames);
        fprintf(stderr,
                "  --out-bb            Path to the file containing the bounding boxes (frame by frame)        [%s]\n",
                def_p_out_bb);
        fprintf(stderr,
                "  --out-stats         TODO! Path to folder                                                   [%s]\n",
                def_p_out_stats);
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
    printf("#  -----------------------\n");
    printf("# |          ----*        |\n");
    printf("# | --* METEOR-DETECT --* |\n");
    printf("# |   -------*            |\n");
    printf("#  -----------------------\n");
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

    // sequence
    double theta, tx, ty;
    int frame;
    int offset = 0;
    int tracks_cnt = -1;
    int n0 = 0, n1 = 0;
    // image
    int b = 1;
    int i0, i1, j0, j1;

    // ------------------------- //
    // -- INITIALISATION VIDEO-- //
    // ------------------------- //

    video_t* video = video_init_from_file(p_in_video, p_fra_start, p_fra_end, p_skip_fra, &i0, &i1, &j0, &j1);

    // ---------------- //
    // -- ALLOCATION -- //
    // ---------------- //

    ROI_t* stats_tmp = (ROI_t*)malloc(MAX_ROI_SIZE * sizeof(ROI_t));
    track_t* tracks = (track_t*)malloc(MAX_TRACKS_SIZE * sizeof(track_t));
    BB_t** BB_array = (BB_t**)malloc(MAX_N_FRAMES * sizeof(BB_t*));
    ROI_buffer_t* ROI_buff = tracking_alloc_ROI_buffer(MAX(p_fra_star_min, p_fra_meteor_min));
    uint8_t **I0 = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // frame
    uint8_t **SM = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // hysteresis
    uint8_t **SH = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // hysteresis
    uint32_t **SM32 = ui32matrix(i0 - b, i1 + b, j0 - b, j1 + b); // hysteresis
    uint32_t **SH32 = ui32matrix(i0 - b, i1 + b, j0 - b, j1 + b); // hysteresis

    // -------------------------- //
    // -- INITIALISATION MATRIX-- //
    // -------------------------- //

    tracking_init_global_data();
    KKPV_data_t* kppv_data = KPPV_init(0, MAX_KPPV_SIZE, 0, MAX_KPPV_SIZE);
    features_init_ROI(stats_tmp, MAX_ROI_SIZE);
    tracking_init_tracks(tracks, MAX_TRACKS_SIZE);
    tracking_init_BB_array(BB_array);
    CCL_data_t* ccl_data = CCL_LSL_init(i0, i1, j0, j1);
    for (int i = 0; i < ROI_buff->size; i++)
        features_init_ROI(ROI_buff->data[i], MAX_ROI_SIZE);
    zero_ui8matrix(I0, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui8matrix(SM, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui8matrix(SH, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui32matrix(SM32, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui32matrix(SH32, i0 - b, i1 + b, j0 - b, j1 + b);

    // ----------------//
    // -- TRAITEMENT --//
    // ----------------//

    printf("# The program is running...\n");
    unsigned n_frames = 0;
    unsigned n_tracks = 0, n_stars = 0, n_meteors = 0, n_noise = 0;
    while (video_get_next_frame(video, I0)) {
        frame = video->frame_current - 1;
        assert(frame < MAX_N_FRAMES);
        fprintf(stderr, "(II) Frame n°%4d", frame);

        // Step 1 : seuillage low/high
        tools_copy_ui8matrix_ui8matrix(I0, i0, i1, j0, j1, SH);
        tools_copy_ui8matrix_ui8matrix(I0, i0, i1, j0, j1, SM);
        threshold_high(SM, i0, i1, j0, j1, p_light_min);
        threshold_high(SH, i0, i1, j0, j1, p_light_max);
        tools_convert_ui8matrix_ui32matrix(SM, i0, i1, j0, j1, SM32);
        tools_convert_ui8matrix_ui32matrix(SH, i0, i1, j0, j1, SH32);

        // Step 2 : ECC/ACC
        n1 = CCL_LSL_apply(ccl_data, SM32, i0, i1, j0, j1);
        features_extract(SM32, i0, i1, j0, j1, stats_tmp, n1);

        // Step 3 : seuillage hysteresis && filter surface
        features_merge_HI_CCL_v2(SH32, SM32, i0, i1, j0, j1, stats_tmp, n1, p_surface_min, p_surface_max);
        int n_shrink = features_shrink_stats(stats_tmp, ROI_buff->data[0], n1);

        // Step 4 : mise en correspondance
        KPPV_match(kppv_data, ROI_buff->data[1], ROI_buff->data[0], n0, n_shrink, p_k);

        // Step 5 : recalage
        features_motion(ROI_buff->data[1], ROI_buff->data[0], n0, n_shrink, &theta, &tx, &ty);

        // Step 6: tracking
        tracking_perform(ROI_buff, tracks, BB_array, n0, n_shrink, frame, &tracks_cnt, &offset, theta, tx, ty,
                         p_r_extrapol, p_angle_max, p_diff_dev, p_track_all, p_fra_star_min, p_fra_meteor_min,
                         p_fra_meteor_max);

        // Saving frames
        if (p_out_frames) {
            tools_create_folder(p_out_frames);
            char filename[1024];
            sprintf(filename, "%s/%05d.pgm", p_out_frames, frame);
            tools_save_frame_ui32matrix(filename, SH32, i0, i1, j0, j1);
        }

        // Saving stats
        if (p_out_stats) {
            tools_create_folder(p_out_stats);
            KPPV_save_asso_conflicts(p_out_stats, frame, kppv_data, n0, n_shrink, ROI_buff->data[1], ROI_buff->data[0],
                                     tracks, tracks_cnt + 1);
            // tools_save_motion(path_motion, theta, tx, ty, frame-1);
            // tools_save_motionExtraction(path_extraction, ROI_buff->data[1], ROI_buff->data[0], n0, theta, tx, ty,
            //                             frame-1);
            // tools_save_error(path_error, ROI_buff->data[1], n0);
        }

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

    if (p_out_bb)
        tracking_save_array_BB(p_out_bb, BB_array, tracks, MAX_N_FRAMES, p_track_all);
    tracking_print_tracks(stdout, tracks, tracks_cnt + 1);

    printf("# Statistics:\n");
    printf("# -> Processed frames = %4d\n", n_frames);
    printf("# -> Detected tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3d]\n", n_meteors, n_stars,
           n_noise, n_tracks);

    // ----------
    // -- free --
    // ----------

    free_ui8matrix(I0, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui8matrix(SM, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui8matrix(SH, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui32matrix(SM32, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui32matrix(SH32, i0 - b, i1 + b, j0 - b, j1 + b);
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
