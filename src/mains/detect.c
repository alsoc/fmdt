#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <nrc2.h>

#include "fmdt/args.h"
#include "fmdt/tools.h"
#include "fmdt/macros.h"
#include "vec.h"

#include "fmdt/CCL/CCL.h"
#include "fmdt/features/features.h"
#include "fmdt/KNN/KNN.h"
#include "fmdt/motion/motion.h"
#include "fmdt/threshold/threshold.h"
#include "fmdt/tracking/tracking.h"
#include "fmdt/video/video.h"
#include "fmdt/image/image.h"

int main(int argc, char** argv) {
    // default values
    int def_p_fra_start = 0;
    int def_p_fra_end = 0;
    int def_p_fra_skip = 0;
    int def_p_light_min = 55;
    int def_p_light_max = 80;
    int def_p_surface_min = 3;
    int def_p_surface_max = 1000;
    int def_p_k = 3;
    int def_p_max_dist = 10;
    float def_p_min_ratio_s = 0.125f;
    int def_p_r_extrapol = 10;
    int def_p_extrapol_order = 3;
    float def_p_angle_max = 20;
    int def_p_fra_star_min = 15;
    int def_p_fra_meteor_min = 3;
    int def_p_fra_meteor_max = 100;
    float def_p_diff_dev = 4.f;
    char* def_p_in_video = NULL;
    char* def_p_out_frames = NULL;
    char* def_p_out_bb = NULL;
    char* def_p_out_stats = NULL;
    char* def_p_out_mag = NULL;
    int def_p_video_loop = 1;
    int def_p_ffmpeg_threads = 0;

    // help
    if (args_find(argc, argv, "-h")) {
        fprintf(stderr,
                "  --in-video          Path to video file or to an images sequence                            [%s]\n",
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
                "  --out-mag           Path to the file containing magnitudes of the tracked objects          [%s]\n",
                def_p_out_mag ? def_p_out_mag : "NULL");
        fprintf(stderr,
                "  --fra-start         Starting point of the video                                            [%d]\n",
                def_p_fra_start);
        fprintf(stderr,
                "  --fra-end           Ending point of the video                                              [%d]\n",
                def_p_fra_end);
        fprintf(stderr,
                "  --fra-skip          Number of skipped frames                                               [%d]\n",
                def_p_fra_skip);
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
                "  -k                  Maximum number of neighbors considered in k-NN algorithm               [%d]\n",
                def_p_k);
        fprintf(stderr,
                "  --max-dist          Maximum number of pixels between two images (in k-NN)                  [%d]\n",
                def_p_max_dist);
        fprintf(stderr,
                "  --min-ratio-s       Minimum surface ratio to match two CCs in k-NN                         [%f]\n",
                def_p_min_ratio_s);
        fprintf(stderr,
                "  --r-extrapol        Search radius for the next CC in case of extrapolation                 [%d]\n",
                def_p_r_extrapol);
        fprintf(stderr,
                "  --extrapol-order    Maximum number of frames to extrapolate objects (linear extrapolation) [%d]\n",
                def_p_extrapol_order);
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
                "  --video-buff        Bufferize all the video in global memory before executing the chain        \n");
        fprintf(stderr,
                "  --video-loop        Number of times the video is read in loop                              [%d]\n",
                def_p_video_loop);
        fprintf(stderr,
                "  --ffmpeg-threads    Select the number of threads to use to decode video input (in ffmpeg)  [%d]\n",
                def_p_ffmpeg_threads);
#ifdef OPENCV_LINK
        fprintf(stderr,
                "  --show-id           Show the ROI/CC ids on the ouptut frames                                   \n");
#endif
        fprintf(stderr,
                "  -h                  This help                                                                  \n");
        exit(1);
    }

    // parse arguments
    const int p_fra_start = args_find_int_min(argc, argv, "--fra-start", def_p_fra_start, 0);
    const int p_fra_end = args_find_int_min(argc, argv, "--fra-end", def_p_fra_end, 0);
    const int p_fra_skip = args_find_int_min(argc, argv, "--fra-skip", def_p_fra_skip, 0);
    const int p_light_min = args_find_int_min_max(argc, argv, "--light-min", def_p_light_min, 0, 255);
    const int p_light_max = args_find_int_min_max(argc, argv, "--light-max", def_p_light_max, 0, 255);
    const int p_surface_min = args_find_int_min(argc, argv, "--surface-min", def_p_surface_min, 0);
    const int p_surface_max = args_find_int_min(argc, argv, "--surface-max", def_p_surface_max, 0);
    const int p_k = args_find_int_min(argc, argv, "-k", def_p_k, 0);
    const int p_max_dist = args_find_int_min(argc, argv, "--max-dist", def_p_max_dist, 0);
    const float p_min_ratio_s = args_find_float_min_max(argc, argv, "--min-ratio-s", def_p_min_ratio_s, 0.f, 1.f);
    const int p_r_extrapol = args_find_int_min(argc, argv, "--r-extrapol", def_p_r_extrapol, 0);
    const int p_extrapol_order = args_find_int_min_max(argc, argv, "--extrapol-order", def_p_extrapol_order, 0, 255);
    const float p_angle_max = args_find_float_min_max(argc, argv, "--angle-max", def_p_angle_max, 0.f, 360.f);
    const int p_fra_star_min = args_find_int_min(argc, argv, "--fra-star-min", def_p_fra_star_min, 2);
    const int p_fra_meteor_min = args_find_int_min(argc, argv, "--fra-meteor-min", def_p_fra_meteor_min, 2);
    const int p_fra_meteor_max = args_find_int_min(argc, argv, "--fra-meteor-max", def_p_fra_meteor_max, 2);
    const float p_diff_dev = args_find_float_min(argc, argv, "--diff-dev", def_p_diff_dev, 0.f);
    const char* p_in_video = args_find_char(argc, argv, "--in-video", def_p_in_video);
    const char* p_out_frames = args_find_char(argc, argv, "--out-frames", def_p_out_frames);
    const char* p_out_bb = args_find_char(argc, argv, "--out-bb", def_p_out_bb);
    const char* p_out_stats = args_find_char(argc, argv, "--out-stats", def_p_out_stats);
    const char* p_out_mag = args_find_char(argc, argv, "--out-mag", def_p_out_mag);
    const int p_track_all = args_find(argc, argv, "--track-all");
    const int p_ffmpeg_threads = args_find_int_min(argc, argv, "--ffmpeg-threads", def_p_ffmpeg_threads, 0);
    const int p_video_buff = args_find(argc, argv, "--video-buff");
    const int p_video_loop = args_find_int_min(argc, argv, "--video-loop", def_p_video_loop, 1);
#ifdef OPENCV_LINK
    const int p_show_id = args_find(argc, argv, "--show-id");
#else
    const int p_show_id = 0;
#endif

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
    printf("#  * out-mag        = %s\n", p_out_mag);
    printf("#  * fra-start      = %d\n", p_fra_start);
    printf("#  * fra-end        = %d\n", p_fra_end);
    printf("#  * fra-skip       = %d\n", p_fra_skip);
    printf("#  * light-min      = %d\n", p_light_min);
    printf("#  * light-max      = %d\n", p_light_max);
    printf("#  * surface-min    = %d\n", p_surface_min);
    printf("#  * surface-max    = %d\n", p_surface_max);
    printf("#  * k              = %d\n", p_k);
    printf("#  * max-dist       = %d\n", p_max_dist);
    printf("#  * min-ratio-s    = %1.3f\n", p_min_ratio_s);
    printf("#  * r-extrapol     = %d\n", p_r_extrapol);
    printf("#  * extrapol-order = %d\n", p_extrapol_order);
    printf("#  * angle-max      = %f\n", p_angle_max);
    printf("#  * fra-star-min   = %d\n", p_fra_star_min);
    printf("#  * fra-meteor-min = %d\n", p_fra_meteor_min);
    printf("#  * fra-meteor-max = %d\n", p_fra_meteor_max);
    printf("#  * diff-dev       = %4.2f\n", p_diff_dev);
    printf("#  * track-all      = %d\n", p_track_all);
    printf("#  * video-buff     = %d\n", p_video_buff);
    printf("#  * video-loop     = %d\n", p_video_loop);
    printf("#  * ffmpeg-threads = %d\n", p_ffmpeg_threads);
#ifdef OPENCV_LINK
    printf("#  * show-id        = %d\n", p_show_id);
#endif
    printf("#\n");

    // arguments checking
    if (!p_in_video) {
        fprintf(stderr, "(EE) '--in-video' is missing\n");
        exit(1);
    }
    if (p_fra_meteor_max < p_fra_meteor_min) {
        fprintf(stderr, "(EE) '--fra-meteor-max' has to be bigger than '--fra-meteor-min'\n");
        exit(1);
    }
    if (p_fra_end && p_fra_end < p_fra_start) {
        fprintf(stderr, "(EE) '--fra-end' has to be higher than '--fra-start'\n");
        exit(1);
    }
    if (p_light_min > p_light_max) {
        fprintf(stderr, "(EE) '--light-max' has to be higher than '--light-min'\n");
        exit(1);
    }
#ifdef OPENCV_LINK
    if (p_show_id && !p_out_frames)
        fprintf(stderr, "(WW) '--show-id' has to be combined with the '--out-frames' parameter\n");
#endif

    // -------------------------- //
    // -- VIDEO INITIALISATION -- //
    // -------------------------- //

    int i0, i1, j0, j1; // image dimension (i0 = y_min, i1 = y_max, j0 = x_min, j1 = x_max)
    video_reader_t* video = video_reader_init(p_in_video, p_fra_start, p_fra_end, p_fra_skip, p_video_buff,
                                              p_ffmpeg_threads, &i0, &i1, &j0, &j1);
    video->loop_size = (size_t)(p_video_loop);

    // ---------------- //
    // -- ALLOCATION -- //
    // ---------------- //

    ROI_t* ROI_array_tmp = features_alloc_ROI(MAX_ROI_SIZE_BEFORE_SHRINK);
    ROI_t* ROI_array0 = features_alloc_ROI(MAX_ROI_SIZE);
    ROI_t* ROI_array1 = features_alloc_ROI(MAX_ROI_SIZE);
    vec_BB_t* BB_array = NULL;
    if (p_out_bb)
        BB_array = (vec_BB_t*)vector_create();
    tracking_data_t* tracking_data = tracking_alloc_data(MAX(p_fra_star_min, p_fra_meteor_min), MAX_ROI_SIZE);
    int b = 1; // image border
    uint8_t **I = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // grayscale input image
    uint8_t **IL = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // binary image (after threshold low)
    uint8_t **IH = ui8matrix(i0 - b, i1 + b, j0 - b, j1 + b); // binary image (after threshold high)
    uint32_t **L1 = ui32matrix(i0 - b, i1 + b, j0 - b, j1 + b); // labels (CCL)
    uint32_t **L2 = ui32matrix(i0 - b, i1 + b, j0 - b, j1 + b); // labels (CCL + hysteresis)

    // --------------------------- //
    // -- MATRIX INITIALISATION -- //
    // --------------------------- //

    tracking_init_global_data();
    KNN_data_t* knn_data = KNN_alloc_and_init_data(MAX_ROI_SIZE);
    features_init_ROI(ROI_array_tmp);
    features_init_ROI(ROI_array0);
    features_init_ROI(ROI_array1);
    tracking_init_data(tracking_data);
    CCL_data_t* ccl_data = CCL_LSL_alloc_and_init_data(i0, i1, j0, j1);
    zero_ui8matrix(I, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui8matrix(IL, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui32matrix(L1, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui8matrix(IH, i0 - b, i1 + b, j0 - b, j1 + b);
    zero_ui32matrix(L2, i0 - b, i1 + b, j0 - b, j1 + b);
    img_data_t* img_data = NULL;
    video_writer_t* video_writer = NULL;
    if (p_out_frames) {
        img_data = image_gs_alloc((j1 - j0) + 1, (i1 - i0) + 1);
        const size_t n_threads = 1;
        video_writer = video_writer_init(p_out_frames, p_fra_start, n_threads, (i1 - i0) + 1, (j1 - j0) + 1,
                                         PIXFMT_GRAY);
    }

    // ----------------//
    // -- PROCESSING --//
    // ----------------//

    printf("# The program is running...\n");
    size_t real_n_tracks = 0;
    unsigned n_frames = 0, n_stars = 0, n_meteors = 0, n_noise = 0;
    int cur_fra;
    while ((cur_fra = video_reader_get_frame(video, I)) != -1) {
        fprintf(stderr, "(II) Frame n°%4d", cur_fra);

        // step 1: threshold low
        threshold((const uint8_t**)I, IL, i0, i1, j0, j1, p_light_min);

        // step 2: CCL/CCA
        const int n_ROI = CCL_LSL_apply(ccl_data, (const uint8_t**)IL, L1);
        features_extract((const uint32_t**)L1, i0, i1, j0, j1, n_ROI, ROI_array_tmp->basic);

        // step 3: hysteresis threshold & surface filtering (+ magnitude computations)
        threshold((const uint8_t**)I, IH, i0, i1, j0, j1, p_light_max);
        features_merge_CCL_HI_v2((const uint32_t**)L1, (const uint8_t**)IH, L2, i0, i1, j0, j1, ROI_array_tmp->basic,
                                 p_surface_min, p_surface_max);
        features_shrink_ROI_array(ROI_array_tmp->basic, ROI_array1->basic);
        features_compute_magnitude((const uint8_t**)I, j1, i1, (const uint32_t**)L2, ROI_array1->basic,
                                   ROI_array1->misc);

        // step 4: k-NN matching
        KNN_match(knn_data, ROI_array0->basic, ROI_array1->basic, ROI_array0->asso, ROI_array1->asso, p_k, p_max_dist,
                  p_min_ratio_s);

        // step 5: motion estimation
        motion_t motion_est1, motion_est2;
        motion_compute(ROI_array0->basic, ROI_array1->basic, ROI_array1->asso, ROI_array1->motion, &motion_est1,
                       &motion_est2);

        // step 6: tracking
        tracking_perform(tracking_data, ROI_array1, &BB_array, cur_fra, &motion_est2, p_r_extrapol, p_angle_max,
                         p_diff_dev, p_track_all, p_fra_star_min, p_fra_meteor_min, p_fra_meteor_max, p_out_mag != NULL,
                         p_extrapol_order, p_min_ratio_s);

        // save frames (CCs)
        if (img_data) {
            image_gs_draw_labels(img_data, (const uint32_t**)L2, ROI_array1->basic, p_show_id);
            video_writer_save_frame(video_writer, (const uint8_t**)image_gs_get_pixels_2d(img_data));
        }

        // save stats
        if (p_out_stats) {
            tools_create_folder(p_out_stats);
            char filename[1024];
            snprintf(filename, sizeof(filename), "%s/%05d.txt", p_out_stats, cur_fra);
            FILE* f = fopen(filename, "w");
            if (f == NULL) {
                fprintf(stderr, "(EE) error while opening '%s'\n", filename);
                exit(1);
            }
            if (f) {
                int prev_fra = cur_fra > p_fra_start ? cur_fra - (p_fra_skip + 1) : -1;
                features_ROI0_ROI1_write(f, prev_fra, cur_fra, ROI_array0->basic, ROI_array0->misc, ROI_array1->basic,
                                         ROI_array1->misc, tracking_data->tracks);
                if (cur_fra > p_fra_start) {
                    fprintf(f, "#\n");
                    KNN_asso_conflicts_write(f, knn_data, ROI_array0->asso, ROI_array1->asso, ROI_array1->motion);
                    fprintf(f, "#\n");
                    motion_write(f, &motion_est1, &motion_est2);
                    fprintf(f, "#\n");
                    tracking_track_array_write_full(f, tracking_data->tracks);
                }
                fclose(f);
            } else {
                fprintf(stderr, "(WW) cannot open '%s' file.", filename);
            }
        }

        n_frames++;
        real_n_tracks = tracking_count_objects(tracking_data->tracks, &n_stars, &n_meteors, &n_noise);
        fprintf(stderr, " -- Tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3lu]\r", n_meteors, n_stars,
                n_noise, (unsigned long)real_n_tracks);
        fflush(stderr);

        ROI_t* tmp = ROI_array0;
        ROI_array0 = ROI_array1;
        ROI_array1 = tmp;
    }
    fprintf(stderr, "\n");

    if (BB_array) {
        FILE* f = fopen(p_out_bb, "w");
        if (f == NULL) {
            fprintf(stderr, "(EE) error while opening '%s'\n", p_out_bb);
            exit(1);
        }
        tracking_BB_array_write(f, BB_array, tracking_data->tracks);
        fclose(f);
    }

    if (p_out_mag) {
        FILE* f = fopen(p_out_mag, "w");
        if (f == NULL) {
            fprintf(stderr, "(EE) error while opening '%s'\n", p_out_mag);
            exit(1);
        }
        tracking_track_array_magnitude_write(f, tracking_data->tracks);
        fclose(f);
    }
    tracking_track_array_write(stdout, tracking_data->tracks);

    printf("# Tracks statistics:\n");
    printf("# -> Processed frames = %4d\n", n_frames);
    printf("# -> Detected tracks = ['meteor': %3d, 'star': %3d, 'noise': %3d, 'total': %3lu]\n", n_meteors, n_stars,
           n_noise, (unsigned long)real_n_tracks);

    // ----------
    // -- FREE --
    // ----------

    free_ui8matrix(I, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui8matrix(IL, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui32matrix(L1, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui8matrix(IH, i0 - b, i1 + b, j0 - b, j1 + b);
    free_ui32matrix(L2, i0 - b, i1 + b, j0 - b, j1 + b);
    features_free_ROI(ROI_array_tmp);
    features_free_ROI(ROI_array0);
    features_free_ROI(ROI_array1);
    video_reader_free(video);
    if (img_data) {
        image_gs_free(img_data);
        video_writer_free(video_writer);
    }
    CCL_LSL_free_data(ccl_data);
    KNN_free_data(knn_data);
    if (BB_array) {
        size_t vs = vector_size(BB_array);
        for (size_t i = 0; i < vs; i++)
            vector_free(BB_array[i]);
        vector_free(BB_array);
    }
    tracking_free_data(tracking_data);

    printf("# End of the program, exiting.\n");

    return EXIT_SUCCESS;
}
